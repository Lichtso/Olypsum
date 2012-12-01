//
//  TextFont.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <SDL_ttf/SDL_ttf.h>
#import "Cam.h"

#ifndef TextFont_h
#define TextFont_h

class TextFont {
    public:
    TTF_Font* ttf;
    unsigned int size;
    TextFont();
    ~TextFont();
    bool loadTTF(const char* fileName);
    GLuint renderStringToTexture(const char* str, Color4 color, bool antialiasing, int& width, int& height);
    void renderStringToSurface(const char* str, SDL_Surface* surface, int x, int y, Color4 color, bool antialiasing);
    void renderStringToScreen(const char* str, btVector3 pos, float scale, Color4 color, bool antialiasing);
};

extern TextFont* mainFont;

#endif
