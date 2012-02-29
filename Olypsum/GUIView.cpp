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

void GUIView::handleMouseDown(Vector3 relativePos) {
    if(relativePos < Vector3(-width, -height, 0.0) || relativePos > Vector3(width, height, 0.0)) return;
    relativePos -= Vector3(posX, posY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseDown(relativePos-Vector3(children[i]->posX, children[i]->posY, 0.0));
}

void GUIView::handleMouseUp(Vector3 relativePos) {
    if(relativePos < Vector3(-width, -height, 0.0) || relativePos > Vector3(width, height, 0.0)) return;
    relativePos -= Vector3(posX, posY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseUp(relativePos-Vector3(children[i]->posX, children[i]->posY, 0.0));
}

void GUIView::handleMouseMove(Vector3 relativePos) {
    if(relativePos < Vector3(-width, -height, 0.0) || relativePos > Vector3(width, height, 0.0)) return;
    relativePos -= Vector3(posX, posY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->handleMouseMove(relativePos-Vector3(children[i]->posX, children[i]->posY, 0.0));
}

void GUIView::draw(Matrix4& parentTransform) {
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX, posY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform);
}



GUIScreenView::GUIScreenView() {
    parent = NULL;
    recalculateSize();
}

void GUIScreenView::recalculateSize() {
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
}

void GUIScreenView::getLimSize(GUIClipRect& clipRect) {
    recalculateSize();
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
}

void GUIScreenView::draw() {
    guiCam->use();
    spriteShaderProgram->setUnfiformF("light", 1.0);
    
    Matrix4 transform;
    transform.setIdentity();
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform);
    
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
}


GUIScreenView* currentScreenView;