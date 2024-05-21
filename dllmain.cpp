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

    cacheManager.AddCacheConfig("Player", PlayerList);
    cacheManager.AddCacheConfig("Mob", MobList);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)[](LPVOID) -> DWORD {
        cacheManager.StartCaching();
        return 0;
        }, NULL, 0, NULL);

    while (true) {
        /*
            if (GetAsyncKeyState(VK_F1) & 1) {
            // Assuming "AvatarMotor" is the instance class and "MVAvatarLocal" is the method class
            std::cout << "F2 pressed, invoking Die on all AvatarMotor instances...\n";
            FindInstancesAndInvokeMethod<void>("AvatarMotor", "MVAvatarLocal", "Die");
        }
        */
        if (GetAsyncKeyState(VK_F1) & 1) {
            std::cout << "F1 pressed, printing all Player and Mob instances...\n";
            PrintAllInstances(PlayerList, MobList);
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            std::cout << "F2 pressed, invoking Jump on all Player instances...\n";
            for (auto& player : PlayerList) {
                InvokeMethod<void>(player, "Player", "Jump");
            }
        }

        if (GetAsyncKeyState(VK_F3) & 1) {
            std::cout << "F3 pressed, setting baseMoveSpeed to 20 for all Player instances...\n";
            for (auto& player : PlayerList) {
                SetFieldValue(player, "Player", "baseMoveSpeed", 20.0f);
            }
        }

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
                float health = GetPropertyValue<float>(player, "Player", "Health");
                std::cout << "Player Health: " << health << std::endl;
            }
        }

        if (GetAsyncKeyState(VK_F7) & 1) {
            std::cout << "F7 pressed, setting property value...\n";
            for (auto& player : PlayerList) {
                SetPropertyValue(player, "Player", "Health", 100.0f);
                std::cout << "Player Health set to 100.\n";
            }
        }

        if (GetAsyncKeyState(VK_F8) & 1) {
            std::cout << "F8 pressed, cloning the first player to the same position...\n";
            if (!PlayerList.empty()) {
                auto playerPosition = PlayerList[0]->GetTransform()->GetPosition();
                CloneGameObject(PlayerList[0], playerPosition);
                std::cout << "Cloned the player to the same position.\n";
            }
            else {
                std::cout << "No player found to clone.\n";
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
            }
        }

        if (GetAsyncKeyState(VK_F11) & 1) {
            std::cout << "F11 pressed, getting component by name...\n";
            if (!PlayerList.empty()) {
                auto component = GetComponent<Unity::CComponent>(PlayerList[0], "SomeComponent");
                if (component) {
                    std::cout << "Found component in Player.\n";
                }
            }
        }

        if (GetAsyncKeyState(VK_F12) & 1) {
            std::cout << "F12 pressed, setting position of the first player...\n";
            if (!PlayerList.empty()) {
                Unity::Vector3 newPosition = { 0.0f, 10.0f, 0.0f };
                SetGameObjectPosition(PlayerList[0], newPosition);
                std::cout << "Set Player position to: " << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << "\n";
            }
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
