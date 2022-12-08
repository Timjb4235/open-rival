#pragma once

#include <memory>

#include "Entity.h"
#include "MapUtils.h"

namespace Rival {

/**
 * Data relating to the current selection and actions of the local player.
 */
struct PlayerContext
{
    MapNode tileUnderMouse { -1, -1 };
    std::weak_ptr<Entity> weakEntityUnderMouse;
    std::weak_ptr<Entity> weakSelectedEntity;
};

}  // namespace Rival