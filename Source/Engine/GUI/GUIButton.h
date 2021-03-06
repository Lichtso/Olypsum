//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef GUIButton_h
#define GUIButton_h

#include "GUILabel.h"

class GUIButton : public GUIFramedView {
    public:
    GUISizeAlignment sizeAlignment;
    int paddingX, paddingY;
    bool enabled;
    enum State {
        Released = 0,
        Highlighted = 1,
        Pressed = 2
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
    void draw(const btVector3& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif