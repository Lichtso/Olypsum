//
//  GUITextField.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUISlider.h"

#ifndef GUITextField_h
#define GUITextField_h

class GUITextField : public GUIRect {
    GLuint texture;
    bool highlighted;
    bool isFirstResponder();
    public:
    GUILabel* label;
    bool enabled;
    void (*onFocus)(GUITextField*);
    void (*onChange)(GUITextField*);
    void (*onBlur)(GUITextField*);
    GUITextField();
    ~GUITextField();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleKeyDown(SDL_keysym* key);
    bool handleKeyUp(SDL_keysym* key);
};

#endif
