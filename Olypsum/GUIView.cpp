//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIView.h"

void GUIView::addChild(GUIRect* child) {
    children.push_back(child);
    child->parent = this;
    
}

void GUIView::updateContent() {
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIView::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX, posY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
}

void GUIView::handleMouseDown(int mouseX, int mouseY) {
    if(mouseX < posX-width || mouseX > posX+width || mouseY < posY-height || mouseY > posY+height) return;
    mouseX -= posX; mouseY -= posY;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

void GUIView::handleMouseUp(int mouseX, int mouseY) {
    if(mouseX < posX-width || mouseX > posX+width || mouseY < posY-height || mouseY > posY+height) return;
    mouseX -= posX; mouseY -= posY;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

void GUIView::handleMouseMove(int mouseX, int mouseY) {
    if(mouseX < posX-width || mouseX > posX+width || mouseY < posY-height || mouseY > posY+height) return;
    mouseX -= posX; mouseY -= posY;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}



GUIScreenView::GUIScreenView() {
    parent = NULL;
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
}

void GUIScreenView::getLimSize(GUIClipRect* clipRect) {
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
    clipRect->minPosX = -width;
    clipRect->minPosY = -height;
    clipRect->maxPosX = width;
    clipRect->maxPosY = height;
}

void GUIScreenView::updateContent() {
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::draw() {
    if(!visible) return;
    
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    spriteShaderProgram->setUnfiformF("light", 1.0);
    
    GUIClipRect clipRect;
    getLimSize(&clipRect);
    
    Matrix4 transform;
    transform.setIdentity();
    transform.translate(Vector3(0.0, 0.0, -1.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
    
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glEnable(GL_DEPTH_TEST);
}

void GUIScreenView::handleMouseDown(int mouseX, int mouseY) {
    GUIView::handleMouseDown(mouseX-width, height-mouseY);
}

void GUIScreenView::handleMouseUp(int mouseX, int mouseY) {
    GUIView::handleMouseUp(mouseX-width, height-mouseY);
}

void GUIScreenView::handleMouseMove(int mouseX, int mouseY) {
    GUIView::handleMouseMove(mouseX-width, height-mouseY);
}


GUIScreenView* currentScreenView;