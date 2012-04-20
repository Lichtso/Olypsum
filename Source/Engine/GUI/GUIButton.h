//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUILabel.h"

#ifndef GUIButton_h
#define GUIButton_h

enum GUIButtonState {
    GUIButtonStateDisabled = 0,
    GUIButtonStateNormal = 1,
    GUIButtonStateHighlighted = 2,
    GUIButtonStatePressed = 3
};

enum GUIButtonType {
    GUIButtonTypeNormal = 0,
    GUIButtonTypeDelete = 1,
    GUIButtonTypeAdd = 2,
    GUIButtonTypeEdit = 3,
    GUIButtonTypeLockable = 4
};

class GUIButton : public GUIView {
    GLuint texture;
    void setInnerPixel(unsigned char* pixel, unsigned int x, unsigned int y);
    public:
    GUISizeAlignment sizeAlignment;
    int paddingX, paddingY;
    GUIButtonType buttonType;
    GUICorners roundedCorners;
    GUIButtonState state;
    void (*onClick)(GUIButton*);
    GUIButton();
    ~GUIButton();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif