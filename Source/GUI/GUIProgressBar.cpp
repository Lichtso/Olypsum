//
//  GUIProgressBar.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
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
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(filled) {
        roundedRect.topColor.r = 100;
        roundedRect.topColor.g = 250;
        roundedRect.topColor.b = 190;
        roundedRect.bottomColor.r = 10;
        roundedRect.bottomColor.g = 150;
        roundedRect.bottomColor.b = 50;
    }else{
        roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 180;
        roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 255;
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

void GUIProgressBar::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRectL, clipRectR;
    if(!getLimSize(clipRectL, parentClipRect)) return;
    clipRectR = clipRectL;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    
    drawBar(clipRectL, true);
    drawBar(clipRectR, false);
}