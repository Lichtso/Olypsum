//
//  GUISlider.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUISlider.h"

#define barHeight 6
#define sliderRadius 12

GUISlider::GUISlider() {
    type = GUIType_Silder;
    textureL = textureM = textureR = 0;
    mouseDragPos = -1;
    highlighted = false;
    value = 0.5;
    orientation = GUIOrientation_Horizontal;
    onChange = NULL;
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
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(enabled) {
        if(filled) {
            roundedRect.topColor.r = 0;
            roundedRect.topColor.g = 60;
            roundedRect.topColor.b = 160;
            roundedRect.bottomColor.r = 140;
            roundedRect.bottomColor.g = 200;
            roundedRect.bottomColor.b = 240;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g =roundedRect.topColor.b = 180;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g =roundedRect.bottomColor.b = 255;
        }
    }else{
        if(filled) {
            roundedRect.topColor.r = 0;
            roundedRect.topColor.g = 30;
            roundedRect.topColor.b = 130;
            roundedRect.bottomColor.r = 90;
            roundedRect.bottomColor.g = 150;
            roundedRect.bottomColor.b = 190;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g =roundedRect.topColor.b = 120;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g =roundedRect.bottomColor.b = 180;
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
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(enabled) {
        if(highlighted) {
            roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 250;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 210;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 240;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 180;
        }
    }else{
        roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 150;
        roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 110;
    }
    roundedRect.drawInTexture();
}

void GUISlider::drawBar(GUIClipRect& clipRect, unsigned int barLength, bool filled) {
    GUIClipRect clipRectB;
    int silderPos = (int)(barLength*value)-barLength*0.5;
    Vector3 minFactor, maxFactor;
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

void GUISlider::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRect, clipRectB, clipRectC;
    if(!getLimSize(clipRect, parentClipRect)) return;
    clipRectB = clipRectC = clipRect;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    
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

void GUISlider::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPos = -1;
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
    
    if(onChange)
        onChange(this);
}

bool GUISlider::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    value -= delta*0.05;
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    
    return true;
}