//
//  GUIImage.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIRoundedRect.h"
#import "GUIRect.h"

#ifndef GUIImage_h
#define GUIImage_h

class GUIImage : public GUIRect {
    public:
    GUISizeAlignment sizeAlignment;
    std::shared_ptr<Texture> texture;
    GUIImage();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
};

#endif
