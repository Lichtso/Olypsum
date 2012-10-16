//
//  GUISlider.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIRoundedRect.h"
#import "GUIRect.h"

#ifndef GUISlider_h
#define GUISlider_h

class GUISlider : public GUIRect {
    GLuint textureL, textureM, textureR;
    int mouseDragPos;
    bool highlighted;
    void generateBar(bool filled);
    void drawBar(GUIClipRect& clipRect, unsigned int barLength, bool filled);
    public:
    float value;
    unsigned int steps;
    bool enabled;
    GUIOrientation orientation;
    std::function<void(GUISlider*)> onChange;
    GUISlider();
    ~GUISlider();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
};

#endif
