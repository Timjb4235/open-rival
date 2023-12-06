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
        if (!prepareNextMovement())
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

void MovementComponent::moveTo(MapNode node)
{
    const MapNode startPos = getStartPosForNextMovement();
    auto newRoute = Pathfinding::findPath(startPos, node, *entity->getWorld(), passabilityChecker);
    setRoute(newRoute);
}

MapNode MovementComponent::getStartPosForNextMovement() const
{
    // If we are already moving between tiles, use the tile where we're about to end up
    return movement.isInProgress() ? movement.destination : entity->getPos();
}

void MovementComponent::setRoute(Pathfinding::Route newRoute)
{
    route = newRoute;
}

void MovementComponent::updateMovement()
{
    movement.timeElapsed += TimeUtils::timeStepMs;

    if (movement.isFinished())
    {
        completeMovement();
    }
}

/**
 * Called before moving to a new tile.
 */
bool MovementComponent::prepareNextMovement()
{
    // Check if we have a route planned
    if (route.isEmpty())
    {
        return false;
    }

    // Verify that the destination tile is traversable
    World* world = entity->getWorld();
    if (!passabilityChecker.isNodeTraversable(*world, *route.peek()))
    {
        // Destination tile is either temporarily or permanently blocked.
        // TODO: If we know that the tile will become available again (e.g. if a unit is leaving the tile), then wait.
        //   Otherwise, re-plan our route.
        onStop();
        return false;
    }

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
    passabilityUpdater.onUnitLeavingTile(*world, entity->getPos());
    passabilityUpdater.onUnitEnteringTile(*world, movement.destination, doesRouteContinue);

    // Inform listeners
    for (std::weak_ptr<MovementListener> weakListener : listeners)
    {
        if (std::shared_ptr<MovementListener> listener = weakListener.lock())
        {
            listener->onUnitMoveStart(&movement.destination);
        }
    }

    return true;
}

/**
 * Called after moving to a new tile.
 */
void MovementComponent::completeMovement()
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
        onStop();
    }
}

void MovementComponent::onStop()
{
    // Update tile passability
    World* world = entity->getWorld();
    passabilityUpdater.onUnitStopped(*world, entity->getPos());

    // Inform listeners
    for (std::weak_ptr<MovementListener> weakListener : listeners)
    {
        if (std::shared_ptr<MovementListener> listener = weakListener.lock())
        {
            listener->onUnitStopped();
        }
    }
}

}  // namespace Rival
