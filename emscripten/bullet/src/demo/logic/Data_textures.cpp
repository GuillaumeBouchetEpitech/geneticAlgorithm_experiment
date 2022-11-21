
#include "Data.hpp"

#include "framework/graphic/ResourceManager.hpp"

void Data::initialiseTextures()
{

    { // font

        const bool pixelated = true;
        const bool repeat = false;
        ResourceManager::get().createTexture(0, "assets/textures/ascii_font.png", pixelated, repeat);

    } // font
}
