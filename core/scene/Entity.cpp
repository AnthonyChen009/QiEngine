#include "Scene.hpp"

namespace Qi {

template<typename T, typename... Args>
T& Entity::addComponent(Args&&... args) {
    QI_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
    return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
}

template<typename T>
T& Entity::getComponent() {
    QI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
    return m_scene->m_registry.get<T>(m_handle);
}

template<typename T>
bool Entity::hasComponent() {
    return m_scene->m_registry.all_of<T>(m_handle);
}

template<typename T>
void Entity::removeComponent() {
    QI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
    m_scene->m_registry.remove<T>(m_handle);
}

}
