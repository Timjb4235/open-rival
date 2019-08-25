#ifndef SPRITE_H
#define SPRITE_H

#include <gl/glew.h>
#include <iostream>
#include <vector>

#include "Texture.h"

namespace Rival {

    /**
     * Class that defines how a texture should be divided into smaller images.
     */
    class Sprite {

    public:

        const Texture& texture;

        Sprite(const Texture&, const int width, const int height);

        Sprite(const Sprite& other) = delete;
        Sprite(Sprite&& other) = delete;
        Sprite& operator=(const Sprite& other) = delete;
        Sprite& operator=(Sprite&& other) = delete;

        const std::vector<GLfloat> getTexCoords(const int index) const;

    private:

        // Width of one image within the texture
        const int width;

        // Height of one image within the texture
        const int height;

        // Number of columns in the texture
        const int xSize;

        // Number of rows in the texture
        const int ySize;

    };

}

#endif // SPRITE_H
