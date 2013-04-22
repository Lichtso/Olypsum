//
//  GUIProgressBar.h
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUIProgressBar_h
#define GUIProgressBar_h

#include "GUITabs.h"

class GUIProgressBar : public GUIRect {
    GUIRoundedRect barL, barR;
    void drawBar(btVector3 transform, GUIClipRect clipRect, GUIRoundedRect& roundedRect);
    public:
    float value;
    GUIOrientation orientation;
    GUIProgressBar();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
};

#endif
