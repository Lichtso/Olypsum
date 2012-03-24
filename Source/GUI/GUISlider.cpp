//
//  GUISlider.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUISlider.h"

#define barHeight 6
#define sliderRadius 12

GUISlider::GUISlider() {
    type = GUIType_Silder;
    textureL = textureM = textureR = 0;
    mouseDragPos = -1;
    highlighted = false;
    value = 0.5;
    orientation = GUIOrientation_Horizontal;
    onChange = NULL;
    enabled = true;
}

void GUISlider::generateBar(bool filled) {
    GUIRoundedRect roundedRect;
    roundedRect.texture = (filled) ? &textureL : &textureR;
    roundedRect.width = ((orientation & GUIOrientation_Horizontal) ? width : height)-sliderRadius+barHeight;
    roundedRect.height = barHeight;
    roundedRect.cornerRadius = barHeight;
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(enabled) {
        if(filled) {
            roundedRect.topColor.r = 0;
            roundedRect.topColor.g = 60;
            roundedRect.topColor.b = 160;
            roundedRect.bottomColor.r = 140;
            roundedRect.bottomColor.g = 200;
            roundedRect.bottomColor.b = 240;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g =roundedRect.topColor.b = 180;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g =roundedRect.bottomColor.b = 255;
        }
    }else{
        if(filled) {
            roundedRect.topColor.r = 0;
            roundedRect.topColor.g = 30;
            roundedRect.topColor.b = 130;
            roundedRect.bottomColor.r = 90;
            roundedRect.bottomColor.g = 150;
            roundedRect.bottomColor.b = 190;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g =roundedRect.topColor.b = 120;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g =roundedRect.bottomColor.b = 180;
        }
    }
    roundedRect.drawInTexture();
}

void GUISlider::updateContent() {
    if(textureL > 0) {
        glDeleteTextures(1, &textureL);
        glDeleteTextures(1, &textureM);
        glDeleteTextures(1, &textureR);
    }
    
    glGenTextures(1, &textureL);
    glGenTextures(1, &textureM);
    glGenTextures(1, &textureR);
    
    if(orientation & GUIOrientation_Horizontal) {
        if(width < sliderRadius) width = sliderRadius;
        height = sliderRadius;
    }else{
        width = sliderRadius;
        if(height < sliderRadius) height = sliderRadius;
    }
    
    generateBar(true);
    generateBar(false);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &textureM;
    roundedRect.width = sliderRadius;
    roundedRect.height = sliderRadius;
    roundedRect.cornerRadius = sliderRadius;
    roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 160;
    if(enabled) {
        if(highlighted) {
            roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 250;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 210;
        }else{
            roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 240;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 180;
        }
    }else{
        roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 150;
        roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 110;
    }
    roundedRect.drawInTexture();
}

