//
//  GUIImage.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "FileManager.h"
#import "GUIImage.h"

GUIImage::GUIImage() :sizeAlignment(GUISizeAlignment_All) {
    
}

void GUIImage::updateContent() {
    if(!texture) return;
    switch(sizeAlignment) {
        case GUISizeAlignment_All:
            width = texture->width >> 1;
            height = texture->height >> 1;
        break;
        case GUISizeAlignment_Width:
            width = (float)texture->width/texture->height*height;
        break;
        case GUISizeAlignment_Height:
            height = (float)texture->height/texture->width*width;
        break;
        default:
        break;
    }
}

void GUIImage::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    GUIRoundedRect roundedRect;
    GLuint textureP = 0;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    roundedRect.texture = (texture) ? &texture->GLname : &textureP;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.drawOnScreen(false, 0, 0, clipRect);
}