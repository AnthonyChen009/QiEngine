#pragma once

#include <vector>
#include <cstdint>

#include <entt/entt.hpp>
#include "glm/glm.hpp"
#include "Node.hpp"

namespace Qi {

class Scene {

public:
    Scene();
    virtual ~Scene();

    void addNode(Node* node, int32_t parentIndex = 0);

    void queueDestroyNode(int32_t index);
    void destroyNode(int32_t index);
    void processDestroyQueue();

    virtual void onReady() {}
    virtual void onUpdate(Timestep ts);
    void onTick(Timestep ts);
    virtual void onEvent(Event& event);

    Node* getNode(int32_t index) { return m_nodes[index]; }
    const std::vector<Node*>& getNodes() const { return m_nodes; }
    entt::registry& getRegistry() { return m_registry; }
private:
    void updateWorldTransforms2D(int32_t rootIndex);
    void updateWorldTransforms3D(int32_t rootIndex);

private:
    std::vector<Node*> m_nodes;
    std::vector<int32_t> m_destroyQueue;

    entt::registry m_registry;

    friend class Node;
};

}
