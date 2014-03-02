//
//  GUIImage.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIImage.h"

GUIImage::GUIImage() :sizeAlignment(GUISizeAlignment::All) {
    
}

void GUIImage::updateContent() {
    if(!texture) return;
    switch(sizeAlignment) {
        case GUISizeAlignment::All:
            width = texture->width >> 1;
            height = texture->height >> 1;
        break;
        case GUISizeAlignment::Width:
            width = (float)texture->width/texture->height*height;
        break;
        case GUISizeAlignment::Height:
            height = (float)texture->height/texture->width*width;
        break;
        default:
        break;
    }
    content.width = width;
    content.height = height;
}

void GUIImage::draw(const btVector3& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    btVector3 transform = parentTransform + btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    if(texture)
        texture->use(0);
    else
        Texture::unbind(0, GL_TEXTURE_2D);
    content.drawOnScreen(transform, 0, 0, parentClipRect);
}