#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <gl/glew.h>
#include <string>
#include <vector>

#include "Rect.h"
#include "RenderUtils.h"
#include "TextureAtlas.h"

namespace Rival { namespace GameInterface {

    /**
     * Height of the in-game UI, in menu co-ordinates.
     */
    static const int uiHeight = 144;

    /**
     * Y-position of the in-game UI, in menu co-ordinates.
     */
    static const int uiPosY = RenderUtils::menuHeight - uiHeight;

    // UI element positions, in menu co-ordinates
    extern const Rect minimapLeftBorder;
    extern const Rect minimapTopBorder;
    extern const Rect minimapBottomBorder;
    extern const Rect mainPanel;
    extern const Rect inventoryOverlay;
    extern const Rect statsPanel;

    ///////////////////////////////////////////////////////////////////////////
    // UiImage class
    ///////////////////////////////////////////////////////////////////////////

    class UiImage {
    public:
        const Rect pos;

        UiImage(Rect pos, const TextureAtlas& texAtlas, const std::string imageKey);

        void addToBuffers(std::vector<GLfloat>& positions, std::vector<GLfloat>& texCoords) const;

    private:
        const TextureAtlas& texAtlas;
        const std::string imageKey;
    };

}}  // namespace Rival::GameInterface

#endif  // GAME_INTERFACE_H
