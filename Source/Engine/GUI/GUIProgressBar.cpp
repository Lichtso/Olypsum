//
//  GUIProgressBar.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIView.h"
#include "GUIProgressBar.h"

GUIProgressBar::GUIProgressBar() {
    type = GUIType_ProgressBar;
    value = 0.5;
    orientation = GUIOrientation_Horizontal;
    width = currentScreenView->width*0.2;
    height = currentScreenView->width*0.01;
}

void GUIProgressBar::updateContent() {
    if(orientation & GUIOrientation_Horizontal) {
        barL.width = barR.width = width;
        barL.height = barR.height = height;
    }else{
        barL.transposed = barR.transposed = true;
        barL.height = barR.height = height;
        barL.width = barR.width = width;
    }
    
    barL.innerShadow = barR.innerShadow = 0;
    barL.cornerRadius = barR.cornerRadius = currentScreenView->width*0.01;
    barL.borderColor = barR.borderColor = Color4(0.63);
    barL.topColor = Color4(0.4, 0.98, 0.75);
    barL.bottomColor = Color4(0.04, 0.59, 0.2);
    barR.topColor = Color4(0.71);
    barR.bottomColor = Color4(1.0);
    
    barL.drawInTexture();
    barR.drawInTexture();
}

void GUIProgressBar::drawBar(btVector3 transform, GUIClipRect clipRect, GUIRoundedRect& roundedRect) {
    if(orientation & GUIOrientation_Horizontal) {
        int splitPos = width*(value*2.0-1.0);
        if(&roundedRect == &barL)
            clipRect.maxPosX = min(clipRect.maxPosX, splitPos);
        else
            clipRect.minPosX = max(clipRect.minPosX, splitPos);
    }else{
        int splitPos = height*(value*2.0-1.0);
        if(&roundedRect == &barL)
            clipRect.maxPosY = min(clipRect.maxPosY, splitPos);
        else
            clipRect.minPosY = max(clipRect.minPosY, splitPos);
    }
    roundedRect.drawOnScreen(transform, 0, 0, clipRect);
}

void GUIProgressBar::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    transform += btVector3(posX, posY, 0.0);
    drawBar(transform, clipRect, barL);
    drawBar(transform, clipRect, barR);
}