//
//  GUIRoundedRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "../Menu/Menu.h"

void GUIDrawableRect::drawOnScreen(const btVector3& parentTransform, int posX, int posY, GUIClipRect &parentClipRect) {
    GUIClipRect clipRect;
    clipRect.minPosX = max(parentClipRect.minPosX, posX-width);
    clipRect.minPosY = max(parentClipRect.minPosY, posY-height);
    clipRect.maxPosX = min(parentClipRect.maxPosX, posX+width);
    clipRect.maxPosY = min(parentClipRect.maxPosY, posY+height);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    modelMat.setIdentity();
    btVector3 halfSize(0.5*(clipRect.maxPosX-clipRect.minPosX), 0.5*(clipRect.maxPosY-clipRect.minPosY), 1.0);
    modelMat.setBasis(modelMat.getBasis().scaled(halfSize));
    modelMat.setOrigin(btVector3(halfSize.x()+clipRect.minPosX, halfSize.y()+clipRect.minPosY, 0)+parentTransform);
    
    btTransform auxMat = btTransform::getIdentity();
    btVector3 minFactor(0.5+0.5*(clipRect.minPosX-posX)/width, 0.5-0.5*(clipRect.maxPosY-posY)/height, 0.0),
    maxFactor(0.5+0.5*(clipRect.maxPosX-posX)/width, 0.5-0.5*(clipRect.minPosY-posY)/height, 0.0);
    btVector3 halfCoords(0.5*(maxFactor.x()-minFactor.x()), -0.5*(maxFactor.y()-minFactor.y()), 1.0);
    auxMat.setBasis(auxMat.getBasis().scaled(halfCoords));
    auxMat.setOrigin(btVector3(minFactor.x()+halfCoords.x(), minFactor.y()-halfCoords.y(), 0));
    
    shaderPrograms[spriteSP]->use();
    currentShaderProgram->setUniformMatrix3("textureMat", &auxMat);
    rectVAO.draw();
}



GUIRoundedRect::GUIRoundedRect() :texture(0), transposed(false), decorationType(GUIDecorationType::Monochrome) {
    roundedCorners = (GUICorner) (GUICorner::TopLeft | GUICorner::TopRight | GUICorner::BottomLeft | GUICorner::BottomRight);
    cornerRadius = menu.screenView->width*0.02;
    width = height = 100;
    topColor = Color4(0.9);
    bottomColor = Color4(1.0);
    borderColor = Color4(0.5);
    edgeGradientCenter = edgeGradientBorder = 1.0;
}

GUIRoundedRect::~GUIRoundedRect() {
    if(texture)
        glDeleteTextures(1, &texture);
}

void GUIRoundedRect::updateContent() {
    if(!texture)
        glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*2, height*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glViewport(0, 0, width*2, height*2);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO.frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    shaderPrograms[genRoundedRectSP+decorationType]->use();
    currentShaderProgram->setUniformVec2("size", width*2, height*2);
    currentShaderProgram->setUniformVec2("edgeGradient", edgeGradientCenter, edgeGradientBorder);
    currentShaderProgram->setUniformF("cornerRadius", cornerRadius);
    currentShaderProgram->setUniformF("seed", (float)(reinterpret_cast<long int>(this)&0xFFFF));
    currentShaderProgram->setUniformI("transposed", transposed);
    currentShaderProgram->setUniformI("roundedCorners[0]", roundedCorners & GUICorner::TopLeft);
    currentShaderProgram->setUniformI("roundedCorners[1]", roundedCorners & GUICorner::TopRight);
    currentShaderProgram->setUniformI("roundedCorners[2]", roundedCorners & GUICorner::BottomLeft);
    currentShaderProgram->setUniformI("roundedCorners[3]", roundedCorners & GUICorner::BottomRight);
    currentShaderProgram->setUniformVec4("topColor", topColor.getQuaternion());
    currentShaderProgram->setUniformVec4("bottomColor", bottomColor.getQuaternion());
    currentShaderProgram->setUniformVec4("borderColor", borderColor.getQuaternion());
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    rectVAO.draw();
    glEnable(GL_DEPTH_TEST);
}

void GUIRoundedRect::drawOnScreen(const btVector3& parentTransform, int posX, int posY, GUIClipRect &parentClipRect) {
    if(!texture) updateContent();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GUIDrawableRect::drawOnScreen(parentTransform, posX, posY, parentClipRect);
}