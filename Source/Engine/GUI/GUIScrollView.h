//
//  GUIScrollView.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUIScrollView_h
#define GUIScrollView_h

#include "GUIView.h"

class GUIScrollView : public GUIFramedView {
    GUIRoundedRect sliderV, sliderH;
    int mouseDragPosX, mouseDragPosY;
    int getBarWidth();
    int getBarHeight();
    int getBarPosX();
    int getBarPosY();
    public:
    bool sliderX, sliderY;
    int scrollPosX, scrollPosY, contentWidth, contentHeight;
    GUIScrollView();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY);
};

#endif
