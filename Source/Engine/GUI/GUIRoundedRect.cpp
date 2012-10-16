//
//  GUIRoundedRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIRoundedRect.h"

GUIRoundedRect::GUIRoundedRect() {
    texture = NULL;
    roundedCorners = (GUICorners) (GUITopLeftCorner | GUITopRightCorner | GUIBottomLeftCorner | GUIBottomRightCorner);
    cornerRadius = 8;
    innerShadow = 0;
    width = height = 0;
    topColor = Color4(0.9);
    bottomColor = Color4(1.0);
    borderColor = Color4(0.5);
}

void GUIRoundedRect::setBorderPixel(unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    pixel[0] = borderColor.r*255;
    pixel[1] = borderColor.g*255;
    pixel[2] = borderColor.b*255;
    pixel[3] = borderColor.a*255;
}

float GUIRoundedRect::getInnerShadowValue(unsigned int x, unsigned int y) {
    float value = 0.0;
    unsigned int border = abs(innerShadow);
    int x0 = max(0, (int)(x-border)),
        y0 = max(0, (int)(y-border)),
        x1 = min(width*2-1, (int)(x+border)),
        y1 = min(height*2-1, (int)(y+border));
    for(unsigned int y2 = y0; y2 <= y1; y2 ++)
        for(unsigned int x2 = x0; x2 <= x1; x2 ++)
            value += (float)pixels[(y2*width*8)+x2*4+3]/255.0;
    value /= 4.0*border*border+4.0*border+1.0;
    return (innerShadow > 0.0) ? value*0.8+0.2 : (1.0-value)*80.0;
}

void GUIRoundedRect::setInnerShadowPixel(unsigned int x, unsigned int y, float value) {
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    if(innerShadow > 0.0) {
        pixel[0] *= value;
        pixel[1] *= value;
        pixel[2] *= value;
    }else{
        pixel[0] += value;
        pixel[1] += value;
        pixel[2] += value;
    }
}

