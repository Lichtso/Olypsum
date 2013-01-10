//
//  GUICheckBox.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIButton.h"

#ifndef GUICheckBox_h
#define GUICheckBox_h

class GUICheckBox : public GUIButton {
    public:
    std::function<void(GUICheckBox*)> onClick;
    GUICheckBox();
    void addChild(GUIRect* child);
    void updateContent();
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif
