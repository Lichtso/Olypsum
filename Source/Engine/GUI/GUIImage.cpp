//
//  GUIImage.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "FileManager.h"
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

void GUIImage::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    texture->use(0);
    content.drawOnScreen(transform, 0, 0, parentClipRect);
}