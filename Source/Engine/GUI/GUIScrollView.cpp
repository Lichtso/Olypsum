//
//  GUIScrollView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIScrollView.h"
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
    innerShadow = 0;
    textureV = textureH = 0;
    mouseDragPosX = mouseDragPosY = -2;
    hideSliderX = hideSliderY = true;
}

GUIScrollView::~GUIScrollView() {
    if(textureV) glDeleteTextures(1, &textureV);
    if(textureH) glDeleteTextures(1, &textureH);
}

void GUIScrollView::updateContent() {
    GUIView::updateContent();
    
    GUIRoundedRect roundedRect;
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 220;
    roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 140;
    roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 140;
    
    if(innerShadow != 0) {
        GUIRoundedRect roundedRect;
        roundedRect.texture = &texture;
        roundedRect.width = width;
        roundedRect.height = height;
        roundedRect.innerShadow = innerShadow*screenSize[2];
        roundedRect.cornerRadius = abs(innerShadow)*screenSize[2];
        roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 200;
        roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 200;
        roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 200;
        roundedRect.drawInTexture();
    }
    
    if(scrollWidth > width) {
        roundedRect.texture = &textureH;
        roundedRect.width = getBarWidth();
        roundedRect.height = scrollBarWidth;
        roundedRect.cornerRadius = scrollBarWidth;
        roundedRect.drawInTexture();
    }
    
    if(scrollHeight > height) {
        roundedRect.texture = &textureV;
        roundedRect.width = scrollBarWidth;
        roundedRect.height = getBarHeight();
        roundedRect.cornerRadius = scrollBarWidth;
        roundedRect.drawInTexture();
    }
}

void GUIScrollView::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if((scrollWidth > width && !textureH) || (scrollHeight > height && !textureV)) updateContent();
    
    GUIRoundedRect roundedRect;
    GUIClipRect clipRect, fixedClipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    fixedClipRect = clipRect;
    clipRect.minPosX += scrollPosX;
    clipRect.maxPosX += scrollPosX;
    clipRect.minPosY -= scrollPosY;
    clipRect.maxPosY -= scrollPosY;
    
    Matrix4 transform = parentTransform;
    transform.translate(Vector3(posX, posY, 0.0));
    
    if(innerShadow != 0) {
        if(!texture) updateContent();
        modelMat = transform;
        GUIRoundedRect roundedRect;
        roundedRect.texture = &texture;
        roundedRect.width = width;
        roundedRect.height = height;
        roundedRect.drawOnScreen(false, 0, 0, clipRect);
        float inset = abs(innerShadow)*screenSize[2];
        clipRect.minPosX += inset;
        clipRect.maxPosX -= inset;
        clipRect.minPosY += inset;
        clipRect.maxPosY -= inset;
    }
    
    transform.translate(Vector3(-scrollPosX, scrollPosY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    
    if(scrollWidth > width && mouseDragPosX > -2) {
        roundedRect.texture = &textureH;
        roundedRect.width = getBarWidth();
        roundedRect.height = scrollBarWidth;
        roundedRect.drawOnScreen(false, getBarPosX(), 8-height, fixedClipRect);
    }
    
    if(scrollHeight > height && mouseDragPosY > -2) {
        roundedRect.texture = &textureV;
        roundedRect.width = scrollBarWidth;
        roundedRect.height = getBarHeight();
        roundedRect.drawOnScreen(false, width-9, getBarPosY(), fixedClipRect);
    }
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