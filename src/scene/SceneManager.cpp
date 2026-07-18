#include "SceneManager.hpp"
#include <algorithm>
#include "core/Assert.hpp"
namespace Qi {

SceneManager::SceneManager(uint32_t width, uint32_t height) : m_initalWidth(width), m_initialHeight(height){

}

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    m_currentScene.reset();

    m_currentScene = std::move(scene);

    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onReady();
    m_currentScene->initialSize(m_initalWidth, m_initialHeight);
}

void SceneManager::onUpdate(Qi::Timestep ts) {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onTick(ts);
}

void SceneManager::onPhysicsUpdate(Qi::Timestep ts) {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onTick(ts);
}

Scene& SceneManager::getCurrentScene() {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    return *m_currentScene;
}

void SceneManager::onEvent(Qi::Event& event) {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onEvent(event);
}

}