void GUIRoundedRect::drawInTexture() {
    if(*texture) glDeleteTextures(1, texture);
    
    pixels = new unsigned char[width*height*16];
    unsigned char* pixel;
    
    for(unsigned int y = 0; y < height*2; y ++)
        for(unsigned int x = 0; x < width*2; x ++) {
            pixel = pixels+(y*width*8)+x*4;
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
            if(pixel[3] == 0) continue;
            pixel[0] = ((bottomColor.r-topColor.r)*0.5*y/height+topColor.r)*255.0;
            pixel[1] = ((bottomColor.g-topColor.g)*0.5*y/height+topColor.g)*255.0;
            pixel[2] = ((bottomColor.b-topColor.b)*0.5*y/height+topColor.b)*255.0;
        }
    
    if(innerShadow != 0) {
        for(unsigned int y = 0; y < cornerRadius; y ++)
            for(unsigned int x = 0; x < cornerRadius; x ++) {
                unsigned int mirX = width*2-1-x, mirY = height*2-1-y;
                setInnerShadowPixel(x, y, getInnerShadowValue(x, y));
                setInnerShadowPixel(mirX, y, getInnerShadowValue(mirX, y));
                setInnerShadowPixel(x, mirY, getInnerShadowValue(x, mirY));
                setInnerShadowPixel(mirX, mirY, getInnerShadowValue(mirX, mirY));
            }
        
        for(unsigned int y = 0; y < cornerRadius; y ++) {
            float value = getInnerShadowValue(cornerRadius, y);
            for(unsigned int x = 0; x < width*2-cornerRadius*2; x ++) {
                setInnerShadowPixel(cornerRadius+x, y, value);
                setInnerShadowPixel(cornerRadius+x, height*2-1-y, value);
            }
        }
        
        for(unsigned int x = 0; x < cornerRadius; x ++) {
            float value = getInnerShadowValue(x, cornerRadius);
            for(unsigned int y = 0; y < height*2-cornerRadius*2; y ++) {
                setInnerShadowPixel(x, cornerRadius+y, value);
                setInnerShadowPixel(width*2-1-x, cornerRadius+y, value);
            }
        }
    }
    
    int xMinA = (roundedCorners&GUITopLeftCorner)?cornerRadius:0,
    xMaxA = width*2-((roundedCorners&GUITopRightCorner)?cornerRadius:0),
    xMinB = (roundedCorners&GUIBottomLeftCorner)?cornerRadius:0,
    xMaxB = width*2-((roundedCorners&GUIBottomRightCorner)?cornerRadius:0),
    yMinA = (roundedCorners&GUITopLeftCorner)?cornerRadius:0,
    yMaxA = height*2-((roundedCorners&GUIBottomLeftCorner)?cornerRadius:0),
    yMinB = (roundedCorners&GUITopRightCorner)?cornerRadius:0,
    yMaxB = height*2-((roundedCorners&GUIBottomRightCorner)?cornerRadius:0);
    
    for(unsigned int x = xMinA; x < xMaxA; x ++) setBorderPixel(x, 0);
    for(unsigned int x = xMinB; x < xMaxB; x ++) setBorderPixel(x, height*2-1);
    for(unsigned int y = yMinA; y < yMaxA; y ++) setBorderPixel(0, y);
    for(unsigned int y = yMinB; y < yMaxB; y ++) setBorderPixel(width*2-1, y);
    
    int error, x, y, radius = cornerRadius;
    //for(unsigned int radius = cornerRadius; radius <= cornerRadius; radius ++) {
        error = -radius;
        x = radius;
        y = 0;
        while(x >= y) {
            if(roundedCorners & GUITopLeftCorner) {
                setBorderPixel(radius-x, radius-y);
                setBorderPixel(radius-y, radius-x);
            }
            if(roundedCorners & GUITopRightCorner) {
                setBorderPixel(x+width*2-radius-1, radius-y);
                setBorderPixel(y+width*2-radius-1, radius-x);
            }
            if(roundedCorners & GUIBottomLeftCorner) {
                setBorderPixel(radius-x, y+height*2-radius-1);
                setBorderPixel(radius-y, x+height*2-radius-1);
            }
            if(roundedCorners & GUIBottomRightCorner) {
                setBorderPixel(x+width*2-radius-1, y+height*2-radius-1);
                setBorderPixel(y+width*2-radius-1, x+height*2-radius-1);
            }
            error += y ++;
            error += y;
            if(error >= 0) {
                error -= x --;
                error -= x;
            }
        }
    //}
    
    glGenTextures(1, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*2, height*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    delete [] pixels;
}

void GUIRoundedRect::drawOnScreen(bool transposed, int posX, int posY, GUIClipRect& parentClipRect) {
    GUIClipRect clipRect;
    clipRect.minPosX = max(parentClipRect.minPosX, posX-width);
    clipRect.minPosY = max(parentClipRect.minPosY, posY-height);
    clipRect.maxPosX = min(parentClipRect.maxPosX, posX+width);
    clipRect.maxPosY = min(parentClipRect.maxPosY, posY+height);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    shaderPrograms[spriteSP]->use();
    
    if(transposed) {
        btVector3 minFactor(0.5-0.5*(clipRect.maxPosY-posY)/height, 0.5+0.5*(clipRect.minPosX-posX)/width, 0.0),
                maxFactor(0.5-0.5*(clipRect.minPosY-posY)/height, 0.5+0.5*(clipRect.maxPosX-posX)/width, 0.0);
        float texCoords[] = {
            maxFactor.x(), maxFactor.y(),
            minFactor.x(), maxFactor.y(),
            minFactor.x(), minFactor.y(),
            maxFactor.x(), minFactor.y()
        };
        shaderPrograms[spriteSP]->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 2*sizeof(float), texCoords);
    }else{
        btVector3 minFactor(0.5+0.5*(clipRect.minPosX-posX)/width, 0.5-0.5*(clipRect.maxPosY-posY)/height, 0.0),
                maxFactor(0.5+0.5*(clipRect.maxPosX-posX)/width, 0.5-0.5*(clipRect.minPosY-posY)/height, 0.0);
        float texCoords[] = {
            maxFactor.x(), maxFactor.y(),
            maxFactor.x(), minFactor.y(),
            minFactor.x(), minFactor.y(),
            minFactor.x(), maxFactor.y()
        };
        shaderPrograms[spriteSP]->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 2*sizeof(float), texCoords);
    }
    
    float vertices[] = {
        (float)clipRect.maxPosX, (float)clipRect.minPosY,
        (float)clipRect.maxPosX, (float)clipRect.maxPosY,
        (float)clipRect.minPosX, (float)clipRect.maxPosY,
        (float)clipRect.minPosX, (float)clipRect.minPosY
    };
    shaderPrograms[spriteSP]->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (texture) ? *texture : 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}