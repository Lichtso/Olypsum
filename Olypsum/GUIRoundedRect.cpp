//
//  GUIRoundedRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIRoundedRect.h"

GUIRoundedRect::GUIRoundedRect() {
    texture = NULL;
    roundedCorners = (GUICorners) (GUITopLeftCorner | GUITopRightCorner | GUIBottomLeftCorner | GUIBottomRightCorner);
    cornerRadius = 8;
    shadowWidth = 0;
    width = height = 0;
    topColor.r = topColor.g = topColor.b = 240;
    bottomColor.r = bottomColor.g = bottomColor.b = 230;
    borderColor.r = borderColor.g = borderColor.b = 130;
    topColor.a = bottomColor.a = borderColor.a = 255;
}

void GUIRoundedRect::setBorderPixel(unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    pixel[0] = borderColor.r;
    pixel[1] = borderColor.g;
    pixel[2] = borderColor.b;
    pixel[3] = borderColor.a;
}

void GUIRoundedRect::setInnerShadowRect(unsigned int minX, unsigned int maxX, unsigned int minY, unsigned int maxY) {
    float value;
    int x0, x1, y0, y1;
    unsigned char* pixel;
    for(unsigned int y = minY; y < maxY; y ++)
        for(unsigned int x = minX; x < maxX; x ++) {
            value = 0.0;
            x0 = max(0, (int)(x-shadowWidth));
            y0 = max(0, (int)(y-shadowWidth));
            x1 = min(width*2-1, (int)(x+shadowWidth));
            y1 = min(height*2-1, (int)(y+shadowWidth));
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
            pixel[0] = ((float)bottomColor.r-(float)topColor.r)*0.5*y/height+topColor.r;
            pixel[1] = ((float)bottomColor.g-(float)topColor.g)*0.5*y/height+topColor.g;
            pixel[2] = ((float)bottomColor.b-(float)topColor.b)*0.5*y/height+topColor.b;
        }
    
    if(shadowWidth > 0) {
        setInnerShadowRect(cornerRadius, width*2-cornerRadius, 0, cornerRadius);
        setInnerShadowRect(cornerRadius, width*2-cornerRadius, height*2-cornerRadius, height*2);
        setInnerShadowRect(0, cornerRadius, 0, height*2);
        setInnerShadowRect(width*2-cornerRadius, width*2, 0, height*2);
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
    
    int error, x, y;
    for(unsigned int radius = cornerRadius-1; radius < cornerRadius+1; radius ++) {
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
    }
    
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*2, height*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    delete [] pixels;
}

void GUIRoundedRect::drawOnScreen(GUIClipRect& clipRect) {
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
    
    spriteShaderProgram->use();
    spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
    spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}