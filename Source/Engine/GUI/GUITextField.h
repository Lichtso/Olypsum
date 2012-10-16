//
//  GUITextField.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIButton.h"
#import "GUIRoundedRect.h"
#import "GUILabel.h"

#ifndef GUITextField_h
#define GUITextField_h

class GUITextField : public GUIRect {
    GLuint texture;
    float cursorDrawTick;
    bool highlighted;
    void removeChar();
    void moveCursorLeft();
    void moveCursorRight();
    public:
    int cursorIndexX;
    GUILabel* label;
    bool enabled;
    std::function<void(GUITextField*)> onFocus, onChange, onBlur;
    GUITextField();
    ~GUITextField();
    void setFirstResponderStatus();
    void removeFirstResponderStatus();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleKeyDown(SDL_keysym* key);
};

#endif
