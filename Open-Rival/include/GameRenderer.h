#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include "BuildingRenderer.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "FramebufferRenderer.h"
#include "MapBorderRenderer.h"
#include "Rect.h"
#include "Resources.h"
#include "Scenario.h"
#include "TileRenderer.h"
#include "UnitRenderer.h"
#include "Window.h"

namespace Rival {

    class GameRenderer {
    public:
        GameRenderer(
                const Window& window,
                const Scenario& scenario,
                const Camera& camera,
                const Rect& viewport,
                const Resources& res);

        void render();

    private:
        // Framebuffer size, in pixels.
        // This is our canvas; we can never render more pixels than this.
        static const int framebufferWidth;
        static const int framebufferHeight;

        const Window& window;
        const Scenario& scenario;
        const Rect& viewport;
        const Camera& camera;
        const Resources& res;

        /**
         * Framebuffer to which the visible region of the game is rendered at
         * a pixel-perfect scale.
         *
         * This ensures that there are no seams between tiles, and we can
         * perform any scaling when we render the framebuffer to the screen.
         */
        Framebuffer gameFbo;

        // Renderers
        BuildingRenderer buildingRenderer;
        FramebufferRenderer gameFboRenderer;
        TileRenderer tileRenderer;
        MapBorderRenderer mapBorderRenderer;
        UnitRenderer unitRenderer;

        void renderGame(int viewportWidth, int viewportHeight);
        void renderFramebuffer(int srcWidth, int srcHeight);
    };

}  // namespace Rival

#endif GAME_RENDERER_H