#include "Timestep.hpp"
#include "events/Event.hpp"
#include "Entity.hpp"

namespace Qi {

class Scene {
public:
    virtual ~Scene() = default;

    virtual void onUpdate(Timestep ts) = 0;
    virtual void onEvent(Event& e) {}

protected:
    entt::registry m_registry;

    Entity createEntity(const std::string& name = "Entity");
    void destroyEntity(Entity entity);

    friend class Entity;
};

}
