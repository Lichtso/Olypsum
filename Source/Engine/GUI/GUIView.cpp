//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Cam.h"
#import "GUIView.h"

GUIView::GUIView() {
    type = GUIType_View;
}

GUIView::~GUIView() {
    for(unsigned int i = 0; i < children.size(); i ++)
        delete children[i];
}

void GUIView::addChild(GUIRect* child) {
    child->parent = this;
    children.push_back(child);
}

void GUIView::removeChild(unsigned int index) {
    delete children[index];
    children.erase(children.begin()+index);
}

void GUIView::updateContent() {
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIView::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}

bool GUIView::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY))
            return true;
    return false;
}

bool GUIView::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY))
            return true;
    return false;
}

void GUIView::handleMouseMove(int mouseX, int mouseY) {
    if(!visible) return;
    for(int i = (int)children.size()-1; i >= 0; i --)
        children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

bool GUIView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, delta))
            return true;
    return false;
}



GUIFramedView::GUIFramedView() {
    type = GUIType_View;
    texture = 0;
    innerShadow = -8;
}

GUIFramedView::~GUIFramedView() {
    if(texture)
        glDeleteTextures(1, &texture);
}

void GUIFramedView::updateContent() {
    if(innerShadow != 0) {
        GUIRoundedRect roundedRect;
        roundedRect.texture = &texture;
        roundedRect.width = width;
        roundedRect.height = height;
        roundedRect.innerShadow = innerShadow*screenSize[2];
        roundedRect.cornerRadius = abs(innerShadow)*screenSize[2];
        roundedRect.topColor = Color4(0.78);
        roundedRect.bottomColor = Color4(0.78);
        roundedRect.borderColor = Color4(0.78);
        roundedRect.drawInTexture();
    }
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIFramedView::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    
    if(innerShadow != 0) {
        if(!texture) updateContent();
        modelMat.setIdentity();
        modelMat.setOrigin(transform);
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
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}



GUIScreenView::GUIScreenView() {
    type = GUIType_ScreenView;
    modalView = NULL;
    firstResponder = NULL;
    width = screenSize[0] >> 1;
    height = screenSize[1] >> 1;
}

bool GUIScreenView::getLimSize(GUIClipRect& clipRect) {
    width = screenSize[0] >> 1;
    height = screenSize[1] >> 1;
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    return (clipRect.minPosX <= clipRect.maxPosX && clipRect.minPosY <= clipRect.maxPosY);
}

void GUIScreenView::updateContent() {
    width = screenSize[0] >> 1;
    height = screenSize[1] >> 1;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::draw() {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect)) return;
    
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    shaderPrograms[spriteSP]->use();
    shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
    
    btVector3 transform = btVector3(0.0, 0.0, -1.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
    
    if(modalView) {
        modelMat.setIdentity();
        modelMat.setOrigin(transform);
        shaderPrograms[spriteSP]->use();
        shaderPrograms[spriteSP]->setUniformF("alpha", 0.8);
        
        float vertices[] = {
            (float)width, (float)-height,
            1.0, 0.0, 0.2,
            (float)width, (float)height,
            1.0, 1.0, 0.2,
            (float)-width, (float)height,
            0.0, 1.0, 0.2,
            (float)-width, (float)-height,
            0.0, 0.0, 0.2
        };
        
        shaderPrograms[spriteSP]->setAttribute(POSITION_ATTRIBUTE, 2, 5*sizeof(float), vertices);
        shaderPrograms[spriteSP]->setAttribute(TEXTURE_COORD_ATTRIBUTE, 3, 5*sizeof(float), &vertices[2]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDrawArrays(GL_QUADS, 0, 4);
        
        shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
        modalView->draw(transform, clipRect);
    }
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glEnable(GL_DEPTH_TEST);
}

bool GUIScreenView::handleMouseDown(int mouseX, int mouseY) {
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    if(modalView)
        return modalView->handleMouseDown(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY))
                return true;
    return false;
}

bool GUIScreenView::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return false;
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(modalView)
        return modalView->handleMouseUp(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY))
                return true;
    return false;
}

void GUIScreenView::handleMouseMove(int mouseX, int mouseY) {
    if(!visible) return;
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(modalView)
        return modalView->handleMouseMove(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

bool GUIScreenView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    if(modalView)
        return modalView->handleMouseWheel(mouseX-modalView->posX, mouseY-modalView->posY, delta);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, delta))
                return true;
    return false;
}

bool GUIScreenView::handleKeyDown(SDL_keysym* key) {
    if(!firstResponder) return false;
    return firstResponder->handleKeyDown(key);
}

bool GUIScreenView::handleKeyUp(SDL_keysym* key) {
    if(!firstResponder) return false;
    return firstResponder->handleKeyUp(key);
}

void GUIScreenView::setModalView(GUIRect* modalViewB) {
    if(firstResponder) {
        firstResponder->removeFirstResponderStatus();
        firstResponder = NULL;
    }
    modalView = modalViewB;
    if(modalView) modalView->parent = this;
}

GUIScreenView* currentScreenView = NULL;