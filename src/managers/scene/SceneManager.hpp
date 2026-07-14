#pragma once
#include "scene/Scene.hpp"
namespace Qi {

class SceneManager {

public:
    void setScene(std::unique_ptr<Scene> scene);

	void onUpdate(Qi::Timestep ts);
	void onEvent(Qi::Event& event);
	Scene& getCurrentScene();

private:
    float m_timeStep = 0;
    std::unique_ptr<Scene> m_currentScene;
};

}
