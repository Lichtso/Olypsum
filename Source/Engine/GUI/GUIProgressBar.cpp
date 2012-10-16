//
//  GUIProgressBar.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIProgressBar.h"

GUIProgressBar::GUIProgressBar() {
    type = GUIType_ProgressBar;
    textureL = textureR = 0;
    value = 0.5;
    orientation = GUIOrientation_Horizontal;
    width = 200;
    height = 10;
}

GUIProgressBar::~GUIProgressBar() {
    if(textureL) {
        glDeleteTextures(1, &textureL);
        glDeleteTextures(1, &textureR);
    }
}

void GUIProgressBar::generateBar(bool filled) {
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    if(orientation & GUIOrientation_Horizontal) {
        roundedRect.width = width;
        roundedRect.height = height;
    }else{
        roundedRect.width = height;
        roundedRect.height = width;
    }
    
    roundedRect.cornerRadius = 6;
    roundedRect.borderColor = Color4(0.63);
    if(filled) {
        roundedRect.topColor = Color4(0.4, 0.98, 0.75);
        roundedRect.bottomColor = Color4(0.04, 0.59, 0.2);
    }else{
        roundedRect.topColor = Color4(0.71);
        roundedRect.bottomColor = Color4(1.0);
    }
    roundedRect.drawInTexture();
}

void GUIProgressBar::updateContent() {
    generateBar(true);
    generateBar(false);
}

void GUIProgressBar::drawBar(GUIClipRect clipRect, bool filled) {
    int splitPos = ((orientation & GUIOrientation_Horizontal) ? width : height)*(value*2.0-1.0);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    roundedRect.width = width;
    roundedRect.height = height;
    if(orientation & GUIOrientation_Horizontal) {
        if(filled)
            clipRect.maxPosX = min(clipRect.maxPosX, splitPos);
        else
            clipRect.minPosX = max(clipRect.minPosX, splitPos);
        roundedRect.drawOnScreen(false, 0, 0, clipRect);
    }else{
        if(filled)
            clipRect.maxPosY = min(clipRect.maxPosY, splitPos);
        else
            clipRect.minPosY = max(clipRect.minPosY, splitPos);
        roundedRect.drawOnScreen(true, 0, 0, clipRect);
    }
}

void GUIProgressBar::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRectL, clipRectR;
    if(!getLimSize(clipRectL, parentClipRect)) return;
    clipRectR = clipRectL;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    shaderPrograms[spriteSP]->use();
    
    drawBar(clipRectL, true);
    drawBar(clipRectR, false);
}