//
//  TextFont.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "TextFont.h"

TextFont::TextFont() {
    ttf = NULL;
    size = 12;
}

TextFont::~TextFont() {
    if(ttf)
        TTF_CloseFont(ttf);
}

bool TextFont::loadTTF(const char* fileName) {
    if(ttf) return false;
    
    std::string url("Fonts/");
    url += fileName;
    url += ".ttf";
    
    ttf = TTF_OpenFont(url.c_str(), size);
    if(ttf == NULL) {
        log(error_log, std::string("Unable to load font ")+url.c_str()+".\n"+TTF_GetError());
        return false;
    }
    
    return true;
}

GLuint TextFont::renderStringToTexture(const char* str, Color4 color, bool antialiasing, int& width, int& height) {
    SDL_Surface *surfaceB;
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    if(antialiasing) {
        surfaceB = TTF_RenderUTF8_Blended(ttf, str, color.getSDL());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceB->w, surfaceB->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surfaceB->pixels);
    }else{
        SDL_Surface *surfaceA = TTF_RenderUTF8_Solid(ttf, str, {255, 255, 255, 255});
        surfaceB = SDL_CreateRGBSurface(surfaceA->flags, surfaceA->w, surfaceA->h, 32, 255, 255 << 8, 255 << 16, 255 << 24);
        unsigned char *pixelsA = (unsigned char*)surfaceA->pixels,
        *pixelsB = (unsigned char*)surfaceB->pixels;
        SDL_Color charColor = color.getSDL();
        unsigned int index = 0;
        for(unsigned int y = 0; y < surfaceA->h; y ++)
            for(unsigned int x = 0; x < surfaceA->w; x ++) {
                if(pixelsA[y*surfaceA->pitch+x] == 0) continue;
                index = y*surfaceB->pitch+x*4;
                pixelsB[index  ] = charColor.r;
                pixelsB[index+1] = charColor.g;
                pixelsB[index+2] = charColor.b;
                pixelsB[index+3] = charColor.unused;
            }
        SDL_FreeSurface(surfaceA);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceB->w, surfaceB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceB->pixels);
    }
    
    width = surfaceB->w;
    height = surfaceB->h;
    SDL_FreeSurface(surfaceB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    return texture;
}

void TextFont::renderStringToSurface(const char* str, SDL_Surface* surfaceB, int xPosB, int yPosB, Color4 color, bool antialiasing) {
    SDL_Surface *surfaceA;
    
    if(antialiasing) {
        surfaceA = TTF_RenderUTF8_Blended(ttf, str, color.getSDL());
    }else{
        surfaceA = TTF_RenderUTF8_Solid(ttf, str, {255, 255, 255, 255});
    }
    
    unsigned char *pixelsA = (unsigned char*)surfaceA->pixels,
                  *pixelsB = (unsigned char*)surfaceB->pixels;
    unsigned int indexA, indexB, width = surfaceA->w, height = surfaceA->h,
                 xPosA = 0, yPosA = 0;
    
    if(xPosB < 0) {
        xPosA = -xPosB;
        xPosB = 0;
        width -= xPosA;
    }
    if(width > surfaceB->w-xPosB)
        width = surfaceB->w-xPosB;
    if(yPosB < 0) {
        yPosA = -yPosB;
        yPosB = 0;
        height -= yPosA;
    }
    if(height > surfaceB->h-yPosB)
        height = surfaceB->h-yPosB;
    
    if(antialiasing) {
        float alpha, iAlpha;
        for(unsigned int y = 0; y < height; y ++)
            for(unsigned int x = 0; x < width; x ++) {
                indexA = (y+yPosA)*surfaceA->pitch+(x+xPosA)*4;
                indexB = (y+yPosB)*surfaceB->pitch+(x+xPosB)*4;
                alpha = pixelsA[indexA+3]/255.0;
                iAlpha = 1.0-alpha;
                pixelsB[indexB  ] = pixelsB[indexB  ]*iAlpha+pixelsA[indexA  ]*alpha;
                pixelsB[indexB+1] = pixelsB[indexB+1]*iAlpha+pixelsA[indexA+1]*alpha;
                pixelsB[indexB+2] = pixelsB[indexB+2]*iAlpha+pixelsA[indexA+2]*alpha;
                pixelsB[indexB+3] = max(pixelsB[indexB+3], pixelsA[indexA+3]);
            }
    }else{
        SDL_Color charColor = color.getSDL();
        for(unsigned int y = 0; y < height; y ++)
            for(unsigned int x = 0; x < width; x ++) {
                if(pixelsA[(y+yPosA)*surfaceA->pitch+(x+xPosA)] == 0) continue;
                indexB = (y+yPosB)*surfaceB->pitch+(x+xPosB)*4;
                pixelsB[indexB  ] = charColor.b;
                pixelsB[indexB+1] = charColor.g;
                pixelsB[indexB+2] = charColor.r;
                pixelsB[indexB+3] = charColor.unused;
            }
    }
    
    SDL_FreeSurface(surfaceA);
}

void TextFont::renderStringToScreen(const char* str, btVector3 pos, float scale, Color4 color, bool antialiasing) {
    int width, height;
    GLuint texture = renderStringToTexture(str, color, antialiasing, width, height);
    
    scale /= size;
    btVector3 rect = btVector3(width*scale, height*scale, 0.0);
    float vertices[] = {
        rect.x(), -rect.y(),
        1.0, 1.0,
        rect.x(), rect.y(),
        1.0, 0.0,
        -rect.x(), rect.y(),
        0.0, 0.0,
        -rect.x(), -rect.y(),
        0.0, 1.0
    };
    
    modelMat.setIdentity();
    modelMat.setBasis(currentCam->camMat.getBasis());
    modelMat.setOrigin(pos);
    
    shaderPrograms[spriteSP]->use();
    shaderPrograms[spriteSP]->setAttribute(POSITION_ATTRIBUTE, 2, 4*sizeof(float), vertices);
    shaderPrograms[spriteSP]->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDeleteTextures(1, &texture);
}

TextFont* mainFont;