//server

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <format>
#include <random>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>

// --- Config ---
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

static std::atomic_bool shut = false;

// --- RNG Handler ---
std::mt19937 rng{ std::random_device{}() };
std::uniform_real_distribution<float> distX(-250.f, 250.f);
std::uniform_real_distribution<float> distY(10.f, 100.f);
std::uniform_real_distribution<float> distZ(-250.f, 250.f);
std::mutex rngMutex;

// --- Game State ---
struct Player
{
    uint32_t id = 0;
    float x = 0, y = 0, z = 0;
    float rx = 0, ry = 0, rz = 0;
    bool dead = false;

    sockaddr_in addr{};
    int addrLen = sizeof(sockaddr_in);
    bool hasAddr = false;

    uint32_t heartbeat = 0;
};

struct GameData
{
    std::mutex GameDataMutex;
    std::unordered_map<uint32_t, Player> players;

    std::unordered_map<std::string, uint32_t> addrToId;
};

GameData game_data;

static SOCKET ListenSocket = INVALID_SOCKET;

// --- Helpers ---
static std::string addrKey(const sockaddr_in& a)
{
    char ip[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, (void*)&a.sin_addr, ip, sizeof(ip));
    uint16_t port = ntohs(a.sin_port);
    return std::string(ip) + ":" + std::to_string(port);
}

static uint64_t nowMS()
{
    using namespace  std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());

}

static void sendLine(const sockaddr_in& to, const std::string& line)
{
    sendto(ListenSocket, line.c_str(), (int)line.size(), 0, (const sockaddr*)&to, sizeof(to));
}

// --- Shoot ---
void shootLaser(const Player& shooter)
{
    constexpr float MAX_RANGE = 250.0f;
    constexpr float HIT_RADIUS = 1.2f;

    if (shooter.dead) return;

    const float pitch = shooter.rx; // radians
    const float yaw = shooter.ry; // radians

    float dirX = sinf(yaw) * cosf(pitch);
    float dirY = sinf(pitch);
    float dirZ = cosf(yaw) * cosf(pitch);

    float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (len < 0.0001f) return;
    dirX /= len; dirY /= len; dirZ /= len;

    const float startX = shooter.x;
    const float startY = shooter.y;
    const float startZ = shooter.z;

    uint32_t bestId = 0;
    float bestT = 1e30f;
    sockaddr_in victimAddr{};
    bool hasVictimAddr = false;

    {
        std::scoped_lock lk(game_data.GameDataMutex);

        for (auto& [pid, target] : game_data.players)
        {
            if (pid == shooter.id || target.dead) continue;

            float toTargetX = target.x - startX;
            float toTargetY = target.y - startY;
            float toTargetZ = target.z - startZ;

            float t = (toTargetX * dirX + toTargetY * dirY + toTargetZ * dirZ);
            if (t < 0.0f || t > MAX_RANGE) continue;

            float closestX = startX + t * dirX;
            float closestY = startY + t * dirY;
            float closestZ = startZ + t * dirZ;

            float dx = target.x - closestX;
            float dy = target.y - closestY;
            float dz = target.z - closestZ;

            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq <= HIT_RADIUS * HIT_RADIUS)
            {
                if (t < bestT)
                {
                    bestT = t;
                    bestId = pid;
                }
            }
        }

        if (bestId != 0)
        {
            auto it = game_data.players.find(bestId);
            if (it != game_data.players.end() && !it->second.dead)
            {
                it->second.dead = true;
                victimAddr = it->second.addr;
                hasVictimAddr = it->second.hasAddr;
            }
        }
    }

    if (bestId != 0)
    {
        std::cout << "Player " << shooter.id << " hit Player " << bestId << "!\n";
        if (hasVictimAddr)
            sendLine(victimAddr, "DEAD\n");
    }
}

// --- SnapShot Thread ---
static void snapshotTick(std::stop_token st)
{
    using namespace std::chrono;
    constexpr auto interval = 50ms;

    while (!st.stop_requested() && !shut)
    {
        std::vector<Player> playersCopy;
        {
            std::scoped_lock lk(game_data.GameDataMutex);
            playersCopy.reserve(game_data.players.size());
            for (auto& [pid, p] : game_data.players)
                playersCopy.push_back(p);
        }

        for (const auto& target : playersCopy)
        {
            if (!target.hasAddr) continue;

            std::string out;
            out += "SNAP\n";
            for (const auto& p : playersCopy)
            {
                if (p.id == target.id) continue;
                out += std::format("P {} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {}\n",
                    p.id, p.x, p.y, p.z, p.rx, p.ry, p.rz, p.dead ? 1 : 0);
            }
            out += "END\n";

            int sent = sendto(ListenSocket, out.c_str(), (int)out.size(), 0,
                (const sockaddr*)&target.addr, sizeof(target.addr));
            if (sent == SOCKET_ERROR)
            {
                std::cout << "snapshot send failed: " << WSAGetLastError() << "\n";
            }
        }

        std::this_thread::sleep_for(interval);
    }
}

