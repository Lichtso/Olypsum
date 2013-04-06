//
//  TextFont.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <SDL_ttf/SDL_ttf.h>
#include "CamObject.h"

#ifndef TextFont_h
#define TextFont_h

//! Used to render text and strings
class TextFont {
    public:
    TTF_Font* ttf; //!< The TTF_Font
    unsigned int size; //!< The font size in pixels
    TextFont();
    ~TextFont();
    //! Loads a TTF font from a .ttf file
    bool loadTTF(const char* fileName);
    /*! Renders a string into a OpenGL texture
     @param str The string to be rendered
     @param color The color of the string used for filling
     @param antialiasing Enable edge smoothing
     @param width A reference which will be used to store the width of the texture
     @param height A reference which will be used to store the height of the texture
     @return The OpenGL identifier of the generated texture
     */
    GLuint renderStringToTexture(const char* str, Color4 color, bool antialiasing, int& width, int& height);
    /*! Renders a string into a SDL_Surface
     @param str The string to be rendered
     @param surface The SDL_Surface to be used to store the result
     @param x The x coord of the string (automatically clamps to edge)
     @param y The y coord of the string (automatically clamps to edge)
     @param color The color of the string used for filling
     @param antialiasing Enable edge smoothing
     */
    void renderStringToSurface(const char* str, SDL_Surface* surface, int x, int y, Color4 color, bool antialiasing);
    /*! Renders a string on the screen
     @param str The string to be rendered
     @param pos The position of the string in the world
     @param scale The size of the string in world units
     @param color The color of the string used for filling
     @param antialiasing Enable edge smoothing
     */
    void renderStringToScreen(const char* str, btVector3 pos, float scale, Color4 color, bool antialiasing);
};

extern TextFont *mainFont, *italicFont;

#endif
