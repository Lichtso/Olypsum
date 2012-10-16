//
//  GUISlider.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUISlider.h"

#define barHeight (6*screenSize[2])
#define sliderRadius (12*screenSize[2])

GUISlider::GUISlider() {
    type = GUIType_Silder;
    textureL = textureM = textureR = 0;
    mouseDragPos = -1;
    highlighted = false;
    value = 0.5;
    steps = 0;
    orientation = GUIOrientation_Horizontal;
    enabled = true;
}

GUISlider::~GUISlider() {
    if(textureL) {
        glDeleteTextures(1, &textureL);
        glDeleteTextures(1, &textureM);
        glDeleteTextures(1, &textureR);
    }
}

void GUISlider::generateBar(bool filled) {
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    roundedRect.width = ((orientation & GUIOrientation_Horizontal) ? width : height)-sliderRadius+barHeight;
    roundedRect.height = barHeight;
    roundedRect.cornerRadius = barHeight;
    roundedRect.borderColor = Color4(0.63);
    if(enabled) {
        if(filled) {
            roundedRect.borderColor = Color4(0.0, 0.24, 0.63);
            roundedRect.bottomColor = Color4(0.55, 0.78, 0.94);
        }else{
            roundedRect.topColor = Color4(0.71);
            roundedRect.bottomColor = Color4(1.0);
        }
    }else{
        if(filled) {
            roundedRect.borderColor = Color4(0.0, 0.12, 0.51);
            roundedRect.bottomColor = Color4(0.35, 0.59, 0.75);
        }else{
            roundedRect.topColor = Color4(0.47);
            roundedRect.bottomColor = Color4(0.71);
        }
    }
    roundedRect.drawInTexture();
}

void GUISlider::updateContent() {
    if(orientation & GUIOrientation_Horizontal) {
        if(width < sliderRadius) width = sliderRadius;
        height = sliderRadius;
    }else{
        width = sliderRadius;
        if(height < sliderRadius) height = sliderRadius;
    }
    
    generateBar(true);
    generateBar(false);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &textureM;
    roundedRect.width = sliderRadius;
    roundedRect.height = sliderRadius;
    roundedRect.cornerRadius = sliderRadius;
    roundedRect.borderColor = Color4(0.63);
    if(enabled) {
        if(highlighted) {
            roundedRect.borderColor = Color4(0.98);
            roundedRect.bottomColor = Color4(0.82);
        }else{
            roundedRect.borderColor = Color4(0.94);
            roundedRect.bottomColor = Color4(0.71);
        }
    }else{
        roundedRect.borderColor = Color4(0.59);
        roundedRect.bottomColor = Color4(0.43);
    }
    roundedRect.drawInTexture();
}

void GUISlider::drawBar(GUIClipRect& clipRect, unsigned int barLength, bool filled) {
    GUIClipRect clipRectB;
    int silderPos = (int)(barLength*value)-barLength*0.5;
    barLength += barHeight*2;
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    
    if(orientation & GUIOrientation_Horizontal) {
        roundedRect.width = width-sliderRadius+barHeight;
        roundedRect.height = barHeight;
        if(filled)
            clipRect.maxPosX = min(clipRect.maxPosX, silderPos);
        else
            clipRect.minPosX = max(clipRect.minPosX, silderPos);
        roundedRect.drawOnScreen(false, 0, 0, clipRect);
    }else{
        roundedRect.width = barHeight;
        roundedRect.height = height-sliderRadius+barHeight;
        if(filled)
            clipRect.maxPosY = min(clipRect.maxPosY, silderPos);
        else
            clipRect.minPosY = max(clipRect.minPosY, silderPos);
        roundedRect.drawOnScreen(true, 0, 0, clipRect);
    }
}

void GUISlider::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRect, clipRectB, clipRectC;
    if(!getLimSize(clipRect, parentClipRect)) return;
    clipRectB = clipRectC = clipRect;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    shaderPrograms[spriteSP]->use();
    
    int barLength = ((orientation & GUIOrientation_Horizontal) ? width*2 : height*2)-sliderRadius*2;
    drawBar(clipRectB, barLength, true);
    drawBar(clipRectC, barLength, false);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &textureM;
    roundedRect.width = sliderRadius;
    roundedRect.height = sliderRadius;
    if(orientation & GUIOrientation_Horizontal)
        roundedRect.drawOnScreen(false, barLength*value-width+sliderRadius, 0, clipRect);
    else
        roundedRect.drawOnScreen(false, 0, barLength*value-height+sliderRadius, clipRect);
}

bool GUISlider::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled) return false;
    highlighted = false;
    
    if(orientation & GUIOrientation_Horizontal) {
        mouseX -= (width-sliderRadius)*(2.0*value-1.0);
        if(mouseX < -sliderRadius || mouseX > sliderRadius || mouseY < -height || mouseY > height) return false;
        mouseDragPos = mouseX;
    }else{
        mouseY -= (height-sliderRadius)*(2.0*value-1.0);
        if(mouseX < -width || mouseX > width || mouseY < -sliderRadius || mouseY > sliderRadius) return false;
        mouseDragPos = mouseY;
    }
    
    highlighted = true;
    return true;
}

bool GUISlider::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPos = -1;
    return false;
}

void GUISlider::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled) return;
    
    if(mouseDragPos == -1) {
        bool prevHighlighted = highlighted;
        
        if(handleMouseDown(mouseX, mouseY))
            mouseDragPos = -1;
        
        if(prevHighlighted != highlighted)
            updateContent();
        
        return;
    }
    
    if(orientation & GUIOrientation_Horizontal)
        value = 0.5+0.5*(mouseX-mouseDragPos)/(width-sliderRadius);
    else
        value = 0.5+0.5*(mouseY-mouseDragPos)/(height-sliderRadius);
    
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    if(steps) value = roundf(value*(float)steps)/(float)steps;
    
    if(onChange) onChange(this);
}

bool GUISlider::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    value -= (steps) ? delta/(float)steps : delta*0.05;
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    if(steps) value = roundf(value*(float)steps)/(float)steps;
    
    if(onChange) onChange(this);
    return true;
}