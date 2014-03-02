//
//  TextFont.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef TextFont_h
#define TextFont_h

#include "ObjectManager.h"

//! Used to render text and strings
class TextFont : public FileResource {
    TTF_Font* ttf; //!< The TTF_Font
    public:
    unsigned int size; //!< The font size in pixels
    TextFont();
    ~TextFont();
    FileResourcePtr<FileResource> load(FilePackage* filePackage, const std::string& name);
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
    void renderStringToScreen(const char* str, const btVector3& pos, float scale, Color4 color, bool antialiasing);
    /*! Calculates the width and height of a text
     @param text The text used to calculate the size
     @param width The result width is stored here
     @param height The result height is stored here
     */
    void calculateTextSize(const char* text, int& width, int& height);
    //! Returns the normal font
    static FileResourcePtr<TextFont> normalFont();
    //! Returns the italic font
    static FileResourcePtr<TextFont> italicFont();
};

#endif
