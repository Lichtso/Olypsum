//
//  GUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "TextFont.h"

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

struct GUIClipRect {
    int minPosX, minPosY, maxPosX, maxPosY;
};

struct GUIColor {
    unsigned char r, g, b, a;
};

class GUIRect {
    public:
    GUIElementType type;
    GUIRect* parent;
    bool visible;
    int posX, posY, width, height;
    GUIRect();
    GUIRect* getRootParent();
    virtual void updateFirstResponderStatus();
    virtual void getLimSize(GUIClipRect* parentClipRect, GUIClipRect* clipRect);
    virtual void updateContent();
    virtual void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    virtual bool handleMouseDown(int mouseX, int mouseY);
    virtual void handleMouseUp(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY);
    virtual bool handleMouseWheel(int mouseX, int mouseY, float delta);
    virtual bool handleKeyDown(SDL_keysym* key);
    virtual bool handleKeyUp(SDL_keysym* key);
};

#endif