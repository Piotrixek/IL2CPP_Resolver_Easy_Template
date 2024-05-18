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
                SetFieldValue<float>(player, "Player", "baseMoveSpeed", 20.0f);
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
