// Node.hpp
#pragma once
#include <string>
#include <vector>
#include <entt/entt.hpp>
#include "core/Timestep.hpp"

namespace Qi {

class Scene;
class Event;
class Timestep;

class Node {
public:
    Node(const std::string& name = "Node") : m_name(name) {

    }

    virtual ~Node() = default;

    virtual void onReady() {}
    virtual void onUpdate(Timestep ts) {}
    virtual void onEvent(Event& event) {}

    void destroy();
    void addChild(Node* node);

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        return m_registry->emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T>
    T& getComponent() {
        return m_registry->get<T>(m_entity);
    }

    template<typename T>
    const T& getComponent() const {
        return m_registry->get<T>(m_entity);
    }

    template<typename T>
    bool hasComponent() const {
        return m_registry->all_of<T>(m_entity);
    }

    bool isInsideTree() const {
        return m_scene != nullptr;
    }

protected:
    std::string m_name;
    int32_t m_index = -1;
    int32_t m_parentIndex = -1;
    std::vector<int32_t> m_childIndices;

    entt::entity m_entity = entt::null;
    entt::registry* m_registry = nullptr;
    Scene* m_scene = nullptr;

    friend class Scene;
};

}
