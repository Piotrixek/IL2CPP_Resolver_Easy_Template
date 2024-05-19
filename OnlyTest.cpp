#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include "includes/IL2CPP_Resolver.hpp"
#include "includes/Unity/API/GameObject.hpp"
#include "helpers/Helpers.hpp"
#include "helpers/CacheInstances.hpp"
#include "helpers/AdvancedHelpers.hpp"

// global variables
std::vector<Unity::CGameObject*> PlayerList;
std::vector<Unity::CGameObject*> MobList;
std::ofstream logFile;

CacheManager cacheManager;

// Function to create a console window
void CreateConsole() {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    SetConsoleTitle("IL2CPP Console");
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONIN$", "r", stdin);
    std::cout << "Console created successfully.\n";
}

// Main thread function
DWORD WINAPI MainThread(LPVOID lpReserved) {
    CreateConsole();
    std::cout << "IL2CPP DLL Loaded" << std::endl;

    if (!IL2CPP::Initialize()) {
        std::cout << "IL2CPP initialization failed.\n";
        return 1;
    }

    IL2CPP::Callback::Initialize();

    cacheManager.AddCacheConfig("MVNetworkGame", PlayerList);
    cacheManager.AddCacheConfig("MVAvatarLocal", MobList);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)[](LPVOID) -> DWORD {
        cacheManager.StartCaching();
        return 0;
        }, NULL, 0, NULL);

    while (true) {
        if (GetAsyncKeyState(VK_F1) & 1) {
            std::cout << "F1 pressed, printing all Player and Mob instances...\n";
            PrintAllInstances(PlayerList, MobList);
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            std::cout << "F2 pressed, invoking Jump on all Player instances...\n";
            for (auto& player : PlayerList) {
                InvokeMethod<void>(player, "MVNetworkGame", "Cleanup");
            }
        }
        /*
        if (GetAsyncKeyState(VK_F3) & 1) {
            std::cout << "F3 pressed, setting walkSpeedBase to 120 for all AvatarMotor instances...\n";
            for (auto& mob : MobList) {
                SetFieldValue(mob, "AvatarMotor", "walkSpeed", 120.0f);
            }
        }
        */

        if (GetAsyncKeyState(VK_F4) & 1) {
            std::cout << "F4 pressed, getting static field value...\n";
            float someValue = GetStaticFieldValue<float>("SomeClass", "someStaticField");
            std::cout << "Static field value: " << someValue << std::endl;
        }

        if (GetAsyncKeyState(VK_F5) & 1) {
            std::cout << "F5 pressed, setting static field value...\n";
            SetStaticFieldValue("SomeClass", "someStaticField", 42.0f);
            std::cout << "Static field value set to 42.\n";
        }

        if (GetAsyncKeyState(VK_F6) & 1) {
            std::cout << "F6 pressed, getting property value...\n";
            for (auto& player : PlayerList) {
                float health = GetPropertyValue<float>(player, "MVAvatarLocal", "Health");
                std::cout << "Player Health: " << health << std::endl;
            }
        }

        if (GetAsyncKeyState(VK_F7) & 1) {
            std::cout << "F7 pressed, setting property value...\n";
            for (auto& player : PlayerList) {
                SetPropertyValue(player, "MVAvatarLocal", "Health", 100.0f);
                std::cout << "Player Health set to 100.\n";
            }
        }

        if (GetAsyncKeyState(VK_F8) & 1) {
            std::cout << "F8 pressed, cloning all players to their current positions...\n";
            for (auto& player : PlayerList) {
                auto playerPosition = player->GetTransform()->GetPosition();
                CloneGameObject(player, playerPosition);
                std::cout << "Cloned player to the same position.\n";
            }
        }

        if (GetAsyncKeyState(VK_F9) & 1) {
            std::cout << "F9 pressed, finding GameObject by name...\n";
            auto gameObject = FindGameObjectByName("Player");
            if (gameObject) {
                std::cout << "Found GameObject: " << gameObject->GetName()->ToString() << "\n";
            }
        }

        if (GetAsyncKeyState(VK_F10) & 1) {
            std::cout << "F10 pressed, finding all GameObjects with tag...\n";
            auto gameObjects = FindGameObjectsWithTag("Player");
            for (auto& obj : gameObjects) {
                std::cout << "Found GameObject with tag: " << obj->GetName()->ToString() << "\n";
                //DestroyGameObjectsByTag("Ground");
                auto playerPosition = obj->GetTransform()->GetPosition();
                CloneGameObject(obj, playerPosition);          
            }
        }

        /*

        if (GetAsyncKeyState(VK_F11) & 1) {
            std::cout << "F11 pressed, getting component by name...\n";
            for (auto& player : PlayerList) {
                auto component = GetComponent<Unity::CComponent>(player, "SomeComponent");
                if (component) {
                    std::cout << "Found component in Player.\n";
                }
            }
        }
        */

        if (GetAsyncKeyState(VK_F12) & 1) {
            std::cout << "F12 pressed, setting position of all players to {0.0f, 10.0f, 0.0f}...\n";
            Unity::Vector3 newPosition = { 0.0f, 10.0f, 0.0f };
            for (auto& player : PlayerList) {
                SetGameObjectPosition(player, newPosition);
                std::cout << "Set Player position to: " << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << "\n";
            }
        }
        /*
        if (GetAsyncKeyState(VK_OEM_3) & 1) { // ` key
            std::cout << "` key pressed, creating an instance of Player and invoking Initialize...\n";
            // Create the instance and invoke the method
            CreateInstanceAndInvoke<void>("Player", ".ctor", "Jump");

            // Find all instances of MVBuildModeAvatarLocal
            auto instances = FindAllInstancesByClass("Player");
            if (!instances.empty()) {
                std::cout << "Player instances found. Invoking Initialize on each...\n";
                for (auto& instance : instances) {
                    InvokeMethod<void>(instance, "Player", "Jump");
                    std::cout << "Initialize method invoked on an instance.\n";
                }
            }
            else {
                std::cout << "No Player instances found.\n";
            }
        }
        

        if (GetAsyncKeyState(VK_OEM_3) & 1) { // ` key
            std::cout << "` key pressed, invoking method on all Player instances using RVA...\n";
            uintptr_t rva = 0x3EED70;
            InvokeMethodOnAllInstancesByRVA<void>("Player", rva);
            std::cout << "Method invoked on all Player instances using RVA " << std::hex << rva << ".\n";
        }
        */
        /*

        if (GetAsyncKeyState(VK_F2) & 1) {
            std::cout << "F2 pressed, invoking Cleanup method on MVNetworkGame class statically...\n";
            InvokeStaticMethod<void>("MVNetworkGame", "Cleanup");
            std::cout << "Cleanup method invoked statically on MVNetworkGame class.\n";
        }

        if (GetAsyncKeyState(VK_F3) & 1) {
            std::cout << "F3 pressed, invoking Cleanup method on all MVNetworkGame instances using delegate...\n";
            for (auto& obj : PlayerList) {
                InvokeMethodByDelegate<void>(obj, "MVNetworkGame", "Cleanup");
            }
            std::cout << "Cleanup method invoked on all MVNetworkGame instances using delegate.\n";
        }

        if (GetAsyncKeyState(VK_F5) & 1) {
            std::cout << "F5 pressed, invoking Cleanup method on specific component of all instances...\n";
            for (auto& obj : PlayerList) {
                InvokeMethodOnComponent<void>(obj, "MVNetworkGameComponent", "Cleanup");
            }
            std::cout << "Cleanup method invoked on MVNetworkGameComponent of all instances.\n";
        }
        */

        if (GetAsyncKeyState(VK_F11) & 1) {
            std::cout << "F11 pressed, counting and printing all instances of a class...\n";
            CountAndPrintInstances("MVAvatarLocal");
        }

        if (GetAsyncKeyState(VK_OEM_3) & 1) { // ` key
            std::cout << "` key pressed, invoking ...\n";
            InvokeMethodOnTaggedGameObjects<void>("Player", "MVAvatarLocal", "Die");
            std::cout << "invoked.\n";
        }

        Sleep(100);
    }
    return 0;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
