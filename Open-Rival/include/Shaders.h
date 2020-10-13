#ifndef SHADERS_H
#define SHADERS_H

#include <gl/glew.h>

namespace Rival {

    ///////////////////////////////////////////////////////////////////////////
    // GameWorldShader:
    // Renders a texture using a view-projection matrix and an accompanying
    // palette texture for colour lookups.
    ///////////////////////////////////////////////////////////////////////////

    class GameWorldShader {
    public:
        GLuint programId;

        // Vertex shader uniform locations
        GLint viewProjMatrixUniformLocation;

        // Vertex shader attribute indices
        GLint vertexAttribIndex = 0;
        GLint texCoordAttribIndex = 1;

        // Vertex shader attribute locations
        GLint vertexAttribLocation;
        GLint texCoordAttribLocation;

        // Fragment shader uniform locations
        GLint texUnitUniformLocation;
        GLint paletteTexUnitUniformLocation;

        static void init();

        bool isValid() const;
    };

    extern GameWorldShader gameWorldShader;

    ///////////////////////////////////////////////////////////////////////////
    // ScreenShader:
    // Just renders a texture without applying any transformations.
    ///////////////////////////////////////////////////////////////////////////

    class ScreenShader {
    public:
        GLuint programId;

        // Vertex shader attribute indices
        GLint vertexAttribIndex = 0;
        GLint texCoordAttribIndex = 1;

        // Vertex shader attribute locations
        GLint vertexAttribLocation;
        GLint texCoordAttribLocation;

        // Fragment shader uniform locations
        GLint texUnitUniformLocation;

        static void init();

        bool isValid() const;
    };

    extern ScreenShader screenShader;

    ///////////////////////////////////////////////////////////////////////////
    // MenuShader:
    // Renders a texture in normalized device coordinates using an accompanying
    // palette texture for colour lookups.
    ///////////////////////////////////////////////////////////////////////////

    class MenuShader {
    public:
        GLuint programId;

        // Vertex shader attribute indices
        GLint vertexAttribIndex = 0;
        GLint texCoordAttribIndex = 1;

        // Vertex shader attribute locations
        GLint vertexAttribLocation;
        GLint texCoordAttribLocation;

        // Fragment shader uniform locations
        GLint texUnitUniformLocation;
        GLint paletteTexUnitUniformLocation;

        static void init();

        bool isValid() const;
    };

    extern MenuShader menuShader;

    ///////////////////////////////////////////////////////////////////////////
    // Generic methods
    ///////////////////////////////////////////////////////////////////////////

    void initialiseShaders();

    GLuint createShader(const char* vertShader, const char* fragShader);

}  // namespace Rival

#endif  // SHADERS_H
