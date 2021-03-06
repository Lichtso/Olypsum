//
//  GUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef GUIRect_h
#define GUIRect_h

#include "GUIClipRect.h"

enum GUIOrientation {
    Left = 1,
    Right = 2,
    Bottom = 4,
    Top = 8,
    Vertical = 3,
    Horizontal = 12
};

enum GUISizeAlignment {
    None = 0,
    Width = 1,
    Height = 2,
    All = 3
};

class GUIView;

class GUIRect {
    public:
    JSObjectRef scriptInstance; //!< The script representation
    GUIView* parent;
    bool visible;
    int width, height, posX, posY;
    GUIRect();
    virtual ~GUIRect();
    GUIRect* getRootParent();
    bool isFocused();
    virtual void setFocused(bool active);
    virtual bool getLimSize(GUIClipRect& clipRect, GUIClipRect& parentClipRect);
    virtual void updateContent();
    virtual void draw(const btVector3& parentTransform, GUIClipRect& parentClipRect) = 0;
    virtual bool handleMouseDown(int mouseX, int mouseY);
    virtual bool handleMouseUp(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY) { };
    virtual bool handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY) {
        return false;
    };
    virtual bool handleKeyDown(SDL_Keycode key, const char* text) {
        return false;
    };
    virtual bool handleKeyUp(SDL_Keycode key) {
        return false;
    };
};

#endif