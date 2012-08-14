//
//  GUIImage.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUITextField.h"

#ifndef GUIImage_h
#define GUIImage_h

class GUIImage : public GUIRect {
    public:
    GUISizeAlignment sizeAlignment;
    Texture* texture;
    GUIImage();
    ~GUIImage();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
};

#endif
