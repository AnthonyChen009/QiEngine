#include "Scene.hpp"
#include "Components.hpp"
#include "renderer/Renderer2D.hpp"
#include "os/Memory.hpp"
#include "core/Assert.hpp"
#include <algorithm>
#include <stack>

namespace Qi {

struct TransformStackEntry {
    int32_t nodeIndex;
    glm::vec2 parentPos;
    float parentRot;
};

Scene::Scene() {
    Node* root = QiNew<Node>("root");

    root->m_index = 0;
    root->m_parentIndex = -1;
    root->m_entity = m_registry.create();
    root->m_scene = this;
    root->m_registry = &m_registry;

    m_nodes.push_back(root);
}

Scene::~Scene() {
    for (int32_t i = (int32_t)m_nodes.size() - 1; i >= 0; --i) {
        Node* node = m_nodes[i];
        if (!node)
            continue;

        if (node->m_entity != entt::null && m_registry.valid(node->m_entity))
            m_registry.destroy(node->m_entity);

        QiDelete(node);
        m_nodes[i] = nullptr;
    }

    m_nodes.clear();
}

void Scene::addNode(Node* node, int32_t parentIndex) {
    QI_CORE_ASSERT(node, "Node is null!");
    QI_CORE_ASSERT(parentIndex >= 0 && parentIndex < (int32_t)m_nodes.size(), "Invalid parent index!");
    QI_CORE_ASSERT(m_nodes[parentIndex], "Parent node is null!");

    node->m_index = (int32_t)m_nodes.size();
    node->m_parentIndex = parentIndex;
    node->m_entity = m_registry.create();
    node->m_scene = this;
    node->m_registry = &m_registry;

    m_nodes.push_back(node);
    m_nodes[parentIndex]->m_childIndices.push_back(node->m_index);

    node->onReady();
}

void Scene::queueDestroyNode(int32_t index) {
    if (index <= 0 || index >= (int32_t)m_nodes.size())
        return;

    if (!m_nodes[index])
        return;

    m_destroyQueue.push_back(index);
}

void Scene::destroyNode(int32_t index) {
    if (index <= 0 || index >= (int32_t)m_nodes.size())
        return;

    Node* node = m_nodes[index];
    if (!node)
        return;

    if (node->m_parentIndex != -1) {
        Node* parent = m_nodes[node->m_parentIndex];

        if (parent) {
            auto& siblings = parent->m_childIndices;
            siblings.erase(
                std::remove(siblings.begin(), siblings.end(), index),
                siblings.end()
            );
        }
    }

    std::stack<int32_t> toVisit;
    std::stack<int32_t> toDelete;

    toVisit.push(index);

    while (!toVisit.empty()) {
        int32_t current = toVisit.top();
        toVisit.pop();

        if (current < 0 || current >= (int32_t)m_nodes.size())
            continue;

        Node* currentNode = m_nodes[current];
        if (!currentNode)
            continue;

        toDelete.push(current);

        for (int32_t childIndex : currentNode->m_childIndices)
            toVisit.push(childIndex);
    }

    while (!toDelete.empty()) {
        int32_t current = toDelete.top();
        toDelete.pop();

        Node* currentNode = m_nodes[current];
        if (!currentNode)
            continue;

        if (currentNode->m_entity != entt::null && m_registry.valid(currentNode->m_entity))
            m_registry.destroy(currentNode->m_entity);

        m_nodes[current] = nullptr;
        QiDelete(currentNode);
    }
}

void Scene::updateWorldTransforms(int32_t rootIndex) {
    std::vector<TransformStackEntry> stack;
    stack.push_back({rootIndex, {0.0f, 0.0f}, 0.0f});
    while (!stack.empty()) {
        TransformStackEntry entry = stack.back();
        stack.pop_back();

        Node* node = m_nodes[entry.nodeIndex];
        if (!node)
            continue;

        glm::vec2 worldPos = entry.parentPos;
        float worldRot = entry.parentRot;

        if (node->hasComponent<TransformComponent>()) {
            auto& transform = node->getComponent<TransformComponent>();

            transform.worldPosition =
                entry.parentPos + transform.position;

            transform.worldRotation =
                entry.parentRot + transform.rotation;

            worldPos = transform.worldPosition;
            worldRot = transform.worldRotation;
        }

        for (int32_t childIndex : node->m_childIndices) {
            stack.push_back({
                childIndex,
                worldPos,
                worldRot
            });
        }
    }
}

void Scene::processDestroyQueue() {
    for (int32_t index : m_destroyQueue)
        destroyNode(index);

    m_destroyQueue.clear();
}

void Scene::onUpdate(Timestep ts) {

}

void Scene::onTick(Timestep ts) {
    onUpdate(ts);

    for (auto* node : m_nodes) {
        if (node)
            node->onUpdate(ts);
    }

    processDestroyQueue();

    // movement system
    auto moveView = m_registry.view<TransformComponent, RigidbodyComponent>();
    for (auto entity : moveView) {
        TransformComponent& transform = moveView.get<TransformComponent>(entity);
        RigidbodyComponent& rb = moveView.get<RigidbodyComponent>(entity);
        transform.position += rb.velocity * (float)ts;
    }
    //update child nodes
    updateWorldTransforms(0);
    // render system
    auto view = m_registry.view<TransformComponent, SpriteComponent>();
    for (auto entity : view) {
        TransformComponent& transform = view.get<TransformComponent>(entity);
        SpriteComponent& sprite = view.get<SpriteComponent>(entity);
        if (sprite.texture)
            Renderer2D::drawTexturedQuad(transform.worldPosition, transform.size, transform.worldRotation, sprite.color, sprite.texture);
        else
            Renderer2D::drawQuad(transform.worldPosition, transform.size, transform.worldRotation, sprite.color);
    }
}

void Scene::onEvent(Event& e) {
    for (Node* node : m_nodes) {
        if (node)
            node->onEvent(e);
    }

    processDestroyQueue();
}

}
