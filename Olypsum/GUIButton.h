//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUILabel.h"

#ifndef GUIButton_h
#define GUIButton_h

#define GUITopLeftCorner 1
#define GUITopRightCorner 2
#define GUIBottomLeftCorner 4
#define GUIBottomRightCorner 8

#define GUIButtonStateDisabled 0
#define GUIButtonStateNormal 1
#define GUIButtonStateHighlighted 2
#define GUIButtonStatePressed 3

class GUIButton : public GUIView {
    GLuint texture;
    void setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y);
    void setInnerShadowRect(unsigned char* pixels, unsigned int minX, unsigned int maxX, unsigned int minY, unsigned int maxY);
    public:
    bool autoSize;
    int paddingX, paddingY;
    char roundedCorners, state;
    void (*clicked)();
    GUIButton();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif