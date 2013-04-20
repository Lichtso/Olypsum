//
//  GUITextField.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIButton.h"
#include "GUIRoundedRect.h"
#include "GUILabel.h"

#ifndef GUITextField_h
#define GUITextField_h

void setClipboardText(const char* str);
std::string getClipboardText();
bool hasClipboardText();

class GUITextField : public GUIFramedView {
    float cursorDrawTick;
    bool highlighted;
    void insertStr(const char* str);
    void removeChar();
    void moveCursorLeft();
    void moveCursorRight();
    public:
    int cursorIndexX;
    bool enabled;
    std::function<void(GUITextField*)> onFocus, onChange, onBlur;
    GUITextField();
    void setFocus(bool active);
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleKeyDown(SDL_keysym* key);
};

#endif
