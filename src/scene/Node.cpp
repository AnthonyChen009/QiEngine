#include "Node.hpp"
#include "Scene.hpp"
#include "core/Assert.hpp"

namespace Qi {

Node::Node(const std::string& name) : m_name(name) {
    m_name = m_nameRegistry.makeUniqueName(name);
}

void Node::destroy() {
    if (m_scene && m_index != -1)
        m_scene->destroyNode(m_index);
}

void Node::addChild(Node* node) {
    QI_CORE_ASSERT(m_scene, "Node has no scene!");
    m_scene->addNode(node, m_index);
}

}
