
#include <Window.h>
#include <Camera.h>

#include <Geometries/Geometry.h>
#include <Geometries/Geometries.h>
#include <Texture.h>
#include <Text.h>

#include <Inputs/InputsMethods.h>
#include <Inputs/InputsEnums.h>
#include <client.h>
#include <thread>
#include "comElem.h"
#include "ServiceBus.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>


bool PlayerAlive = true;
bool Bullet_isShoot = false;

int main()
{
    ServiceBus bus;

    //Window creation
    sr::Window window(L"MultiPlayer", 1920, 1080);

    //Camera creation
    sr::Camera camera(sr::CameraType::PERSPECTIVE);
    camera.SetFOV(gce::PI / 4.0f);
    camera.SetFarPlane(750.0f);
    camera.SetNearPlane(0.001f);

    //Texture loading
    sr::Texture texture("res/Textures/uv.png");

    //Mesh creation
    sr::Cube floor;
    floor.SetTexture(texture);
    floor.SetPosition({ 0.0f, 0.0f, 0.0f });
    floor.SetScale({ 500.0f, 0.1f, 500.0f });

    sr::Cube cube;
    cube.SetColor({ 0.0f, 1.0f, 0.0f });
    cube.SetScale({ 0.5f, 0.5f, 1.5f });
    cube.SetRotation({ 0.0f, 0.0f, 90.0f });

    gce::Font font(L"Arial");

    sr::Text text;
    text.SetFont(&font);
    text.SetPosition({ 25.0f, 25.0f });
    text.SetColor(gce::Color::White);

    Client client;
    std::unordered_map<uint32_t, std::unique_ptr<sr::Cube>> remotePlayers;

    std::jthread connection_thread([&](std::stop_token st)
        {
            client.start(st, bus); 
        });


    auto lastNetTick = std::chrono::steady_clock::now();
	constexpr auto NET_INTERVAL = std::chrono::milliseconds(20);


    //Rendering Loop
    while (window.IsOpen())
    {

        WorldState worldState;

        {
            std::lock_guard lk(bus.WorldStateMutex);
            worldState = bus.worldState;
            if (worldState.hasSpawn)
            {
                cube.SetPosition(worldState.spawnPoint);
                bus.worldState.hasSpawn = false;
            }
        }
        std::unordered_set<uint32_t> spawnedPlayers;
        spawnedPlayers.reserve(worldState.ennemiesList.size());


        for (auto& e : worldState.ennemiesList)
        {
            spawnedPlayers.insert(e.id);

			auto it = remotePlayers.find(e.id);
            if (it == remotePlayers.end())
            {
                auto newPlayer = std::make_unique<sr::Cube>();
                newPlayer->SetColor({ 1.0f, 0.0f, 0.0f });
                newPlayer->SetScale({ 0.5f, 0.5f, 1.5f });
                newPlayer->SetRotation({ 0.0f, 0.0f, 90.0f });
                newPlayer->SetPosition(e.position);

                auto [insertedIt, ok] = remotePlayers.emplace(e.id, std::move(newPlayer));
                it = insertedIt;

            } else
            {
				it->second->SetPosition(e.position);
				it->second->SetRotation(e.rotation);
            }
        }
		for (auto it = remotePlayers.begin(); it != remotePlayers.end(); )
        {
            if (!spawnedPlayers.contains(it->first))
                it = remotePlayers.erase(it);
            else
                ++it;
        }


        window.Begin(camera);
        window.Draw(floor);

        window.Draw(cube);

		for (auto& [id, remotePlayer] : remotePlayers)
        {
            window.Draw(*remotePlayer);
        }

        window.DrawText(text);

        window.End();

        window.Display();

        //Game logic

        //Vector initialisation
        gce::Vector3f32 player_moov = {};
        gce::Vector3f32 player_rotation = {};
        gce::Vector3f32 player_fly = {};
        gce::Vector3f32 floor_pos = {};
        gce::Vector3f32 player_pos = {};
        gce::Vector3f32 player_rot = {};
        gce::Vector3f32 player_death_fall = {};
        gce::Vector3f32 player_death_moov = {};
        gce::Vector3f32 player_death_rot = {};

        if (sr::GetKey(sr::Keyboard::O))
        {
            PlayerAlive = true;
            camera.SetRotation({ 0.0f, 0.0f, 0.0f });
        }

        if (sr::GetKey(sr::Keyboard::P))
            PlayerAlive = false;

        //Player input
        player_moov.z = sr::GetKey(sr::Keyboard::Z) - sr::GetKey(sr::Keyboard::S);
        player_moov.x = sr::GetKey(sr::Keyboard::D) - sr::GetKey(sr::Keyboard::Q);
        player_rotation.y = sr::GetKey(sr::Keyboard::E) - sr::GetKey(sr::Keyboard::A);
        player_fly.y = sr::GetKey(sr::Keyboard::SPACEBAR) - sr::GetKey(sr::Keyboard::LCONTROL);

        //player shoot

        static bool wasShootPressed = false;
		bool shootPressed = sr::GetKey(sr::Keyboard::F);

		if (shootPressed && !wasShootPressed)
        {
            std::lock_guard lk(bus.shootMutex);
            bus.shootRequested = true;
        }
        wasShootPressed = shootPressed;

        floor_pos = floor.GetPosition();
        player_pos = cube.GetPosition();
        player_rot = cube.GetRotation();

        camera.SetPosition(player_pos);

        //Player alive condition
        if (PlayerAlive == true)
        {
            text.SetText(L"Bonjour ! Tu est PILOTE ... !!");
            player_moov.SelfNormalize();
            player_rotation.SelfNormalize();
            player_fly.SelfNormalize();
            player_moov *= 0.08f;
            player_rotation *= 0.008f;
            player_fly *= 0.08f;

            cube.Rotate(player_rotation);
            camera.Rotate(player_rotation);
            cube.Translate(player_moov*2);
            cube.Translate(player_fly/2);
        }

        //Player dead condition
        if (PlayerAlive == false)
        {
            text.SetText(L"Skill issue ! (X) pour quiter");
            player_death_fall.y = sr::GetMousePosition();
            player_death_fall.SelfNormalize();
            player_death_fall *= -0.008f;
            cube.Translate(player_death_fall);

            if (player_pos.y > floor_pos.y + 0.5f)
            {
                player_death_moov.z = sr::GetMousePosition();
                player_death_rot.z = sr::GetMousePosition();

                player_death_moov.SelfNormalize();
                player_death_rot.SelfNormalize();
                player_death_moov *= 0.006f;
                player_death_rot *= 0.01f;

                cube.Translate(player_death_moov);
                cube.Rotate(player_death_rot);
                camera.Rotate(player_death_rot);
            }
            if (sr::GetKey(sr::Keyboard::X))
            {
                {
                    std::lock_guard lk(bus.playerStateMutex);
                    bus.playerState.Alive = false;  
                    bus.outDirty = true;             
                }

                bus.player_condition_variable.notify_one();
                break;
            }
        }

        //Player win condition
        //if (PlayerAlive == true && Ennemie_Vector == false)
        //{
        //    text.SetText(L"Les ennemies c'est chaw !! (X) pour quiter");
        //    if (sr::GetKey(sr::Keyboard::X))
        //        break;
        //}

        //Player colision

        //Player colision y
        if (player_pos.y < floor_pos.y + 0.5f)
            cube.SetPosition({ player_pos.x, 0.5f, player_pos.z });
        if (player_pos.y > floor_pos.y + 60.0f)
            cube.SetPosition({ player_pos.x, 60.0f, player_pos.z });
        //Player colision z
        if (player_pos.z > floor_pos.z + 240.0f)
            cube.SetPosition({ player_pos.x, player_pos.y, 240.0f });
        if (player_pos.z < floor_pos.z - 240.0f)
            cube.SetPosition({ player_pos.x, player_pos.y, -240.0f });
        //Player colision x
        if (player_pos.x > floor_pos.x + 240.0f)
            cube.SetPosition({ 240.0f, player_pos.y, player_pos.z });
        if (player_pos.x < floor_pos.x - 240.0f)
            cube.SetPosition({ -240.0f, player_pos.y, player_pos.z });

        commsElement playerState{
            .position = cube.GetPosition(),
            .rotation = cube.GetRotation(),
            .Alive = PlayerAlive
        };

		auto now = std::chrono::steady_clock::now();
		if (now - lastNetTick >= NET_INTERVAL)
        {
			std::lock_guard lk(bus.playerStateMutex);
            bus.playerState = playerState;
			bus.outDirty = true;
            bus.player_condition_variable.notify_one();
            lastNetTick = now;
        }
    }
    return 0;
}
