//
//  GUIRoundedRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "LightVolume.h"
#include "GUIClipRect.h"

#ifndef GUIRoundedRect_h
#define GUIRoundedRect_h

enum GUICorners {
    GUITopLeftCorner = 1,
    GUITopRightCorner = 2,
    GUIBottomLeftCorner = 4,
    GUIBottomRightCorner = 8
};

class GUIDrawableRect {
    public:
    int width, height;
    void drawOnScreen(btVector3 parentTransform, int posX, int posY, GUIClipRect &parentClipRect);
};

class GUIRoundedRect : public GUIDrawableRect {
    float getInnerShadowValue(unsigned char* pixels, unsigned int x, unsigned int y);
    void setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y);
    void setBorderPixelBlended(unsigned char* pixels, unsigned int x, unsigned int y, float alpha);
    void setInnerShadowPixel(unsigned char* pixels, unsigned int x, unsigned int y, float value);
    public:
    GLuint texture;
    bool transposed;
    GUICorners roundedCorners;
    Color4 topColor, bottomColor, borderColor;
    int innerShadow;
    unsigned int cornerRadius;
    GUIRoundedRect();
    ~GUIRoundedRect();
    void drawInTexture();
    void drawOnScreen(btVector3 parentTransform, int posX, int posY, GUIClipRect &parentClipRect);
};

#endif