static void snapshotKick(std::stop_token st)
{
	constexpr  uint64_t TIMEOUT_MS = 3000; // 3 seconds
    using namespace  std::chrono;
	constexpr  auto interval = 500ms;

    while (!st.stop_requested() && !shut)
    {
        const uint64_t t = nowMS();
        {
            std::scoped_lock lk(game_data.GameDataMutex);
            for (auto it = game_data.players.begin(); it != game_data.players.end(); )
            {
                if (t - it->second.heartbeat > TIMEOUT_MS)
                {
                    std::cout << "Kicking player " << it->second.id << " due to timeout.\n";
                    it = game_data.players.erase(it);
                }
                else
                {
                    ++it;
                }
            }
		}
		std::this_thread::sleep_for(interval);
    }

}

int main(void)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* result = nullptr;
    int iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        std::cout << "getaddrinfo failed: " << iResult << "\n";
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        std::cout << "socket failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        std::cout << "bind failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    std::cout << "UDP Server listening on port " << DEFAULT_PORT << "...\n";

    std::jthread snapshotThread(snapshotTick);
    std::jthread snapshotKickThread(snapshotKick);

    char buf[4096];

    while (!shut)
    {
        sockaddr_in from{};
        int fromLen = sizeof(from);

        int n = recvfrom(ListenSocket, buf, (int)sizeof(buf) - 1, 0, (sockaddr*)&from, &fromLen);
        if (n <= 0)
            continue;

        buf[n] = '\0';
        std::string msg(buf, buf + n);

        if (!msg.empty() && msg.back() == '\r') msg.pop_back();
        if (!msg.empty() && msg.back() == '\n') msg.pop_back();

        // --- Register / retrieve player by addr ---
        uint32_t id = 0;
        Player shooterCopy;

        {
            std::scoped_lock lk(game_data.GameDataMutex);

            const std::string key = addrKey(from);
            auto itId = game_data.addrToId.find(key);

            if (itId == game_data.addrToId.end())
            {
                static uint32_t nextId = 1;
                id = nextId++;

                Player p{};
                p.id = id;
                {
                    std::lock_guard rlk(rngMutex);
                    p.x = distX(rng);
                    p.y = distY(rng);
                    p.z = distZ(rng);
                }
                p.addr = from;
                p.addrLen = fromLen;
                p.hasAddr = true;

                game_data.players[id] = p;
                game_data.addrToId[key] = id;

                sendLine(from, std::format("YOU {}\n", id));
                sendLine(from, std::format("SPAWN {:.2f} {:.2f} {:.2f}\n", p.x, p.y, p.z));
            }
            else
            {
                id = itId->second;
                auto& p = game_data.players[id];
                p.addr = from;
                p.addrLen = fromLen;
                p.hasAddr = true;
            }

            shooterCopy = game_data.players[id];
        }

        // --- Commands ---
        if (msg == "shoot")
        {
            shootLaser(shooterCopy);
            continue;
        }
        if (msg == "/quit")
        {
            std::scoped_lock lk(game_data.GameDataMutex);
            game_data.players.erase(id);
            continue;
        }

        // --- Position update ---
        float px, py, pz, rx, ry, rz;
        int aliveInt = 1;

        if (sscanf_s(msg.c_str(), "%f,%f,%f;%f,%f,%f;%d", &px, &py, &pz, &rx, &ry, &rz, &aliveInt) == 7)
        {
            std::scoped_lock lk(game_data.GameDataMutex);
            auto it = game_data.players.find(id);
            if (it != game_data.players.end())
            {
                it->second.heartbeat = nowMS();
                it->second.x = px; it->second.y = py; it->second.z = pz;
                it->second.rx = rx; it->second.ry = ry; it->second.rz = rz;
                it->second.dead = (aliveInt == 0);
            }
        }
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}