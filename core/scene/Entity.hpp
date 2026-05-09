#pragma once
#include <entt/entt.hpp>

namespace Qi {

class Scene;

class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene)
        : m_handle(handle), m_scene(scene) {}

    template<typename T, typename... Args>
    T& addComponent(Args&&... args);

    template<typename T>
    T& getComponent();

    template<typename T>
    bool hasComponent();

    template<typename T>
    void removeComponent();

    bool isValid() const { return m_handle != entt::null; }
    operator bool() const { return isValid(); }

private:
    entt::entity m_handle = entt::null;
    Scene* m_scene = nullptr;

    friend class Scene;
};

}
