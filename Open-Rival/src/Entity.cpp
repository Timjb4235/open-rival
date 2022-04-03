#include "pch.h"
#include "Entity.h"

namespace Rival {

    Entity::Entity(int width, int height)
        : width(width),
          height(height),
          pos({ -1, -1 }),
          scenario(nullptr),
          deleted(false),
          moved(true),
          id(-1) {}

    void Entity::attach(std::unique_ptr<EntityComponent> component) {
        component->onAttach(this);
        components.insert({ component->getKey(), std::move(component) });
    }

    void Entity::onSpawn(Scenario* newScenario, int newId, MapNode newPos) {
        scenario = newScenario;
        id = newId;
        pos = newPos;

        for (auto const& kv : components) {
            const auto& component = kv.second;
            component->onEntitySpawned(scenario);
        }
    }

    void Entity::earlyUpdate() {
        moved = false;
    }

    void Entity::update() {
        updateComponents();
        cleanUpComponents();
    }

    void Entity::updateComponents() const {
        for (auto const& kv : components) {
            const auto& component = kv.second;
            if (!component->isDeleted()) {
                component->update();
            }
        }
    }

    void Entity::cleanUpComponents() {
        for (auto it = components.cbegin(); it != components.cend();) {
            const auto& component = it->second;
            if (component->isDeleted()) {
                it = components.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Entity::setPos(MapNode newPos) {
        pos = newPos;
        moved = true;
    }

}  // namespace Rival
