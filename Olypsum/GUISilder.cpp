//
//  GUISilder.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUISilder.h"

#define barHeight 10
#define sliderRadius 11

GUISilder::GUISilder() {
    type = GUITypeSilder;
    textureL = textureM = textureR = 0;
    mouseDragPos = -1;
    highlighted = false;
    value = 0.5;
    orientation = GUIOrientationHorizontal;
    onChange = NULL;
    enabled = true;
}

void GUISilder::setBarBorderPixel(unsigned char* pixels, unsigned int barLength, unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*barLength*4)+x*4;
    pixel[0] = 150;
    pixel[1] = pixel[0];
    pixel[2] = pixel[0];
    pixel[3] = 255;
}

void GUISilder::generateBar(bool filled) {
    unsigned int barLength = ((orientation & GUIOrientationHorizontal) ? width*2 : height*2)-sliderRadius*2+barHeight;
    unsigned char pixels[barLength*barHeight*4], *pixel;
    
    int diffX, diffY, borderRadius = barHeight>>1;
    for(unsigned int y = 0; y < barHeight; y ++)
        for(unsigned int x = 0; x < barLength; x ++) {
            pixel = pixels+(y*barLength*4)+x*4;
            if(enabled) {
                if(filled) {
                    pixel[0] = 140.0*y/barHeight;
                    pixel[1] = 60+140.0*y/barHeight;
                    pixel[2] = 160+80.0*y/barHeight;
                }else{
                    pixel[0] = 180+60.0*y/barHeight;
                    pixel[1] = pixel[0];
                    pixel[2] = pixel[0];
                }
            }else{
                if(filled) {
                    pixel[0] = 90.0*y/barHeight;
                    pixel[1] = 30+120.0*y/barHeight;
                    pixel[2] = 130+60.0*y/barHeight;
                }else{
                    pixel[0] = 120+60.0*y/barHeight;
                    pixel[1] = pixel[0];
                    pixel[2] = pixel[0];
                }
            }
            
            if(x < borderRadius) {
                diffX = borderRadius-x;
                diffY = y-borderRadius;
                pixel[3] = (sqrt(diffX*diffX+diffY*diffY) < borderRadius) ? 255 : 0;
            }else if(x >= barLength-borderRadius) {
                diffX = barLength-borderRadius-x;
                diffY = y-borderRadius;
                pixel[3] = (sqrt(diffX*diffX+diffY*diffY) < borderRadius) ? 255 : 0;
            }else
                pixel[3] = 255;
        }
    
    for(unsigned int x = borderRadius; x < barLength-borderRadius; x ++) {
        pixel = pixels+x*4;
        pixel[0] = 150;
        pixel[1] = pixel[0];
        pixel[2] = pixel[0];
        pixel = pixels+((barHeight-1)*barLength*4)+x*4;
        pixel[0] = 150;
        pixel[1] = pixel[0];
        pixel[2] = pixel[0];
    }
    
    int error = -borderRadius, x = borderRadius, y = 0;
    while(x >= y) {
        setBarBorderPixel(pixels, barLength, borderRadius-x, borderRadius-y);
        setBarBorderPixel(pixels, barLength, borderRadius-y, borderRadius-x);
        setBarBorderPixel(pixels, barLength, borderRadius-x, borderRadius+y-1);
        setBarBorderPixel(pixels, barLength, borderRadius-y, borderRadius+x-1);
        setBarBorderPixel(pixels, barLength, barLength-borderRadius+x-1, borderRadius-y);
        setBarBorderPixel(pixels, barLength, barLength-borderRadius+y-1, borderRadius-x);
        setBarBorderPixel(pixels, barLength, barLength-borderRadius+x-1, borderRadius+y-1);
        setBarBorderPixel(pixels, barLength, barLength-borderRadius+y-1, borderRadius+x-1);
        
        error += y ++;
        error += y;
        if(error >= 0) {
            error -= x --;
            error -= x;
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, (filled) ? textureL : textureR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, barLength, barHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GUISilder::setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*sliderRadius*8)+x*4;
    pixel[0] = 100;
    pixel[1] = pixel[0];
    pixel[2] = pixel[0];
    pixel[3] = 255;
}

void GUISilder::updateContent() {
    if(textureL > 0) {
        glDeleteTextures(1, &textureL);
        glDeleteTextures(1, &textureM);
        glDeleteTextures(1, &textureR);
    }
    
    glGenTextures(1, &textureL);
    glGenTextures(1, &textureM);
    glGenTextures(1, &textureR);
    
    if(orientation & GUIOrientationHorizontal) {
        if(width < sliderRadius) width = sliderRadius;
        height = sliderRadius;
    }else{
        width = sliderRadius;
        if(height < sliderRadius) height = sliderRadius;
    }
    
    generateBar(true);
    generateBar(false);
    
    unsigned char pixels[sliderRadius*sliderRadius*16], *pixel;
    int diffX, diffY;
    for(int y = 0; y < sliderRadius*2; y ++)
        for(int x = 0; x < sliderRadius*2; x ++) {
            pixel = pixels+(y*sliderRadius*8)+x*4;
            pixel[0] = (enabled) ? ((highlighted) ? 250-20.0*y/sliderRadius : 240-30.0*y/sliderRadius) : 150-20.0*y/sliderRadius;
            pixel[1] = pixel[0];
            pixel[2] = pixel[0];
            diffX = x-sliderRadius;
            diffY = y-sliderRadius;
            pixel[3] = (sqrt(diffX*diffX+diffY*diffY) < sliderRadius-0.3) ? 255 : 0;
        }
    
    int error = -sliderRadius, x = sliderRadius, y = 0;
    while(x >= y) {
        setBorderPixel(pixels, sliderRadius-x, sliderRadius-y);
        setBorderPixel(pixels, sliderRadius-y, sliderRadius-x);
        setBorderPixel(pixels, sliderRadius-x, sliderRadius+y-1);
        setBorderPixel(pixels, sliderRadius-y, sliderRadius+x-1);
        setBorderPixel(pixels, sliderRadius+x-1, sliderRadius-y);
        setBorderPixel(pixels, sliderRadius+y-1, sliderRadius-x);
        setBorderPixel(pixels, sliderRadius+x-1, sliderRadius+y-1);
        setBorderPixel(pixels, sliderRadius+y-1, sliderRadius+x-1);
        
        error += y ++;
        error += y;
        if(error >= 0) {
            error -= x --;
            error -= x;
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, textureM);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sliderRadius*2, sliderRadius*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GUISilder::drawBar(GUIClipRect* clipRect, unsigned int barLength, bool filled) {
    GUIClipRect clipRectB;
    int silderPos = (int)(barLength*value);
    Vector3 minFactor, maxFactor;
    barLength += barHeight;
    if(orientation & GUIOrientationHorizontal) {
        if(filled) {
            clipRectB.minPosX = max(clipRect->minPosX, sliderRadius-width-(barHeight>>1));
            clipRectB.maxPosX = min(clipRect->maxPosX, sliderRadius-width+silderPos);
        }else{
            clipRectB.minPosX = max(clipRect->minPosX, sliderRadius-width+silderPos);
            clipRectB.maxPosX = min(clipRect->maxPosX, width-sliderRadius+(barHeight>>1));
        }
        clipRectB.minPosY = max(clipRect->minPosY, -(barHeight>>1));
        clipRectB.maxPosY = min(clipRect->maxPosY, barHeight>>1);
        minFactor = Vector3(0.5+(float)clipRectB.minPosX/barLength, 0.5-(float)clipRectB.maxPosY/barHeight, 0.0);
        maxFactor = Vector3(0.5+(float)clipRectB.maxPosX/barLength, 0.5-(float)clipRectB.minPosY/barHeight, 0.0);
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
        clipRectB.minPosX = max(clipRect->minPosX, -(barHeight>>1));
        clipRectB.maxPosX = min(clipRect->maxPosX, barHeight>>1);
        if(filled) {
            clipRectB.minPosY = max(clipRect->minPosY, sliderRadius-height-(barHeight>>1));
            clipRectB.maxPosY = min(clipRect->maxPosY, sliderRadius-height+silderPos);
        }else{
            clipRectB.minPosY = max(clipRect->minPosY, sliderRadius-height+silderPos);
            clipRectB.maxPosY = min(clipRect->maxPosY, height-sliderRadius+(barHeight>>1));
        }
        minFactor = Vector3(0.5-(float)clipRectB.maxPosY/barLength, 0.5+(float)clipRectB.maxPosX/barHeight, 0.0);
        maxFactor = Vector3(0.5-(float)clipRectB.minPosY/barLength, 0.5+(float)clipRectB.minPosX/barHeight, 0.0);
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

void GUISilder::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    if(textureL == 0)
        updateContent();
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    
    GUIClipRect clipRectB;
    int barLength, silderPos;
    Vector3 minFactor, maxFactor;
    if(orientation & GUIOrientationHorizontal) {
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

bool GUISilder::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled) return false;
    highlighted = false;
    
    if(orientation & GUIOrientationHorizontal) {
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

void GUISilder::handleMouseUp(int mouseX, int mouseY) {
    mouseDragPos = -1;
}

void GUISilder::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled) return;
    
    if(mouseDragPos == -1) {
        bool prevHighlighted = highlighted;
        
        if(handleMouseDown(mouseX, mouseY))
            mouseDragPos = -1;
        
        if(prevHighlighted != highlighted)
            updateContent();
        
        return;
    }
    
    if(orientation & GUIOrientationHorizontal)
        value = 0.5+0.5*(mouseX-mouseDragPos)/(width-sliderRadius);
    else
        value = 0.5+0.5*(mouseY-mouseDragPos)/(height-sliderRadius);
    
    if(value < 0.0) value = 0.0;
    else if(value > 1.0) value = 1.0;
    
    if(onChange)
        onChange(this);
}