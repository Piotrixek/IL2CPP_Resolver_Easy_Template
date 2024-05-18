#pragma once
#include <iostream>
#include <string>
#include "../includes/IL2CPP_Resolver.hpp"
#include "../includes/Unity/API/GameObject.hpp"

// Helper function to invoke a static method
template<typename Ret, typename... Args>
Ret InvokeStaticMethod(const std::string& className, const std::string& methodName, Args... args) {
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

    return reinterpret_cast<Ret(*)(Args...)>(methodPointer)(args...);
}

// Helper function to get the value of a static field
template<typename T>
T GetStaticFieldValue(const std::string& className, const std::string& fieldName) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return T();
    }

    void* value = IL2CPP::Class::Utils::GetStaticField(il2cppClass, fieldName.c_str());
    if (!value) {
        std::cerr << "Field " << fieldName << " not found in class " << className << ".\n";
        return T();
    }

    return *reinterpret_cast<T*>(value);
}

// Helper function to set the value of a static field
template<typename T>
void SetStaticFieldValue(const std::string& className, const std::string& fieldName, const T& value) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return;
    }

    IL2CPP::Class::Utils::SetStaticField(il2cppClass, fieldName.c_str(), (void*)&value);
}

// Helper function to get a property value
template<typename T>
T GetPropertyValue(Unity::CGameObject* obj, const std::string& className, const std::string& propertyName) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return T();
    }

    auto instance = obj->GetComponent(className.c_str());
    if (!instance) {
        std::cerr << "Failed to get instance of class " << className << ".\n";
        return T();
    }

    return reinterpret_cast<T(*)(void*)>(IL2CPP::Class::Utils::GetMethodPointer(il2cppClass, ("get_" + propertyName).c_str()))(instance);
}

// Helper function to set a property value
template<typename T>
void SetPropertyValue(Unity::CGameObject* obj, const std::string& className, const std::string& propertyName, const T& value) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return;
    }

    auto instance = obj->GetComponent(className.c_str());
    if (!instance) {
        std::cerr << "Failed to get instance of class " << className << ".\n";
        return;
    }

    reinterpret_cast<void(*)(void*, T)>(IL2CPP::Class::Utils::GetMethodPointer(il2cppClass, ("set_" + propertyName).c_str()))(instance, value);
}
