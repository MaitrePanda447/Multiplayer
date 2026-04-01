#include "client.h"
#include <chrono>
#include <iostream>
#include <print>
#include <thread>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <WS2tcpip.h>


constexpr auto DEFAULT_BUFLEN = 512;
#define DEFAULT_PORT "27015";
std::mutex vecMutex;
std::vector<int> vector;
std::atomic_bool PlayerAlive = true;

static bool sendAll(SOCKET s, const char* data, int len)
{
    int total = 0;
    while (total < len)
    {
		std::cout << "Sending " << (len - total) << " bytes...\n";
        int sent = send(s, data + total, len - total, 0);
        if (sent == SOCKET_ERROR)
            return false;
        total += sent;
    }
    return true;
}


//Receive server infos
void Listener(std::stop_token st, SOCKET client_socket, ServiceBus& bus)
{
    char buffer[4090];
    std::string pending;

    bool inSnap = false;
    std::vector<std::string> snapLines;

    while (!st.stop_requested())
    {
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0)
        {
            pending.append(buffer, buffer + bytesReceived);

            size_t pos = 0;
            while ((pos = pending.find('\n')) != std::string::npos)
            {
                std::string line = pending.substr(0, pos);
                pending.erase(0, pos + 1);

                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (line.empty()) continue;

                if (line == "SNAP")
                {
                    inSnap = true;
                    snapLines.clear();
                    continue;
                }

                if (line == "END")
                {
                    std::unordered_map<uint32_t, ennemies> fresh;

                    for (const auto& l : snapLines)
                    {
                        uint32_t pid = 0;
                        float x = 0, y = 0, z = 0, rx = 0, ry = 0, rz = 0;
                        int deadInt = 0;

                        if (sscanf_s(l.c_str(), "P %u %f %f %f %f %f %f %d",
                            &pid, &x, &y, &z, &rx, &ry, &rz, &deadInt) == 8)
                        {
                            ennemies e{};
                            e.id = pid;
                            e.position = gce::Vector3f32(x, y, z);
                            e.rotation = gce::Vector3f32(rx, ry, rz);
                            e.alive = (deadInt == 0); 
                            fresh[pid] = e;
                        }
                        else
                        {
                            std::cerr << "Bad SNAP line: " << l << "\n";
                        }
                    }

                    {
                        std::scoped_lock lk(bus.WorldStateMutex);

                        bus.worldState.ennemiesById = std::move(fresh);

                        bus.worldState.ennemiesList.clear();
                        bus.worldState.ennemiesList.reserve(bus.worldState.ennemiesById.size());
                        for (auto& [id, e] : bus.worldState.ennemiesById)
                            bus.worldState.ennemiesList.push_back(e);
                    }

                    inSnap = false;
                    continue;
                }

                if (inSnap)
                {
                    snapLines.push_back(line);
                    continue;
                }

                if (line.rfind("SPAWN ", 0) == 0)
                {
                    float x = 0, y = 0, z = 0;
                    if (sscanf_s(line.c_str(), "SPAWN %f %f %f", &x, &y, &z) == 3)
                    {
                        std::scoped_lock lk(bus.WorldStateMutex);
                        bus.worldState.spawnPoint = gce::Vector3f32(x, y, z);
                        bus.worldState.hasSpawn = true;
                    }
                    continue;
                }

                if (line.rfind("YOU ", 0) == 0)
                {
                    uint32_t myId = 0;
                    if (sscanf_s(line.c_str(), "YOU %u", &myId) == 1)
                    {
                        std::scoped_lock lk(bus.WorldStateMutex);
                        bus.worldState.playerID = myId;
                        bus.worldState.hasYOU = true;
                    }
                    continue;
                }

                if (line == "DEAD")
                {
                    PlayerAlive = false;
                    std::scoped_lock lk(bus.WorldStateMutex);
                    bus.worldState.PlayerAlive = false;
                    continue;
                }

                // sinon ignore
            }
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Connection closed by server.\n";
            break;
        }
        else
        {
            std::cerr << "recv failed: " << WSAGetLastError() << "\n";
            break;
        }
    }
}

//Send client infos to server
static void client_thread(std::stop_token st, SOCKET server_socket, ServiceBus& bus)
{
	while (!st.stop_requested())
	{
		std::string message;
		{
			std::unique_lock lock(bus.playerStateMutex);
            std::chrono::milliseconds(200),[&] { return st.stop_requested() || bus.outDirty; };
			if (st.stop_requested())
				break;	

            const int aliveInt = bus.playerState.Alive ? 1 : 0;

			message = std::format("{:.2f},{:.2f},{:.2f};{:.2f},{:.2f},{:.2f};{}",
				bus.playerState.position.x, bus.playerState.position.y, bus.playerState.position.z,
				bus.playerState.rotation.x, bus.playerState.rotation.y, bus.playerState.rotation.z,
				aliveInt
			);
            message.push_back('\n');
			bus.outDirty = false;
		}
        bool doShoot = false;
        {
            std::scoped_lock lk(bus.shootMutex);
            if (bus.shootRequested)
            {
                doShoot = true;
                bus.shootRequested = false;
            }
        }
        if (doShoot)
        {
            const char* shootMsg = "shoot\n";
			if (!sendAll(server_socket, shootMsg, strlen(shootMsg)))
            {
                std::cerr << "send failed: " << WSAGetLastError() << "\n";
                break;
            }
        }

        if (!sendAll(server_socket, message.c_str(), (int)message.size()))
        {
            std::cerr << "send failed: " << WSAGetLastError() << "\n";
            break;
        }
    }
}

int Client::start(std::stop_token st, ServiceBus& bus)
{
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = nullptr,
		* ptr = nullptr,
		hints;
	const char* sendbuf = "BONJOUR ! Je suis le CLIENT !!";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed\n";
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	SOCKET clientSock = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	if (clientSock == INVALID_SOCKET)
	{
		std::cerr << "Create socket failed";
		WSACleanup();
		return 1;
	}

	sockaddr_in server{};
	server.sin_family = AF_INET;
	server.sin_port = htons(27015);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

	if (connect(clientSock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cerr << "connect() failed: " << WSAGetLastError() << "\n";
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}

	std::cout << "Connected to server. \n";

	std::jthread Sender_thread(client_thread, clientSock, std::ref(bus));
	std::jthread Listener_thread(Listener, clientSock, std::ref(bus));
	while (PlayerAlive)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		continue;
	}
    {
        std::lock_guard lk(bus.playerStateMutex);
        bus.outDirty = true; 
    }
    bus.player_condition_variable.notify_one();

	closesocket(clientSock);
	WSACleanup();
	return 0;
}

