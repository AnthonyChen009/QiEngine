#pragma once
#include "scene/Scene.hpp"
namespace Qi {

class SceneManager {

public:
    SceneManager(uint32_t width, uint32_t height);
    void setScene(std::unique_ptr<Scene> scene);
    void clear();
	void onUpdate(Qi::Timestep ts);
	void onEvent(Qi::Event& event);
	void onPhysicsUpdate(Qi::Timestep ts);
	Scene& getCurrentScene();

private:
    float m_timeStep = 0;
    std::unique_ptr<Scene> m_currentScene;
    uint32_t m_initalWidth;
    uint32_t m_initialHeight;
};

}
