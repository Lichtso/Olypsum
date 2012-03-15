//
//  GUISlider.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUITabs.h"

#ifndef GUISlider_h
#define GUISlider_h

class GUISlider : public GUIRect {
    GLuint textureL, textureM, textureR;
    int mouseDragPos;
    bool highlighted;
    void setBarBorderPixel(unsigned char* pixels, unsigned int barLength, unsigned int x, unsigned int y);
    void generateBar(bool filled);
    void setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y);
    void drawBar(GUIClipRect* clipRect, unsigned int barLength, bool filled);
    public:
    float value;
    bool enabled;
    GUIOrientation orientation;
    void (*onChange)(GUISlider*);
    GUISlider();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif
