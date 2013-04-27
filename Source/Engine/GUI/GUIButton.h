//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUIButton_h
#define GUIButton_h

#include "GUILabel.h"

class GUIButton : public GUIFramedView {
    public:
    GUISizeAlignment sizeAlignment;
    unsigned int paddingX, paddingY;
    enum State {
        Disabled = 0,
        Enabled = 1,
        Highlighted = 2,
        Pressed = 3
    } state;
    enum Type {
        Normal = 0,
        Delete = 1,
        Add = 2,
        Edit = 3,
        Lockable = 4
    } type;
    std::function<void(GUIButton*)> onClick;
    GUIButton();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif