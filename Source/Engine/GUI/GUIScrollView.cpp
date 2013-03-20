//
//  GUIScrollView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIScrollView.h"

#define scrollBarWidth (currentScreenView->width*0.005)

int GUIScrollView::getBarWidth() {
    if(contentWidth > width*2 && contentHeight > height*2)
        return ((float)(width-scrollBarWidth)/contentWidth)*width;
    else
        return ((float)width/contentWidth)*width;
}

int GUIScrollView::getBarHeight() {
    if(contentWidth > width*2 && contentHeight > height*2)
        return ((float)(height-scrollBarWidth)/contentHeight)*height;
    else
        return ((float)height/contentHeight)*height;
}

int GUIScrollView::getBarPosX() {
    int barWidth = getBarWidth();
    if(contentWidth > width*2 && contentHeight > height*2)
        return (float)scrollPosX/(contentWidth-width*2)*(width*2-scrollBarWidth*2-barWidth*2-5)-width+barWidth+2;
    else
        return (float)scrollPosX/(contentWidth-width*2)*(width*2-barWidth*2-5)-width+barWidth+2;
}

int GUIScrollView::getBarPosY() {
    int barHeight = getBarHeight();
    if(contentWidth > width*2 && contentHeight > height*2)
        return (1.0-(float)scrollPosY/(contentHeight-height*2))*(height*2-scrollBarWidth*2-barHeight*2-5)-height+scrollBarWidth*2+barHeight+2;
    else
        return (1.0-(float)scrollPosY/(contentHeight-height*2))*(height*2-barHeight*2-5)-height+barHeight+2;
}

GUIScrollView::GUIScrollView() {
    scrollPosX = scrollPosY = 0;
    contentWidth = width;
    contentHeight = height;
    content.innerShadow = 0;
    mouseDragPosX = mouseDragPosY = -2;
    hideSliderX = hideSliderY = true;
}

void GUIScrollView::updateContent() {
    GUIFramedView::updateContent();
    
    if(contentWidth > width*2) {
        sliderH.width = getBarWidth();
        sliderH.height = scrollBarWidth;
        sliderH.cornerRadius = scrollBarWidth;
        sliderH.drawInTexture();
    }
    
    if(contentHeight > height*2) {
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
    
    if(contentWidth > width*2 && mouseDragPosX > -2)
        sliderH.drawOnScreen(transform, getBarPosX(), 1.5*scrollBarWidth-1-height, fixedClipRect);
    
    if(contentHeight > height*2 && mouseDragPosY > -2)
        sliderV.drawOnScreen(transform, width-1.5*scrollBarWidth, getBarPosY(), fixedClipRect);
}

bool GUIScrollView::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    int barPosY = getBarPosY(), barHeight = getBarHeight();
    if(contentHeight > height*2 &&
       mouseX >= width-2.5*scrollBarWidth && mouseX <= width-0.5*scrollBarWidth &&
       mouseY >= barPosY-barHeight && mouseY <= barPosY+barHeight) {
        mouseDragPosY = mouseY-barPosY+barHeight;
        return true;
    }
    
    int barPosX = getBarPosX(), barWidth = getBarWidth();
    if(contentWidth > width*2 &&
       mouseX >= barPosX-barWidth && mouseX <= barPosX+barWidth &&
       mouseY >= 0.5*scrollBarWidth-height && mouseY <= 2.5*scrollBarWidth-height) {
        mouseDragPosX = mouseX-barPosX+barWidth;
        return true;
    }
    
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseDown(mouseX-children[i]->posX+scrollPosX, mouseY-children[i]->posY-scrollPosY))
            return true;
    return false;
}

bool GUIScrollView::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPosX = (contentWidth > width*2 &&
                     (!hideSliderX || (mouseX >= -width && mouseX <= width &&
                                       mouseY >= -height && mouseY <= 3*scrollBarWidth-height))) ? -1 : -2;
    mouseDragPosY = (contentHeight > height*2 &&
                     (!hideSliderY || (mouseX >= width-3*scrollBarWidth && mouseX <= width &&
                                       mouseY >= -height && mouseY <= height))) ? -1 : -2;
    
    return GUIView::handleMouseUp(mouseX+scrollPosX, mouseY-scrollPosY);
}

void GUIScrollView::handleMouseMove(int mouseX, int mouseY) {
    if(contentWidth > width*2) {
        int barWidth = getBarWidth(), widthB = (contentHeight > height*2) ? width-scrollBarWidth : width;
        if(mouseDragPosX >= 0) {
            scrollPosX = ((float)mouseX-2+width-mouseDragPosX)/(widthB*2-barWidth*2-4)*(contentWidth-width*2);
            if(scrollPosX < 0) scrollPosX = 0;
            else if(scrollPosX > contentWidth-width*2) scrollPosX = contentWidth-width*2;
            return;
        }else mouseDragPosX = (!hideSliderX || (mouseX >= -width && mouseX <= width && mouseY >= -height && mouseY <= 3*scrollBarWidth-height)) ? -1 : -2;
    }else mouseDragPosX = -2;
    
    if(contentHeight > height) {
        int barHeight = getBarHeight(), heightB = (contentWidth > width) ? height-scrollBarWidth : height;
        if(mouseDragPosY >= 0) {
            scrollPosY = ((float)height-mouseY-barHeight*2-2+mouseDragPosY)/(heightB*2-barHeight*2-4)*(contentHeight-height*2);
            if(scrollPosY < 0) scrollPosY = 0;
            else if(scrollPosY > contentHeight-height*2) scrollPosY = contentHeight-height*2;
            return;
        }else mouseDragPosY = (!hideSliderY || (mouseX >= width-3*scrollBarWidth && mouseX <= width && mouseY >= -height && mouseY <= height)) ? -1 : -2;
    }else mouseDragPosY = -2;
    
    GUIView::handleMouseMove(mouseX+scrollPosX, mouseY-scrollPosY);
}

bool GUIScrollView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseWheel(mouseX-children[i]->posX+scrollPosX, mouseY-children[i]->posY-scrollPosY, delta))
            return true;
    
    if(contentHeight < height*2) return false;
    
    scrollPosY -= delta*10.0;
    if(scrollPosY < 0) scrollPosY = 0;
    else if(scrollPosY > contentHeight-height*2) scrollPosY = contentHeight-height*2;
    
    return true;
}