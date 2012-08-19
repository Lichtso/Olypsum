//
//  TextFont.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"
#import <SDL/SDL.h>
#import <SDL_ttf/SDL_ttf.h>

#ifndef TextFont_h
#define TextFont_h

class TextFont {
    public:
    TTF_Font* ttf;
    unsigned int size;
    TextFont();
    ~TextFont();
    bool loadTTF(const char* fileName);
    GLuint renderStringToTexture(const char* str, GUIColor color, bool antialiasing, int& width, int& height);
    void renderStringToSurface(const char* str, SDL_Surface* surface, int x, int y, GUIColor color, bool antialiasing);
    void renderStringToScreen(const char* str, Vector3 pos, float scale, GUIColor color, bool antialiasing);
};

extern TextFont* mainFont;

#endif
