#include "SceneManager.hpp"
#include <algorithm>
#include "core/Assert.hpp"
namespace Qi {

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    m_currentScene.reset();

    m_currentScene = std::move(scene);

    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onReady();

}

void SceneManager::onUpdate(Qi::Timestep ts) {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onTick(ts);

}

Scene& SceneManager::getCurrentScene() {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    return *m_currentScene;
}

void SceneManager::onEvent(Qi::Event& e) {
    QI_CORE_ASSERT(m_currentScene, "No active scene!");
    m_currentScene->onEvent(e);

}

}
