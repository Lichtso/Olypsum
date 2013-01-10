//
//  GUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <SDL/SDL.h>
#include "GUIClipRect.h"

#ifndef GUIRect_h
#define GUIRect_h

enum GUIElementType {
    GUIType_Rect = 0,
    GUIType_View = 1,
    GUIType_ScrollView = 2,
    GUIType_ScreenView = 3,
    GUIType_Label = 4,
    GUIType_Button = 5,
    GUIType_CheckBox = 6,
    GUIType_ButtonList = 7,
    GUIType_Tabs = 8,
    GUIType_ProgressBar = 9,
    GUIType_Silder = 10,
    GUIType_TextField = 11
};

enum GUIOrientation {
    GUIOrientation_Left = 1,
    GUIOrientation_Right = 2,
    GUIOrientation_Top = 4,
    GUIOrientation_Bottom = 8,
    GUIOrientation_Vertical = 3,
    GUIOrientation_Horizontal = 12
};

class GUIRect {
    public:
    GUIElementType type;
    GUIRect* parent;
    bool visible;
    int posX, posY, width, height;
    GUIRect();
    virtual ~GUIRect();
    GUIRect* getRootParent();
    bool isFirstResponder();
    virtual void setFirstResponderStatus();
    virtual void removeFirstResponderStatus();
    virtual bool getLimSize(GUIClipRect& clipRect, GUIClipRect& parentClipRect);
    virtual void updateContent();
    virtual void draw(btVector3 transform, GUIClipRect& parentClipRect);
    virtual bool handleMouseDown(int mouseX, int mouseY);
    virtual bool handleMouseUp(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY);
    virtual bool handleMouseWheel(int mouseX, int mouseY, float delta);
    virtual bool handleKeyDown(SDL_keysym* key);
    virtual bool handleKeyUp(SDL_keysym* key);
};

#endif