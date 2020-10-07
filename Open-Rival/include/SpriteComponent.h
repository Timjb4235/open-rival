#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <map>

#include "EntityComponent.h"
#include "SpriteRenderable.h"
#include "Unit.h"

namespace Rival {

    /**
     * Component that links an Entity to a Renderable.
     *
     * This ensures that all graphical resources are released automatically
     * when an Entity is destroyed.
     */
    class SpriteComponent : public EntityComponent {

    public:
        static const std::string key;

        int txIndex;

        SpriteComponent(const Spritesheet& spritesheet);

        const SpriteRenderable& getRenderable() const;

        int getTxIndex() const;

    private:
        const SpriteRenderable renderable;
    };

}  // namespace Rival

#endif  // SPRITE_COMPONENT_H