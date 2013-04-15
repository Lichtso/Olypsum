//
//  GUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <SDL/SDL.h>
#include "FileManager.h"
#include "GUIClipRect.h"

#ifndef GUIRect_h
#define GUIRect_h

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
    v8::Persistent<v8::Object> scriptInstance; //!< The script representation
    GUIRect* parent;
    bool visible;
    int width, height, posX, posY;
    GUIRect();
    virtual ~GUIRect();
    GUIRect* getRootParent();
    bool isFirstResponder();
    virtual void setFirstResponderStatus(bool active);
    virtual bool getLimSize(GUIClipRect& clipRect, GUIClipRect& parentClipRect);
    virtual void updateContent();
    virtual void draw(btVector3 transform, GUIClipRect& parentClipRect) = 0;
    virtual bool handleMouseDown(int mouseX, int mouseY);
    virtual bool handleMouseUp(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY);
    virtual bool handleMouseWheel(int mouseX, int mouseY, float delta);
    virtual bool handleKeyDown(SDL_keysym* key);
    virtual bool handleKeyUp(SDL_keysym* key);
};

#endif