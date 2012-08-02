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

class GUIColor {
    public:
    unsigned char r, g, b, a;
    GUIColor() : r(0), g(0), b(0), a(255) {};
    GUIColor(unsigned char gray) : r(gray), g(gray), b(gray), a(255) {};
    GUIColor(unsigned char gray, unsigned char aB) : r(gray), g(gray), b(gray), a(aB) {};
    GUIColor(unsigned char rB, unsigned char gB, unsigned char bB) : r(rB), g(gB), b(bB), a(255) {};
    GUIColor(unsigned char rB, unsigned char gB, unsigned char bB, unsigned char aB) : r(rB), g(gB), b(bB), a(aB) {};
    GUIColor& operator=(const GUIColor& B);
    SDL_Color getSDL();
};

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

extern TextFont *mainFont, *titleFont;

#endif
