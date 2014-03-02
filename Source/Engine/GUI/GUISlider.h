//
//  GUISlider.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUISlider_h
#define GUISlider_h

#include "GUIProgressBar.h"

class GUISlider : public GUIRect {
    GUIRoundedRect barL, barR, slider;
    int mouseDragPos;
    bool highlighted;
    void drawBar(const btVector3& parentTransform, GUIClipRect clipRect, GUIRoundedRect& roundedRect);
    public:
    float value;
    unsigned int steps;
    bool enabled;
    GUIOrientation orientation;
    std::function<void(GUISlider*, bool)> onChange;
    GUISlider();
    void updateContent();
    void draw(const btVector3& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY);
};

#endif
