//
//  GUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButton.h"

#define cornerRadius 12
#define borderWidth 1
#define shadowWidth 6

void GUIButton::setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    pixel[0] = 100;
    pixel[1] = pixel[0];
    pixel[2] = pixel[0];
    pixel[3] = 255;
}

void GUIButton::setInnerShadowRect(unsigned char* pixels, unsigned int minX, unsigned int maxX, unsigned int minY, unsigned int maxY) {
    float value;
    int x0, x1, y0, y1;
    unsigned char* pixel;
    for(unsigned int y = minY; y < maxY; y ++)
        for(unsigned int x = minX; x < maxX; x ++) {
            value = 0.0;
            x0 = max(0, (int)x-shadowWidth);
            y0 = max(0, (int)y-shadowWidth);
            x1 = min(width*2-1, (int)x+shadowWidth);
            y1 = min(height*2-1, (int)y+shadowWidth);
            for(unsigned int y2 = y0; y2 <= y1; y2 ++)
                for(unsigned int x2 = x0; x2 <= x1; x2 ++)
                    value += (float)pixels[(y2*width*8)+x2*4+3]/255.0;
            pixel = pixels+(y*width*8)+x*4;
            value /= 4.0*shadowWidth*shadowWidth+4.0*shadowWidth+1.0;
            pixel[0] *= value;
            pixel[1] *= value;
            pixel[2] *= value;
        }
}

GUIButton::GUIButton() {
    type = GUITypeButton;
    texture = 0;
    clicked = NULL;
    paddingX = 10;
    paddingY = 5;
    autoSize = true;
    buttonType = GUIButtonTypeNormal;
    state = GUIButtonStateNormal;
    roundedCorners = (GUICorners) (GUITopLeftCorner | GUITopRightCorner | GUIBottomLeftCorner | GUIBottomRightCorner);
}

