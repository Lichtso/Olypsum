//
//  GUITextField.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef GUITextField_h
#define GUITextField_h

#include "GUISlider.h"

class GUITextField : public GUIFramedView {
    GUIRoundedRect cursor;
    float cursorDrawTick;
    bool highlighted;
    void insertStr(const char* str);
    void removeChar();
    void moveCursorLeft();
    void moveCursorRight();
    public:
    unsigned int cursorX;
    bool enabled;
    std::function<void(GUITextField*)> onFocus, onChange;
    GUITextField();
    void setFocused(bool active);
    bool addChild(GUIRect* child);
    void updateContent();
    void draw(const btVector3& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleKeyDown(SDL_Keycode key, const char* text);
    //! Sets the offset of the cursorX in UTF8 characters
    void setCursorX(unsigned int cursorX);
    //! Returns the offset of the cursorX in UTF8 characters
    unsigned int getCursorX();
};

#endif
