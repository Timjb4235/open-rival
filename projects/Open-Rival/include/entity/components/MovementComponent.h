#pragma once

#include <memory>
#include <set>
#include <string>

#include "entity/components/EntityComponent.h"
#include "entity/components/MovementListener.h"
#include "game/Pathfinding.h"
#include "utils/CollectionUtils.h"

namespace Rival {

/**
 * Represents a movement between 2 tiles.
 */
struct Movement
{
    MapNode destination = { -1, -1 };

    /**
     * Time spent moving so far, in ms.
     */
    int timeElapsed = 0;

    /**
     * Total time required to complete the movement, in ms.
     */
    int timeRequired = 0;

    void clear();
    bool isValid() const;
    bool isInProgress() const;
    bool isFinished() const;
};

/**
 * Base class for a component that allows entities to move between tiles.
 */
class MovementComponent : public EntityComponent
{
public:
    MovementComponent(const Pathfinding::PassabilityChecker& passabilityChecker,
            Pathfinding::PassabilityUpdater& passabilityUpdater);
    virtual ~MovementComponent() = default;

    // Begin EntityComponent override
    void update() override;
    // End EntityComponent override

    void addListener(std::weak_ptr<MovementListener> listener);
    void removeListener(std::weak_ptr<MovementListener> listener);

    void moveTo(const MapNode& node);

    /**
     * Gets the movement that's currently in progress.
     */
    const Movement& getMovement() const
    {
        return movement;
    }

private:
    MapNode getStartPosForNextMovement() const;
    void updateMovement();
    bool prepareNextMovement();
    void completeMovement();
    void stopMovement();
    bool tryToRepath();

public:
    static const std::string key;

protected:
    const Pathfinding::PassabilityChecker& passabilityChecker;
    Pathfinding::PassabilityUpdater& passabilityUpdater;

    WeakPtrSet<MovementListener> listeners;

    Pathfinding::Route route;

    Movement movement;

    // TMP: This should depend on the unit's speed
    int ticksPerMove = 30;

private:
    static constexpr float horizontalMoveTimeMultiplier = 1.5f;
};

}  // namespace Rival