void GUIButton::updateContent() {
    if(autoSize) {
        width = borderWidth+cornerRadius+paddingX;
        height = borderWidth+cornerRadius+paddingY;
    }
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        if(children[i]->type == GUITypeLabel) {
            GUILabel* label = (GUILabel*)children[i];
            label->font = titleFont;
            switch(state) {
                case GUIButtonStateDisabled:
                    label->color.r = label->color.g = label->color.b = 60;
                break;
                case GUIButtonStateNormal:
                case GUIButtonStateHighlighted:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                        case GUIButtonTypeRadio:
                            label->color.r = label->color.g = label->color.b = 60;
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                            label->color.r = label->color.g = label->color.b = 240;
                        break;
                    }
                break;
                case GUIButtonStatePressed:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                            label->color.r = 30;
                            label->color.g = 110;
                            label->color.b = 200;
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                        case GUIButtonTypeRadio:
                            label->color.r = label->color.g = label->color.b = 240;
                        break;
                    }
                break;
            }
        }
        children[i]->updateContent();
        
        if(!autoSize) continue;
        if(children[i]->posX+children[i]->width+paddingX > width)
            width = children[i]->posX+children[i]->width+paddingX;
        if(children[i]->posY+children[i]->height+paddingY > height)
            height = children[i]->posY+children[i]->height+paddingY;
        if(children[i]->posX-children[i]->width-paddingX < -width)
            width = children[i]->width-children[i]->posX+paddingX;
        if(children[i]->posY-children[i]->height-paddingY < -height)
            height = children[i]->height-children[i]->posY+paddingY;
    }
    
    if(texture > 0)
        glDeleteTextures(1, &texture);
    
    unsigned char pixels[width*height*16];
    unsigned char* pixel;
    for(unsigned int y = 0; y < height*2; y ++)
        for(unsigned int x = 0; x < width*2; x ++) {
            pixel = pixels+(y*width*8)+x*4;
            switch(state) {
                case GUIButtonStateDisabled:
                    pixel[0] = 200-50.0*y/height;
                    pixel[1] = pixel[0];
                    pixel[2] = pixel[0];
                break;
                case GUIButtonStateNormal:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                        case GUIButtonTypeRadio:
                            pixel[0] = 240-30.0*y/height;
                            pixel[1] = pixel[0];
                            pixel[2] = pixel[0];
                            break;
                        case GUIButtonTypeDelete:
                            pixel[0] = 240-40.0*y/height;
                            pixel[1] = 140-70.0*y/height;
                            pixel[2] = pixel[1];
                        break;
                        case GUIButtonTypeAdd:
                            pixel[0] = 140-70.0*y/height;
                            pixel[1] = 240-50.0*y/height;
                            pixel[2] = pixel[0];
                        break;
                        case GUIButtonTypeEdit:
                            pixel[0] = 140-70.0*y/height;
                            pixel[1] = 200-70.0*y/height;
                            pixel[2] = 240-40.0*y/height;
                        break;
                    }
                break;
                case GUIButtonStateHighlighted:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                        case GUIButtonTypeRadio:
                            pixel[0] = 250-20.0*y/height;
                            pixel[1] = pixel[0];
                            pixel[2] = pixel[0];
                        break;
                        case GUIButtonTypeDelete:
                            pixel[0] = 250-30.0*y/height;
                            pixel[1] = 150-60.0*y/height;
                            pixel[2] = pixel[1];
                        break;
                        case GUIButtonTypeAdd:
                            pixel[0] = 140-60.0*y/height;
                            pixel[1] = 240-40.0*y/height;
                            pixel[2] = pixel[0];
                        break;
                        case GUIButtonTypeEdit:
                            pixel[0] = 160-70.0*y/height;
                            pixel[1] = 220-70.0*y/height;
                            pixel[2] = 240-30.0*y/height;
                        break;
                    }
                break;
                case GUIButtonStatePressed:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                            pixel[0] = 180+30.0*y/height;
                            pixel[1] = pixel[0];
                            pixel[2] = pixel[0];
                        break;
                        case GUIButtonTypeDelete:
                            pixel[0] = 160+40.0*y/height;
                            pixel[1] = 70.0*y/height;
                            pixel[2] = pixel[1];
                        break;
                        case GUIButtonTypeAdd:
                            pixel[0] = 70.0*y/height;
                            pixel[1] = 140+50.0*y/height;
                            pixel[2] = pixel[0];
                        break;
                        case GUIButtonTypeEdit:
                        case GUIButtonTypeRadio:
                            pixel[0] = 70.0*y/height;
                            pixel[1] = 60+70.0*y/height;
                            pixel[2] = 160+40.0*y/height;
                        break;
                    }
                break;
            }
            pixel[3] = 255;
            if((roundedCorners & GUITopLeftCorner) && x < cornerRadius && y < cornerRadius) {
                if(cornerRadius-x-cos(asin((float)(cornerRadius-y)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUITopRightCorner) && x >= width*2-cornerRadius && y < cornerRadius) {
                if(x-width*2+cornerRadius+1-cos(asin((float)(cornerRadius-y)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUIBottomLeftCorner) && x < cornerRadius && y >= height*2-cornerRadius) {
                if(cornerRadius-x-cos(asin((float)(y-height*2+cornerRadius+1)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUIBottomRightCorner) && x >= width*2-cornerRadius && y >= height*2-cornerRadius) {
                if(x-width*2+cornerRadius+1-cos(asin((float)(y-height*2+cornerRadius+1)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }
        }
    
    if(state >= GUIButtonStatePressed) {
        setInnerShadowRect(pixels, cornerRadius, width*2-cornerRadius, 0, cornerRadius);
        setInnerShadowRect(pixels, cornerRadius, width*2-cornerRadius, height*2-cornerRadius, height*2);
        setInnerShadowRect(pixels, 0, cornerRadius, 0, height*2);
        setInnerShadowRect(pixels, width*2-cornerRadius, width*2, 0, height*2);
    }
    
    int xMinA = (roundedCorners&GUITopLeftCorner)?cornerRadius:0,
        xMaxA = width*2-((roundedCorners&GUITopRightCorner)?cornerRadius:0),
        xMinB = (roundedCorners&GUIBottomLeftCorner)?cornerRadius:0,
        xMaxB = width*2-((roundedCorners&GUIBottomRightCorner)?cornerRadius:0),
        yMinA = (roundedCorners&GUITopLeftCorner)?cornerRadius:0,
        yMaxA = height*2-((roundedCorners&GUIBottomLeftCorner)?cornerRadius:0),
        yMinB = (roundedCorners&GUITopRightCorner)?cornerRadius:0,
        yMaxB = height*2-((roundedCorners&GUIBottomRightCorner)?cornerRadius:0);
    
    for(unsigned int x = xMinA; x < xMaxA; x ++) setBorderPixel(pixels, x, 0);
    for(unsigned int x = xMinB; x < xMaxB; x ++) setBorderPixel(pixels, x, height*2-1);
    for(unsigned int y = yMinA; y < yMaxA; y ++) setBorderPixel(pixels, 0, y);
    for(unsigned int y = yMinB; y < yMaxB; y ++) setBorderPixel(pixels, width*2-1, y);
    
    int error, x, y;
    for(unsigned int radius = cornerRadius-1; radius < cornerRadius+1; radius ++) {
        error = -radius;
        x = radius;
        y = 0;
        while(x >= y) {
            if(roundedCorners & GUITopLeftCorner) {
                setBorderPixel(pixels, radius-x, radius-y);
                setBorderPixel(pixels, radius-y, radius-x);
            }
            if(roundedCorners & GUITopRightCorner) {
                setBorderPixel(pixels, x+width*2-radius-1, radius-y);
                setBorderPixel(pixels, y+width*2-radius-1, radius-x);
            }
            if(roundedCorners & GUIBottomLeftCorner) {
                setBorderPixel(pixels, radius-x, y+height*2-radius-1);
                setBorderPixel(pixels, radius-y, x+height*2-radius-1);
            }
            if(roundedCorners & GUIBottomRightCorner) {
                setBorderPixel(pixels, x+width*2-radius-1, y+height*2-radius-1);
                setBorderPixel(pixels, y+width*2-radius-1, x+height*2-radius-1);
            }
            error += y ++;
            error += y;
            if(error >= 0) {
                error -= x --;
                error -= x;
            }
        }
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*2, height*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GUIButton::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    if(texture == 0) updateContent();
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    Vector3 minFactor(0.5+0.5*clipRect.minPosX/width, 0.5-0.5*clipRect.maxPosY/height, 0.0),
            maxFactor(0.5+0.5*clipRect.maxPosX/width, 0.5-0.5*clipRect.minPosY/height, 0.0);
    
    float vertices[] = {
        clipRect.maxPosX, clipRect.minPosY,
        maxFactor.x, maxFactor.y,
        clipRect.maxPosX, clipRect.maxPosY,
        maxFactor.x, minFactor.y,
        clipRect.minPosX, clipRect.maxPosY,
        minFactor.x, minFactor.y,
        clipRect.minPosX, clipRect.minPosY,
        minFactor.x, maxFactor.y
    };
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
    spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(modelMat, &clipRect);
}

bool GUIButton::handleMouseDown(int mouseX, int mouseY) {
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height || state == GUIButtonStateDisabled) return false;
    state = GUIButtonStatePressed;
    updateContent();
    return true;
}

void GUIButton::handleMouseUp(int mouseX, int mouseY) {
    if(state == GUIButtonStateDisabled) return;
    bool triggerEvent = false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height)
        state = GUIButtonStateNormal;
    else{
        if(state == GUIButtonStatePressed)
            triggerEvent = true;
        state = GUIButtonStateHighlighted;
    }
    updateContent();
    if(triggerEvent && clicked)
        clicked(this);
}

void GUIButton::handleMouseMove(int mouseX, int mouseY) {
    if(state == GUIButtonStateDisabled) return;
    GUIButtonState prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButtonStateNormal;
    }else if(state == GUIButtonStatePressed)
        return;
    else
        state = GUIButtonStateHighlighted;
    if(prevState != state) updateContent();
}