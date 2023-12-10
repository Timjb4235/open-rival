#include "entity/components/MovementComponent.h"

#include "entity/Entity.h"
#include "game/World.h"
#include "utils/TimeUtils.h"

namespace Rival {

const std::string MovementComponent::key = "movement";

void Movement::clear()
{
    destination = { -1, -1 };
    timeElapsed = 0;
    timeRequired = 0;
}

bool Movement::isValid() const
{
    return timeRequired > 0;
}

bool Movement::isInProgress() const
{
    return isValid() && !isFinished();
}

bool Movement::isFinished() const
{
    return isValid() && timeElapsed >= timeRequired;
}

MovementComponent::MovementComponent(
        const Pathfinding::PassabilityChecker& passabilityChecker, Pathfinding::PassabilityUpdater& passabilityUpdater)
    : EntityComponent(key)
    , movement({ 0, 0 })
    , passabilityChecker(passabilityChecker)
    , passabilityUpdater(passabilityUpdater)
{
}

void MovementComponent::update()
{
    // Prepare the next movement if we are not currently moving between tiles
    if (!movement.isValid())
    {
        if (!tryStartNextMovement())
        {
            return;
        }
    }

    updateMovement();
    entity->moved = true;
}

void MovementComponent::addListener(std::weak_ptr<MovementListener> listener)
{
    if (listener.expired())
    {
        return;
    }
    listeners.emplace(listener);
}

void MovementComponent::removeListener(std::weak_ptr<MovementListener> listener)
{
    if (listener.expired())
    {
        return;
    }
    listeners.erase(listener);
}

void MovementComponent::prepareForMovement()
{
    World* world = entity->getWorld();
    passabilityUpdater.onUnitPreparingMove(*world, entity->getPos());
}

void MovementComponent::resetPassability()
{
    World* world = entity->getWorld();
    passabilityUpdater.onUnitStopped(*world, entity->getPos());
}

void MovementComponent::moveTo(const MapNode& node, Pathfinding::Hints hints)
{
    ticksSpentWaiting = 0;
    prepareForMovement();

    const MapNode startPos = getStartPosForNextMovement();
    route = Pathfinding::findPath(startPos, node, *entity->getWorld(), passabilityChecker, hints);

    if (route.isEmpty())
    {
        // Failed to plan a route
        resetPassability();
    }
}

MapNode MovementComponent::getStartPosForNextMovement() const
{
    // If we are already moving between tiles, use the tile where we're about to end up
    return movement.isInProgress() ? movement.destination : entity->getPos();
}

void MovementComponent::updateMovement()
{
    movement.timeElapsed += TimeUtils::timeStepMs;

    if (movement.isFinished())
    {
        onReachedNewTile();
    }
}

/**
 * Called before moving to a new tile.
 */
bool MovementComponent::tryStartNextMovement()
{
    // Check if we have a route planned
    if (route.isEmpty())
    {
        return false;
    }

    // Check that the destination is still pathable
    World* world = entity->getWorld();
    const MapNode* nextNode = route.peek();
    if (!passabilityChecker.isNodePathable(*world, *nextNode))
    {
        // Tile is no longer pathable, e.g. a building has been placed or terrain has changed
        stopMovement();
        return false;
    }

    // Check that the destination is traversable
    if (!passabilityChecker.isNodeTraversable(*world, *nextNode))
    {
        if (passabilityChecker.isNodeObstructed(*world, *nextNode))
        {
            // Tile is obstructed, e.g. another unit has stopped there
            if (!tryRepathAroundObstruction(*world))
            {
                // No way around the obstruction - give up
                stopMovement();
                return false;
            }
        }
        else
        {
            // Tile is only temporarily obstructed, e.g. another unit is leaving the tile
            if (!tryRepathAroundTemporaryObstruction(*world))
            {
                return false;
            }
        }

        nextNode = route.peek();
        if (!passabilityChecker.isNodeTraversable(*world, *nextNode))
        {
            // Still can't move yet!
            return false;
        }
    }

    startNextMovement(*world);
    return true;
}

void MovementComponent::startNextMovement(WritablePathfindingMap& map)
{
    ticksSpentWaiting = 0;

    // Configure the new movement
    movement.destination = route.pop();
    movement.timeRequired = ticksPerMove * TimeUtils::timeStepMs;

    // Horizontal movements should take longer because the distance is greater
    const Facing facing = MapUtils::getDir(entity->getPos(), movement.destination);
    if (facing == Facing::East || facing == Facing::West)
    {
        movement.timeRequired = static_cast<int>(movement.timeRequired * horizontalMoveTimeMultiplier);
    }

    // Update tile passability
    const bool doesRouteContinue = !route.isEmpty();
    passabilityUpdater.onUnitLeavingTile(map, entity->getPos());
    passabilityUpdater.onUnitEnteringTile(map, movement.destination, doesRouteContinue);

    // Inform listeners
    for (std::weak_ptr<MovementListener> weakListener : listeners)
    {
        if (std::shared_ptr<MovementListener> listener = weakListener.lock())
        {
            listener->onUnitMoveStart(&movement.destination);
        }
    }
}

bool MovementComponent::tryRepathAroundObstruction(const PathfindingMap& map, Pathfinding::Hints hints)
{
    if (!tryRepath(hints))
    {
        // This should never happen, but just in case.
        // At the very least we would expect a path to the obstruction to be returned.
        return false;
    }

    const MapNode* nextNode = route.peek();
    if (passabilityChecker.isNodeObstructed(map, *nextNode))
    {
        // Alternate route is still obstructed
        return false;
    }

    return true;
}

bool MovementComponent::tryRepathAroundTemporaryObstruction(const PathfindingMap& map)
{
    if (ticksSpentWaiting < maxTicksToWaitForTileToClear)
    {
        // Wait a while to see if the obstruction clears
        ++ticksSpentWaiting;
        return false;
    }

    // Reset the timer so we don't try to repath every tick
    ticksSpentWaiting = 0;

    // Repath, but treat the next node as an obstruction
    const MapNode* nextNode = route.peek();
    Pathfinding::Hints hints;
    hints.nodesToAvoid.insert(*nextNode);

    return tryRepathAroundObstruction(map, hints);
}

void MovementComponent::onReachedNewTile()
{
    // Update tile passability
    World* world = entity->getWorld();
    passabilityUpdater.onUnitLeftTile(*world, entity->getPos());

    // Update entity position
    // TODO: Currently, during movement, entities are considered to occupy their original tile until they have fully
    // moved into the new tile. We may want to move the unit to the new tile once they are halfway through the movement.
    entity->setPos(movement.destination);

    movement.clear();

    if (route.isEmpty())
    {
        // Reached end of route
        stopMovement();
    }
}

void MovementComponent::stopMovement()
{
    route = {};
    movement.clear();
    resetPassability();
    ticksSpentWaiting = 0;

    // Inform listeners
    for (std::weak_ptr<MovementListener> weakListener : listeners)
    {
        if (std::shared_ptr<MovementListener> listener = weakListener.lock())
        {
            listener->onUnitStopped();
        }
    }
}

bool MovementComponent::tryRepath(Pathfinding::Hints hints)
{
    if (route.isEmpty())
    {
        return false;
    }

    const MapNode destination = route.getDestination();
    moveTo(destination, hints);

    return !route.isEmpty();
}

}  // namespace Rival
