//
//  GUIRoundedRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef GUIRoundedRect_h
#define GUIRoundedRect_h

#include "GUIRect.h"

enum GUICorner {
    TopLeft = 1,
    TopRight = 2,
    BottomLeft = 4,
    BottomRight = 8
};

enum GUIDecorationType {
    Monochrome = 0,
    BrushedSteel = 1,
    Stipple = 2
};

class GUIDrawableRect {
    public:
    int width, height;
    void drawOnScreen(const btVector3& parentTransform, int posX, int posY, GUIClipRect &parentClipRect);
};

class GUIRoundedRect : public GUIDrawableRect {
    float getInnerShadowValue(unsigned char* pixels, unsigned int x, unsigned int y);
    void setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y);
    void setBorderPixelBlended(unsigned char* pixels, unsigned int x, unsigned int y, float alpha);
    void setInnerShadowPixel(unsigned char* pixels, unsigned int x, unsigned int y, float value);
    public:
    GLuint texture;
    bool transposed;
    GUICorner roundedCorners;
    GUIDecorationType decorationType;
    unsigned int cornerRadius;
    float edgeGradientCenter, edgeGradientBorder;
    Color4 topColor, bottomColor, borderColor;
    GUIRoundedRect();
    ~GUIRoundedRect();
    void updateContent();
    void drawOnScreen(const btVector3& parentTransform, int posX, int posY, GUIClipRect &parentClipRect);
};

#endif
