// dllmain.cpp
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "includes/IL2CPP_Resolver.hpp" // your resolver location

// global variables
std::vector<Unity::CGameObject*> PlayerList;
std::vector<Unity::CGameObject*> MobList;
std::ofstream logFile;

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

// Function to print all Player and Mob instances' locations and rotations
void PrintAllInstances() {
    std::cout << "Printing Player instances:\n";
    for (auto& player : PlayerList) {
        auto transform = player->GetTransform();
        auto position = transform->GetPosition();
        auto rotation = transform->GetRotation().ToEuler();
        std::string name = player->GetName()->ToString();
        std::cout << "Player: " << name
            << " Position - X: " << position.x
            << ", Y: " << position.y
            << ", Z: " << position.z
            << " Rotation - X: " << rotation.x
            << ", Y: " << rotation.y
            << ", Z: " << rotation.z << std::endl;
    }

    std::cout << "Printing Mob instances:\n";
    for (auto& mob : MobList) {
        auto transform = mob->GetTransform();
        auto position = transform->GetPosition();
        auto rotation = transform->GetRotation().ToEuler();
        std::string name = mob->GetName()->ToString();
        std::cout << "Mob: " << name
            << " Position - X: " << position.x
            << ", Y: " << position.y
            << ", Z: " << position.z
            << " Rotation - X: " << rotation.x
            << ", Y: " << rotation.y
            << ", Z: " << rotation.z << std::endl;
    }
}

// Function to calculate the quaternion for rotation
/*
Unity::Quaternion LookAt(const Unity::Vector3& source, const Unity::Vector3& target) {
    Unity::Vector3 direction = (target - source).Normalize();
    Unity::Vector3 forward = { 0, 0, 1 };

    float dot = forward.Dot(direction);
    Unity::Vector3 cross = forward.Cross(direction);

    float w = sqrtf((forward.Length() * direction.Length())) + dot;
    Unity::Quaternion quaternion(cross.x, cross.y, cross.z, w);
    return quaternion.Normalized();
}
*/

// Function to rotate players to the closest mob for quick and easy aimbot
/*
void RotatePlayersToClosestMobs() {
    for (auto& player : PlayerList) {
        Unity::CGameObject* closestMob = nullptr;
        float minDistance = FLT_MAX;

        auto playerTransform = player->GetTransform();
        auto playerPosition = playerTransform->GetPosition();

        for (auto& mob : MobList) {
            auto mobTransform = mob->GetTransform();
            auto mobPosition = mobTransform->GetPosition();
            float distance = playerPosition.Distance(mobPosition);

            if (distance < minDistance) {
                minDistance = distance;
                closestMob = mob;
            }
        }

        if (closestMob) {
            auto mobTransform = closestMob->GetTransform();
            auto mobPosition = mobTransform->GetPosition();
            auto rotation = LookAt(playerPosition, mobPosition);
            playerTransform->SetRotation(rotation);
        }
    }
}
*/

// Function to cache instances of Player and Mob classes
void CacheInstances() {
    while (true) {
        PlayerList.clear();
        MobList.clear();
        std::cout << "Starting to cache instances...\n";
        auto playerList = Unity::Object::FindObjectsOfType<Unity::CComponent>("Player");
        auto mobList = Unity::Object::FindObjectsOfType<Unity::CComponent>("Mob");

        if (playerList) {
            std::cout << "Found " << playerList->m_uMaxLength << " Player objects.\n";
            for (int i = 0; i < playerList->m_uMaxLength; i++) {
                if (auto component = playerList->operator[](i)) {
                    if (auto gameObject = component->GetGameObject()) {
                        PlayerList.push_back(gameObject);
                    }
                }
            }
        }
        else {
            std::cout << "No Player objects found.\n";
        }

        if (mobList) {
            std::cout << "Found " << mobList->m_uMaxLength << " Mob objects.\n";
            for (int i = 0; i < mobList->m_uMaxLength; i++) {
                if (auto component = mobList->operator[](i)) {
                    if (auto gameObject = component->GetGameObject()) {
                        MobList.push_back(gameObject);
                    }
                }
            }
        }
        else {
            std::cout << "No Mob objects found.\n";
        }

        Sleep(1000);
    }
}

// Function to find and invoke the "Jump" method on all instances of the Player class
void InvokePlayerJump() {

    // Fetch the Player class
    auto playerClass = IL2CPP::Class::Find("Player");

    if (!playerClass) {
        std::cout << "Player class not found.\n";
        return;
    }

    // Get the method pointer for the "Jump" method
    void* jumpMethod = IL2CPP::Class::Utils::GetMethodPointer(playerClass, "Jump", -1);
    if (!jumpMethod) {
        std::cout << "Jump method not found.\n";
        return;
    }

    for (auto& player : PlayerList) {
        // Get the Player component instance
        auto playerInstance = player->GetComponent("Player");

        if (playerInstance) {
            // Invoke the "Jump" method
            reinterpret_cast<void(*)(void*)>(jumpMethod)(playerInstance);
            std::cout << "Jump method invoked successfully on " << player->GetName()->ToString() << ".\n";
        }
        else {
            std::cout << "Failed to get Player instance.\n";
        }
    }
}

// Function to set the baseMoveSpeed field on all instances of the Player class
void SetPlayerBaseMoveSpeed(float newSpeed) {

    // Fetch the Player class
    auto playerClass = IL2CPP::Class::Find("Player");

    if (!playerClass) {
        std::cout << "Player class not found.\n";
        return;
    }

    // Get the field offset for the baseMoveSpeed field
    int baseMoveSpeedOffset = IL2CPP::Class::Utils::GetFieldOffset(playerClass, "baseMoveSpeed");
    if (baseMoveSpeedOffset < 0) {
        std::cout << "baseMoveSpeed field not found.\n";
        return;
    }

    for (auto& player : PlayerList) {
        // Get the Player component instance
        auto playerInstance = player->GetComponent("Player");

        if (playerInstance) {
            // Set the baseMoveSpeed field
            *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(playerInstance) + baseMoveSpeedOffset) = newSpeed;
            std::cout << "baseMoveSpeed set to " << newSpeed << " for " << player->GetName()->ToString() << ".\n";
        }
        else {
            std::cout << "Failed to get Player instance.\n";
        }
    }
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

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CacheInstances, NULL, 0, NULL);

    while (true) {
        if (GetAsyncKeyState(VK_F1) & 1) {
            std::cout << "F1 pressed, printing all Player and Mob instances...\n";
            PrintAllInstances();
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            std::cout << "F2 pressed, invoking Jump on all Player instances...\n";
            InvokePlayerJump();
        }

        if (GetAsyncKeyState(VK_F3) & 1) {
            std::cout << "F3 pressed, setting baseMoveSpeed to 20 for all Player instances...\n";
            SetPlayerBaseMoveSpeed(20.0f); // float
        }

        Sleep(100);
    }
    return 0;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);  // Disable thread notifications to improve performance
        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
