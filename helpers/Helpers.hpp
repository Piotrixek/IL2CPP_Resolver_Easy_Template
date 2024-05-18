#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../includes/IL2CPP_Resolver.hpp"
#include "../includes/Unity/API/GameObject.hpp"

// Helper function to invoke a method
template<typename Ret, typename... Args>
Ret InvokeMethod(Unity::CGameObject* obj, const std::string& className, const std::string& methodName, Args... args) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return Ret();
    }

    void* methodPointer = IL2CPP::Class::Utils::GetMethodPointer(il2cppClass, methodName.c_str(), sizeof...(args));
    if (!methodPointer) {
        std::cerr << "Method " << methodName << " not found in class " << className << ".\n";
        return Ret();
    }

    auto instance = obj->GetComponent(className.c_str());
    if (!instance) {
        std::cerr << "Failed to get instance of class " << className << ".\n";
        return Ret();
    }

    return reinterpret_cast<Ret(*)(void*, Args...)>(methodPointer)(instance, args...);
}

// Helper function to set a field value
template<typename T>
void SetFieldValue(Unity::CGameObject* obj, const std::string& className, const std::string& fieldName, const T& value) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return;
    }

    int fieldOffset = IL2CPP::Class::Utils::GetFieldOffset(il2cppClass, fieldName.c_str());
    if (fieldOffset < 0) {
        std::cerr << "Field " << fieldName << " not found in class " << className << ".\n";
        return;
    }

    auto instance = obj->GetComponent(className.c_str());
    if (!instance) {
        std::cerr << "Failed to get instance of class " << className << ".\n";
        return;
    }

    *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(instance) + fieldOffset) = value;
}

// Helper function to print all instances
void PrintAllInstances(const std::vector<Unity::CGameObject*>& PlayerList, const std::vector<Unity::CGameObject*>& MobList) {
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
