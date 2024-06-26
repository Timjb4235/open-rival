#include "entity/components/WalkerComponent.h"

#include <iostream>  // TMP

#include "game/MapUtils.h"
#include "game/World.h"

namespace Rival {

WalkerPassability WalkerComponent::walkerPassability = WalkerPassability();

bool WalkerPassability::isNodePathable(const PathfindingMap& map, const MapNode& node) const
{
    TilePassability passability = map.getPassability(node);
    return (passability & unpathableFlags) == TilePassability::Clear;
}

bool WalkerPassability::isNodeObstructed(const PathfindingMap& map, const MapNode& node) const
{
    TilePassability passability = map.getPassability(node);
    return (passability & obstructedFlags) != TilePassability::Clear;
}

bool WalkerPassability::isNodeTraversable(const PathfindingMap& map, const MapNode& node) const
{
    TilePassability passability = map.getPassability(node);
    return (passability & untraversableFlags) == TilePassability::Clear;
}

void WalkerPassability::onUnitPreparingMove(PathfindingMap& map, const MapNode& node)
{
    map.setPassability(node, TilePassability::GroundUnitPendingMove);
}

void WalkerPassability::onUnitLeavingTile(PathfindingMap& map, const MapNode& node)
{
    map.setPassability(node, TilePassability::GroundUnitLeaving);
}

void WalkerPassability::onUnitEnteringTile(PathfindingMap& map, const MapNode& node, bool isPassingThrough)
{
    map.setPassability(node, isPassingThrough ? TilePassability::GroundUnitLeaving : TilePassability::GroundUnit);
}

void WalkerPassability::onUnitLeftTile(PathfindingMap& map, const MapNode& node)
{
    map.setPassability(node, TilePassability::Clear);
}

void WalkerPassability::onUnitStopped(PathfindingMap& map, const MapNode& node)
{
    map.setPassability(node, TilePassability::GroundUnit);
}

WalkerComponent::WalkerComponent()
    : MovementComponent(WalkerComponent::walkerPassability, WalkerComponent::walkerPassability)
{
}

}  // namespace Rival
