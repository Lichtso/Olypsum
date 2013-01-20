//
//  GUIScrollView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIScrollView.h"

#define scrollBarWidth 6

int GUIScrollView::getBarWidth() {
    if(scrollWidth > width && scrollHeight > height)
        return ((float)(width-scrollBarWidth)/scrollWidth)*width;
    else
        return ((float)width/scrollWidth)*width;
}

int GUIScrollView::getBarHeight() {
    if(scrollWidth > width && scrollHeight > height)
        return ((float)(height-scrollBarWidth)/scrollHeight)*height;
    else
        return ((float)height/scrollHeight)*height;
}

int GUIScrollView::getBarPosX() {
    int barWidth = getBarWidth();
    if(scrollWidth > width && scrollHeight > height)
        return (float)scrollPosX/(scrollWidth-width)*(width*2-scrollBarWidth*2-barWidth*2-5)-width+barWidth+2;
    else
        return (float)scrollPosX/(scrollWidth-width)*(width*2-barWidth*2-5)-width+barWidth+2;
}

int GUIScrollView::getBarPosY() {
    int barHeight = getBarHeight();
    if(scrollWidth > width && scrollHeight > height)
        return (1.0-(float)scrollPosY/(scrollHeight-height))*(height*2-scrollBarWidth*2-barHeight*2-5)-height+scrollBarWidth*2+barHeight+2;
    else
        return (1.0-(float)scrollPosY/(scrollHeight-height))*(height*2-barHeight*2-5)-height+barHeight+2;
}

GUIScrollView::GUIScrollView() {
    scrollPosX = scrollPosY = 0;
    scrollWidth = width;
    scrollHeight = height;
    content.innerShadow = 0;
    mouseDragPosX = mouseDragPosY = -2;
    hideSliderX = hideSliderY = true;
}

void GUIScrollView::updateContent() {
    GUIFramedView::updateContent();
    
    if(scrollWidth > width) {
        sliderH.width = getBarWidth();
        sliderH.height = scrollBarWidth;
        sliderH.cornerRadius = scrollBarWidth;
        sliderH.drawInTexture();
    }
    
    if(scrollHeight > height) {
        sliderV.width = scrollBarWidth;
        sliderV.height = getBarHeight();
        sliderV.cornerRadius = scrollBarWidth;
        sliderV.drawInTexture();
    }
}

void GUIScrollView::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect, fixedClipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    float inset = abs(content.innerShadow);
    if(content.innerShadow != 0)
        content.drawOnScreen(transform, 0, 0, clipRect);
    fixedClipRect = clipRect;
    
    clipRect.minPosX += scrollPosX+inset;
    clipRect.maxPosX += scrollPosX-inset;
    clipRect.minPosY += -scrollPosY+inset;
    clipRect.maxPosY += -scrollPosY-inset;
    btVector3 childTransform = transform+btVector3(-scrollPosX, scrollPosY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(childTransform, clipRect);
    
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    
    if(scrollWidth > width && mouseDragPosX > -2)
        sliderH.drawOnScreen(transform, getBarPosX(), 8-height, fixedClipRect);
    
    if(scrollHeight > height && mouseDragPosY > -2)
        sliderV.drawOnScreen(transform, width-9, getBarPosY(), fixedClipRect);
}

bool GUIScrollView::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    int barPosY = getBarPosY(), barHeight = getBarHeight();
    if(scrollHeight > height && mouseX >= width-15 && mouseX <= width-3 && mouseY >= barPosY-barHeight && mouseY <= barPosY+barHeight) {
        mouseDragPosY = mouseY-barPosY+barHeight;
        return true;
    }
    
    int barPosX = getBarPosX(), barWidth = getBarWidth();
    if(scrollWidth > width && mouseX >= barPosX-barWidth && mouseX <= barPosX+barWidth && mouseY >= 3-height && mouseY <= 15-height) {
        mouseDragPosX = mouseX-barPosX+barWidth;
        return true;
    }
    
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseDown(mouseX-children[i]->posX+scrollPosX, mouseY-children[i]->posY-scrollPosY))
            return true;
    return false;
}

bool GUIScrollView::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPosX = (scrollWidth > width && (!hideSliderX || (mouseX >= -width && mouseX <= width && mouseY >= -height && mouseY <= 18-height))) ? -1 : -2;
    mouseDragPosY = (scrollHeight > height && (!hideSliderY || (mouseX >= width-18 && mouseX <= width && mouseY >= -height && mouseY <= height))) ? -1 : -2;
    
    return GUIView::handleMouseUp(mouseX+scrollPosX, mouseY-scrollPosY);
}

void GUIScrollView::handleMouseMove(int mouseX, int mouseY) {
    if(scrollWidth > width) {
        int barWidth = getBarWidth(), widthB = (scrollHeight > height) ? width-scrollBarWidth : width;
        if(mouseDragPosX >= 0) {
            scrollPosX = ((float)mouseX-2+width-mouseDragPosX)/(widthB*2-barWidth*2-4)*(scrollWidth-width);
            if(scrollPosX < 0) scrollPosX = 0;
            else if(scrollPosX > scrollWidth-width) scrollPosX = scrollWidth-width;
            return;
        }else mouseDragPosX = (!hideSliderX || (mouseX >= -width && mouseX <= width && mouseY >= -height && mouseY <= 18-height)) ? -1 : -2;
    }else mouseDragPosX = -2;
    
    if(scrollHeight > height) {
        int barHeight = getBarHeight(), heightB = (scrollWidth > width) ? height-scrollBarWidth : height;
        if(mouseDragPosY >= 0) {
            scrollPosY = ((float)height-mouseY-barHeight*2-2+mouseDragPosY)/(heightB*2-barHeight*2-4)*(scrollHeight-height);
            if(scrollPosY < 0) scrollPosY = 0;
            else if(scrollPosY > scrollHeight-height) scrollPosY = scrollHeight-height;
            return;
        }else mouseDragPosY = (!hideSliderY || (mouseX >= width-18 && mouseX <= width && mouseY >= -height && mouseY <= height)) ? -1 : -2;
    }else mouseDragPosY = -2;
    
    GUIView::handleMouseMove(mouseX+scrollPosX, mouseY-scrollPosY);
}

bool GUIScrollView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseWheel(mouseX-children[i]->posX+scrollPosX, mouseY-children[i]->posY-scrollPosY, delta))
            return true;
    
    if(scrollHeight < height) return false;
    
    scrollPosY -= delta*10.0;
    if(scrollPosY < 0) scrollPosY = 0;
    else if(scrollPosY > scrollHeight-height) scrollPosY = scrollHeight-height;
    
    return true;
}