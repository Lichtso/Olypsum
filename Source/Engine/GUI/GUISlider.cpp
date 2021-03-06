//
//  GUISlider.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "../Menu/Menu.h"

#define barHeight (menu.screenView->width*0.01)
#define sliderRadius (menu.screenView->width*0.02)

GUISlider::GUISlider() :mouseDragPos(-1), enabled(true), highlighted(false), value(0.5), steps(0), orientation(GUIOrientation::Horizontal) {
    
}

void GUISlider::updateContent() {
    if(orientation & GUIOrientation::Horizontal) {
        if(width < sliderRadius) width = sliderRadius;
        height = sliderRadius;
        barL.width = barR.width = width-sliderRadius+barHeight;
        barL.height = barR.height = barHeight;
    }else{
        width = sliderRadius;
        if(height < sliderRadius) height = sliderRadius;
        barL.transposed = barR.transposed = true;
        barL.width = barR.width = barHeight;
        barL.height = barR.height = height-sliderRadius+barHeight;
    }
    
    barL.cornerRadius = barR.cornerRadius = barHeight;
    barL.topColor = Color4(0.55, 0.78, 0.94);
    barL.bottomColor = Color4(0.0, 0.24, 0.63);
    barR.topColor = Color4(0.71);
    barR.bottomColor = Color4(1.0);
    barL.borderColor = barR.borderColor = Color4(0.63);
    slider.width = sliderRadius;
    slider.height = sliderRadius;
    slider.cornerRadius = sliderRadius;
    slider.topColor = Color4(0.98);
    slider.bottomColor = Color4(0.82);
    slider.borderColor = Color4(0.63);
    if(enabled) {
        slider.edgeGradientCenter = (highlighted) ? 1.2 : 1.0;
        slider.edgeGradientBorder = 1.0;
    }else{
        slider.edgeGradientCenter = 0.6;
        slider.edgeGradientBorder = 0.8;
    }
    barL.updateContent();
    barR.updateContent();
    slider.updateContent();
}

void GUISlider::drawBar(const btVector3& parentTransform, GUIClipRect clipRect, GUIRoundedRect& roundedRect) {
    if(orientation & GUIOrientation::Horizontal) {
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
    roundedRect.drawOnScreen(parentTransform, 0, 0, clipRect);
}

void GUISlider::draw(const btVector3& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    btVector3 transform = parentTransform + btVector3(posX, posY, 0.0);
    
    drawBar(transform, clipRect, barL);
    drawBar(transform, clipRect, barR);
    
    int barLength = ((orientation & GUIOrientation::Horizontal) ? width : height)-sliderRadius;
    if(orientation & GUIOrientation::Horizontal)
        slider.drawOnScreen(transform, 2.0*barLength*value-width+sliderRadius, 0, clipRect);
    else
        slider.drawOnScreen(transform, 0, 2.0*barLength*value-height+sliderRadius, clipRect);
}

bool GUISlider::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled) return false;
    highlighted = false;
    
    if(orientation & GUIOrientation::Horizontal) {
        mouseX -= (width-sliderRadius)*(2.0*value-1.0);
        if(mouseX*mouseX + mouseY*mouseY > sliderRadius*sliderRadius) return false;
        mouseDragPos = mouseX;
    }else{
        mouseY -= (height-sliderRadius)*(2.0*value-1.0);
        if(mouseX*mouseX + mouseY*mouseY > sliderRadius*sliderRadius) return false;
        mouseDragPos = mouseY;
    }
    
    highlighted = true;
    return true;
}

bool GUISlider::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return false;
    
    if(mouseDragPos != -1) {
        if(steps) {
            value = roundf(value*(float)steps)/(float)steps;
            if(onChange) onChange(this, false);
        }
        
        if(!handleMouseDown(mouseX, mouseY))
            updateContent();
        
        mouseDragPos = -1;
    }
    
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
    
    if(orientation & GUIOrientation::Horizontal)
        value = 0.5+0.5*(mouseX-mouseDragPos)/(width-sliderRadius);
    else
        value = 0.5+0.5*(mouseY-mouseDragPos)/(height-sliderRadius);
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    
    if(onChange) onChange(this, true);
}

bool GUISlider::handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -barHeight || mouseY > barHeight) return false;
    
    float delta = (orientation & GUIOrientation::Horizontal) ? deltaX : deltaY;
    value -= (steps) ? delta/(float)steps : delta*0.05;
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    if(steps) value = roundf(value*(float)steps)/(float)steps;
    handleMouseMove(mouseX, mouseY);
    
    if(onChange) onChange(this, false);
    return true;
}