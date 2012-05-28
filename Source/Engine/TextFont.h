//
//  TextFont.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ParticleSystem.h"
#import <SDL/SDL.h>
#import <SDL_ttf/SDL_ttf.h>

#ifndef TextFont_h
#define TextFont_h

class TextFont {
    public:
    TTF_Font* ttf;
    unsigned int size;
    SDL_Color color, backgroundColor;
    TextFont();
    ~TextFont();
    bool loadTTF(const char* fileName);
    GLuint renderStringToTexture(const char* str, SDL_Color colorB, bool antialiasing, int& width, int& height);
    void renderStringToSurface(const char* str, SDL_Surface* surface, int x, int y, SDL_Color colorB, bool antialiasing);
    void renderStringToScreen(const char* str, Vector3 pos, float scale, SDL_Color color, bool antialiasing);
};

extern TextFont *mainFont, *titleFont;

#endif
