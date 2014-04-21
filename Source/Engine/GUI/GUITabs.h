//
//  GUITabs.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef GUITabs_h
#define GUITabs_h

#include "GUICheckBox.h"

class GUITabs : public GUIView {
    public:
    GUISizeAlignment sizeAlignment;
    GUIOrientation orientation;
    int selected;
    bool deactivatable;
    std::function<void(GUITabs*)> onChange;
    GUITabs();
    bool addChild(GUIRect* child);
    void updateContent();
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif
