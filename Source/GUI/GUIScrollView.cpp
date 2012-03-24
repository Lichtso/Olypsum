//
//  GUIScrollView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIScrollView.h"

GUIScrollView::GUIScrollView() {
    scrollPosX = scrollPosY = 0;
    scrollWidth = width;
    scrollHeight = height;
}

void GUIScrollView::updateContent() {
    
}

void GUIScrollView::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    GUIRect::draw(parentTransform, parentClipRect);
    clipRect.minPosX -= scrollPosX;
    clipRect.maxPosX -= scrollPosX;
    clipRect.minPosY -= scrollPosY;
    clipRect.maxPosY -= scrollPosY;
    
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX+scrollPosX, posY+scrollPosY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
}

bool GUIScrollView::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    return GUIView::handleMouseDown(mouseX-scrollPosX, mouseY-scrollPosY);
}

void GUIScrollView::handleMouseUp(int mouseX, int mouseY) {
    GUIView::handleMouseUp(mouseX-scrollPosX, mouseY-scrollPosY);
}

void GUIScrollView::handleMouseMove(int mouseX, int mouseY) {
    GUIView::handleMouseMove(mouseX-scrollPosX, mouseY-scrollPosY);
}

bool GUIScrollView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height || scrollHeight < height) return false;
    
    scrollPosY -= delta*20.0;
    if(scrollPosY < 0) scrollPosY = 0;
    else if(scrollPosY > scrollHeight-height) scrollPosY = scrollHeight-height;
    
    return true;
}