void GUISlider::drawBar(GUIClipRect* clipRect, unsigned int barLength, bool filled) {
    GUIClipRect clipRectB;
    int silderPos = (int)(barLength*value);
    Vector3 minFactor, maxFactor;
    barLength += barHeight*2;
    
    if(orientation & GUIOrientation_Horizontal) {
        if(filled) {
            clipRectB.minPosX = max(clipRect->minPosX, sliderRadius-width-barHeight);
            clipRectB.maxPosX = min(clipRect->maxPosX, sliderRadius-width+silderPos);
        }else{
            clipRectB.minPosX = max(clipRect->minPosX, sliderRadius-width+silderPos);
            clipRectB.maxPosX = min(clipRect->maxPosX, width-sliderRadius+barHeight);
        }
        clipRectB.minPosY = max(clipRect->minPosY, -barHeight);
        clipRectB.maxPosY = min(clipRect->maxPosY, barHeight);
        minFactor = Vector3(0.5+(float)clipRectB.minPosX/barLength, 0.5-0.5*clipRectB.maxPosY/barHeight, 0.0);
        maxFactor = Vector3(0.5+(float)clipRectB.maxPosX/barLength, 0.5-0.5*clipRectB.minPosY/barHeight, 0.0);
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
        clipRectB.minPosX = max(clipRect->minPosX, -barHeight);
        clipRectB.maxPosX = min(clipRect->maxPosX, barHeight);
        if(filled) {
            clipRectB.minPosY = max(clipRect->minPosY, sliderRadius-height-barHeight);
            clipRectB.maxPosY = min(clipRect->maxPosY, sliderRadius-height+silderPos);
        }else{
            clipRectB.minPosY = max(clipRect->minPosY, sliderRadius-height+silderPos);
            clipRectB.maxPosY = min(clipRect->maxPosY, height-sliderRadius+barHeight);
        }
        minFactor = Vector3(0.5-(float)clipRectB.maxPosY/barLength, 0.5+0.5*clipRectB.maxPosX/barHeight, 0.0);
        maxFactor = Vector3(0.5-(float)clipRectB.minPosY/barLength, 0.5+0.5*clipRectB.minPosX/barHeight, 0.0);
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

void GUISlider::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    if(!textureL) updateContent();
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    
    GUIClipRect clipRectB;
    int barLength, silderPos;
    Vector3 minFactor, maxFactor;
    if(orientation & GUIOrientation_Horizontal) {
        barLength = width*2-sliderRadius*2;
        silderPos = barLength*value;
        clipRectB.minPosX = max(clipRect.minPosX, silderPos-width);
        clipRectB.maxPosX = min(clipRect.maxPosX, silderPos-width+sliderRadius*2);
        clipRectB.minPosY = max(clipRect.minPosY, -sliderRadius);
        clipRectB.maxPosY = min(clipRect.maxPosY, sliderRadius);
        minFactor = Vector3(0.5*(clipRectB.minPosX-silderPos+width)/sliderRadius, 0.5-0.5*clipRectB.maxPosY/sliderRadius, 0.0),
        maxFactor = Vector3(0.5*(clipRectB.maxPosX-silderPos+width)/sliderRadius, 0.5-0.5*clipRectB.minPosY/sliderRadius, 0.0);
    }else{
        barLength = height*2-sliderRadius*2;
        silderPos = barLength*value;
        clipRectB.minPosX = max(clipRect.minPosX, -sliderRadius);
        clipRectB.maxPosX = min(clipRect.maxPosX, sliderRadius);
        clipRectB.minPosY = max(clipRect.minPosY, silderPos-height);
        clipRectB.maxPosY = min(clipRect.maxPosY, silderPos-height+sliderRadius*2);
        minFactor = Vector3(0.5+0.5*clipRectB.minPosX/sliderRadius, 1.0-0.5*(clipRectB.maxPosY-silderPos+height)/sliderRadius, 0.0),
        maxFactor = Vector3(0.5+0.5*clipRectB.maxPosX/sliderRadius, 1.0-0.5*(clipRectB.minPosY-silderPos+height)/sliderRadius, 0.0);
    }
    drawBar(&clipRect, barLength, true);
    drawBar(&clipRect, barLength, false);
    
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
    glBindTexture(GL_TEXTURE_2D, textureM);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

bool GUISlider::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled) return false;
    highlighted = false;
    
    if(orientation & GUIOrientation_Horizontal) {
        mouseX -= (width-sliderRadius)*(2.0*value-1.0);
        if(mouseX < -sliderRadius || mouseX > sliderRadius || mouseY < -height || mouseY > height) return false;
        mouseDragPos = mouseX;
    }else{
        mouseY -= (height-sliderRadius)*(2.0*value-1.0);
        if(mouseX < -width || mouseX > width || mouseY < -sliderRadius || mouseY > sliderRadius) return false;
        mouseDragPos = mouseY;
    }
    
    highlighted = true;
    return true;
}

void GUISlider::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPos = -1;
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
    
    if(orientation & GUIOrientation_Horizontal)
        value = 0.5+0.5*(mouseX-mouseDragPos)/(width-sliderRadius);
    else
        value = 0.5+0.5*(mouseY-mouseDragPos)/(height-sliderRadius);
    
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    
    if(onChange)
        onChange(this);
}

bool GUISlider::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    value -= delta*0.05;
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    
    return true;
}