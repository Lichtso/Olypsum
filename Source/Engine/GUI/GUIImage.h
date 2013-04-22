//
//  GUIImage.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUIImage_h
#define GUIImage_h

#include "GUITextField.h"

class GUIImage : public GUIRect {
    public:
    GUIDrawableRect content;
    GUISizeAlignment sizeAlignment;
    FileResourcePtr<Texture> texture;
    GUIImage();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
};

#endif
