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

// Helper function to clone a GameObject and set its position
Unity::CGameObject* CloneGameObject(Unity::CGameObject* obj, const Unity::Vector3& position) {
    auto il2cppClass = IL2CPP::Class::Find("UnityEngine.Object");
    if (!il2cppClass) {
        std::cerr << "Class UnityEngine.Object not found.\n";
        return nullptr;
    }

    void* instantiateMethod = IL2CPP::Class::Utils::GetMethodPointer(il2cppClass, "Instantiate", 1);
    if (!instantiateMethod) {
        std::cerr << "Method Instantiate not found in class UnityEngine.Object.\n";
        return nullptr;
    }

    auto clonedObj = reinterpret_cast<Unity::CGameObject * (*)(void*)>(instantiateMethod)(obj);
    if (!clonedObj) {
        std::cerr << "Failed to clone object.\n";
        return nullptr;
    }

    auto transform = clonedObj->GetTransform();
    transform->SetPosition(position);

    return clonedObj;
}

// Helper function to find a GameObject by name
Unity::CGameObject* FindGameObjectByName(const std::string& name) {
    auto gameObject = Unity::GameObject::Find(name.c_str());
    if (!gameObject) {
        std::cerr << "GameObject " << name << " not found.\n";
    }
    return gameObject;
}

// Helper function to find all GameObjects with a specific tag
std::vector<Unity::CGameObject*> FindGameObjectsWithTag(const std::string& tag) {
    auto gameObjectsArray = Unity::GameObject::FindWithTag(tag.c_str());
    std::vector<Unity::CGameObject*> gameObjects;
    if (!gameObjectsArray) {
        std::cerr << "No GameObjects found with tag " << tag << ".\n";
        return gameObjects;
    }
    for (size_t i = 0; i < gameObjectsArray->m_uMaxLength; ++i) {
        gameObjects.push_back(gameObjectsArray->operator[](i));
    }
    return gameObjects;
}

// Helper function to get a component by name
template<typename T>
T* GetComponent(Unity::CGameObject* obj, const std::string& componentName) {
    auto component = obj->GetComponent(componentName.c_str());
    if (!component) {
        std::cerr << "Component " << componentName << " not found in GameObject.\n";
        return nullptr;
    }
    return reinterpret_cast<T*>(component);
}

// Helper function to get all components by name
template<typename T>
std::vector<T*> GetComponents(Unity::CGameObject* obj, const std::string& componentName) {
    auto componentsArray = obj->GetComponents(componentName.c_str());
    std::vector<T*> components;
    if (!componentsArray) {
        std::cerr << "No components found with name " << componentName << " in GameObject.\n";
        return components;
    }
    for (size_t i = 0; i < componentsArray->m_uMaxLength; ++i) {
        components.push_back(reinterpret_cast<T*>(componentsArray->operator[](i)));
    }
    return components;
}

// Helper function to set the position of a GameObject
void SetGameObjectPosition(Unity::CGameObject* obj, const Unity::Vector3& position) {
    auto transform = obj->GetTransform();
    transform->SetPosition(position);
}

// Helper function to get the position of a GameObject
Unity::Vector3 GetGameObjectPosition(Unity::CGameObject* obj) {
    auto transform = obj->GetTransform();
    return transform->GetPosition();
}

// Helper function to set the rotation of a GameObject
void SetGameObjectRotation(Unity::CGameObject* obj, const Unity::Quaternion& rotation) {
    auto transform = obj->GetTransform();
    transform->SetRotation(rotation);
}

// Helper function to get the rotation of a GameObject
Unity::Quaternion GetGameObjectRotation(Unity::CGameObject* obj) {
    auto transform = obj->GetTransform();
    return transform->GetRotation();
}

// Helper function to destroy all instances of a class
void DestroyInstancesByClass(const std::string& className) {
    auto classType = IL2CPP::Class::Find(className.c_str());
    if (!classType) {
        std::cerr << "Class " << className << " not found.\n";
        return;
    }

    auto instancesArray = Unity::Object::FindObjectsOfType<Unity::CObject>(className.c_str());
    if (!instancesArray || instancesArray->m_uMaxLength == 0) {
        std::cerr << "No instances of class " << className << " found.\n";
        return;
    }

    for (size_t i = 0; i < instancesArray->m_uMaxLength; ++i) {
        auto instance = instancesArray->operator[](i);
        if (instance) {
            instance->Destroy();
            std::cout << "Destroyed instance of class " << className << ".\n";
        }
    }
}

// Helper function to destroy all GameObjects with a specific tag
void DestroyGameObjectsByTag(const std::string& tag) {
    auto gameObjectsArray = Unity::GameObject::FindWithTag(tag.c_str());
    if (!gameObjectsArray) {
        std::cerr << "No GameObjects found with tag " << tag << ".\n";
        return;
    }

    for (size_t i = 0; i < gameObjectsArray->m_uMaxLength; ++i) {
        auto gameObject = gameObjectsArray->operator[](i);
        gameObject->Destroy();
        std::cout << "Destroyed GameObject with tag " << tag << ": " << gameObject->GetName()->ToString() << "\n";
    }
}

// Helper function to create an instance of a class
template<typename T>
T* CreateInstance(const std::string& className) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return nullptr;
    }

    auto instance = reinterpret_cast<T*>(Unity::Object::New(il2cppClass));
    if (!instance) {
        std::cerr << "Failed to create instance of class " << className << ".\n";
        return nullptr;
    }

    std::cout << "Successfully created instance of class " << className << " at address 0x" << std::hex << reinterpret_cast<uintptr_t>(instance) << std::dec << "\n";
    return instance;
}

// Helper function to count and print all instances of a class
void CountAndPrintInstances(const std::string& className) {
    auto il2cppClass = IL2CPP::Class::Find(className.c_str());
    if (!il2cppClass) {
        std::cerr << "Class " << className << " not found.\n";
        return;
    }

    auto allObjects = Unity::Object::FindObjectsOfType<Unity::CGameObject>(className.c_str());
    if (!allObjects) {
        std::cout << "No instances of class " << className << " found.\n";
        return;
    }

    size_t count = allObjects->m_uMaxLength;
    std::cout << "Number of instances of class " << className << ": " << count << "\n";

    for (size_t i = 0; i < count; ++i) {
        auto gameObject = allObjects->operator[](i);
        std::cout << "Instance " << i + 1 << ": " << gameObject->GetName()->ToString() << "\n";
    }
}
