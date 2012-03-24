//
//  GUIProgressBar.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIProgressBar.h"

GUIProgressBar::GUIProgressBar() {
    type = GUIType_ProgressBar;
    textureL = textureR = 0;
    value = 0.5;
    orientation = GUIOrientation_Horizontal;
    width = 200;
    height = 10;
}

void GUIProgressBar::generateBar(bool filled) {
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.cornerRadius = 6;
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(filled) {
        roundedRect.topColor.r = 100;
        roundedRect.topColor.g = 250;
        roundedRect.topColor.b = 190;
        roundedRect.bottomColor.r = 10;
        roundedRect.bottomColor.g = 150;
        roundedRect.bottomColor.b = 50;
    }else{
        roundedRect.topColor.r = roundedRect.topColor.g =roundedRect.topColor.b = 180;
        roundedRect.bottomColor.r = roundedRect.bottomColor.g =roundedRect.bottomColor.b = 255;
    }
    roundedRect.drawInTexture();
}

void GUIProgressBar::updateContent() {
    if(textureL > 0) {
        glDeleteTextures(1, &textureL);
        glDeleteTextures(1, &textureR);
    }
    
    glGenTextures(1, &textureL);
    glGenTextures(1, &textureR);
    
    generateBar(true);
    generateBar(false);
}

void GUIProgressBar::drawBar(GUIClipRect* clipRect, unsigned int barLength, bool filled) {
    GUIClipRect clipRectB;
    int splitPos = (int)(barLength*value);
    Vector3 minFactor, maxFactor;
    
    if(orientation & GUIOrientation_Horizontal) {
        if(filled) {
            clipRectB.minPosX = max(clipRect->minPosX, -width);
            clipRectB.maxPosX = min(clipRect->maxPosX, -width+splitPos);
        }else{
            clipRectB.minPosX = max(clipRect->minPosX, -width+splitPos);
            clipRectB.maxPosX = min(clipRect->maxPosX, width);
        }
        clipRectB.minPosY = max(clipRect->minPosY, -height);
        clipRectB.maxPosY = min(clipRect->maxPosY, height);
        minFactor = Vector3(0.5+(float)clipRectB.minPosX/barLength, 0.5-0.5*clipRectB.maxPosY/height, 0.0);
        maxFactor = Vector3(0.5+(float)clipRectB.maxPosX/barLength, 0.5-0.5*clipRectB.minPosY/height, 0.0);
        if(clipRectB.minPosX > clipRectB.maxPosX || clipRectB.minPosY > clipRectB.maxPosY) return;
        float vertices[] = {
            clipRectB.maxPosX, clipRectB.minPosY,
            maxFactor.x, maxFactor.y,
            clipRectB.maxPosX, clipRectB.maxPosY,
            maxFactor.x, minFactor.y,
            clipRectB.minPosX, clipRectB.maxPosY,
            minFactor.x, minFactor.y,
            clipRectB.minPosX, clipRectB.minPosY,
            minFactor.x, maxFactor.y
        };
        spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
        spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    }else{
        clipRectB.minPosX = max(clipRect->minPosX, -width);
        clipRectB.maxPosX = min(clipRect->maxPosX, width);
        if(filled) {
            clipRectB.minPosY = max(clipRect->minPosY, -height);
            clipRectB.maxPosY = min(clipRect->maxPosY, -height+splitPos);
        }else{
            clipRectB.minPosY = max(clipRect->minPosY, -height+splitPos);
            clipRectB.maxPosY = min(clipRect->maxPosY, height);
        }
        minFactor = Vector3(0.5-(float)clipRectB.maxPosY/barLength, 0.5+0.5*clipRectB.maxPosX/width, 0.0);
        maxFactor = Vector3(0.5-(float)clipRectB.minPosY/barLength, 0.5+0.5*clipRectB.minPosX/width, 0.0);
        if(clipRectB.minPosX > clipRectB.maxPosX || clipRectB.minPosY > clipRectB.maxPosY) return;
        float vertices[] = {
            clipRectB.maxPosX, clipRectB.minPosY,
            maxFactor.x, minFactor.y,
            clipRectB.maxPosX, clipRectB.maxPosY,
            minFactor.x, minFactor.y,
            clipRectB.minPosX, clipRectB.maxPosY,
            minFactor.x, maxFactor.y,
            clipRectB.minPosX, clipRectB.minPosY,
            maxFactor.x, maxFactor.y
        };
        spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
        spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    }
    glBindTexture(GL_TEXTURE_2D, (filled) ? textureL : textureR);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GUIProgressBar::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    
    int barLength = ((orientation & GUIOrientation_Horizontal) ? width*2 : height*2);
    drawBar(&clipRect, barLength, true);
    drawBar(&clipRect, barLength, false);
}