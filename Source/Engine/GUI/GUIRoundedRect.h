//
//  GUIRoundedRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIRect.h"

#ifndef GUIRoundedRect_h
#define GUIRoundedRect_h

enum GUICorners {
    GUITopLeftCorner = 1,
    GUITopRightCorner = 2,
    GUIBottomLeftCorner = 4,
    GUIBottomRightCorner = 8
};

class GUIRoundedRect {
    unsigned char* pixels;
    float getInnerShadowValue(unsigned int x, unsigned int y);
    void setBorderPixel(unsigned int x, unsigned int y);
    void setInnerShadowPixel(unsigned int x, unsigned int y, float value);
    public:
    GLuint* texture;
    GUICorners roundedCorners;
    unsigned int cornerRadius;
    GUIColor topColor, bottomColor, borderColor;
    int width, height, innerShadow;
    GUIRoundedRect();
    void drawInTexture();
    void drawOnScreen(bool transposed, int posX, int posY, GUIClipRect& parentClipRect);
};

#endif
