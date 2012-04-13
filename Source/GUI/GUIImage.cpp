//
//  GUIImage.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FileManager.h"

GUIImage::GUIImage() {
    texture = NULL;
    sizeAlignment = GUISizeAlignment_All;
}

GUIImage::~GUIImage() {
    if(texture) fileManager.releaseTexture(texture);
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

void GUIImage::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    GUIRoundedRect roundedRect;
    
    if(texture) {
        if(!texture->GLname) {
            texture->uploadToVRAM();
            texture->unloadFromRAM();
            updateContent();
        }
        roundedRect.texture = &texture->GLname;
    }else{
        GLuint textureP = 0;
        roundedRect.texture = &textureP;
    }
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.drawOnScreen(false, 0, 0, clipRect);
}