//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUILabel.h"
#import "GUIView.h"
#import "GUIRoundedRect.h"

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
    std::function<void(GUIButton*)> onClick;
    GUIButton();
    ~GUIButton();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif