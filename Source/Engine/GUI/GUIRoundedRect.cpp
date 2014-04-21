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



GUIRoundedRect::GUIRoundedRect() :texture(0), transposed(false) {
    roundedCorners = (GUICorner) (GUICorner::TopLeft | GUICorner::TopRight | GUICorner::BottomLeft | GUICorner::BottomRight);
    innerShadow = -menu.screenView->width*0.012;
    cornerRadius = menu.screenView->width*0.02;
    width = height = 100;
    topColor = Color4(0.9);
    bottomColor = Color4(1.0);
    borderColor = Color4(0.5);
}

GUIRoundedRect::~GUIRoundedRect() {
    if(texture)
        glDeleteTextures(1, &texture);
}

void GUIRoundedRect::setBorderPixel(unsigned char* pixels, unsigned int x, unsigned int y) {
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    pixel[0] = borderColor.r*255;
    pixel[1] = borderColor.g*255;
    pixel[2] = borderColor.b*255;
    pixel[3] = borderColor.a*255;
}

void GUIRoundedRect::setBorderPixelBlended(unsigned char* pixels, unsigned int x, unsigned int y, float alpha) {
    float antiAlpha = 1.0-alpha;
    alpha *= 255;
    unsigned char* pixel = pixels+(y*width*8)+x*4;
    pixel[0] = pixel[0]*antiAlpha + borderColor.r*alpha;
    pixel[1] = pixel[1]*antiAlpha + borderColor.g*alpha;
    pixel[2] = pixel[2]*antiAlpha + borderColor.b*alpha;
    pixel[3] = pixel[3]*antiAlpha + borderColor.a*alpha;
}

float GUIRoundedRect::getInnerShadowValue(unsigned char* pixels, unsigned int x, unsigned int y) {
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

void GUIRoundedRect::setInnerShadowPixel(unsigned char* pixels, unsigned int x, unsigned int y, float value) {
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

void GUIRoundedRect::updateContent() {
    unsigned char *pixel, *pixels = new unsigned char[width*height*16];
    
    for(unsigned int y = 0; y < height*2; y ++)
        for(unsigned int x = 0; x < width*2; x ++) {
            pixel = pixels+(y*width*8)+x*4;
            pixel[3] = 255;
            if((roundedCorners & GUICorner::TopLeft) && x < cornerRadius && y < cornerRadius) {
                if(cornerRadius-x-cos(asin((float)(cornerRadius-y)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUICorner::TopRight) && x >= width*2-cornerRadius && y < cornerRadius) {
                if(x-width*2+cornerRadius+1-cos(asin((float)(cornerRadius-y)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUICorner::BottomLeft) && x < cornerRadius && y >= height*2-cornerRadius) {
                if(cornerRadius-x-cos(asin((float)(y-height*2+cornerRadius+1)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }else if((roundedCorners & GUICorner::BottomRight) && x >= width*2-cornerRadius && y >= height*2-cornerRadius) {
                if(x-width*2+cornerRadius+1-cos(asin((float)(y-height*2+cornerRadius+1)/cornerRadius))*cornerRadius > 0)
                    pixel[3] = 0;
            }
            if(pixel[3] == 0) {
                pixel[0] = pixel[1] = pixel[2] = 0;
                continue;
            }
            float scale = (transposed) ? 0.5*x/width : 0.5*y/height;
            pixel[0] = ((bottomColor.r-topColor.r)*scale+topColor.r)*255.0;
            pixel[1] = ((bottomColor.g-topColor.g)*scale+topColor.g)*255.0;
            pixel[2] = ((bottomColor.b-topColor.b)*scale+topColor.b)*255.0;
            pixel[3] = ((bottomColor.a-topColor.a)*scale+topColor.a)*255.0;
        }
    
    if(innerShadow != 0) {
        for(unsigned int y = 0; y < cornerRadius; y ++)
            for(unsigned int x = 0; x < cornerRadius; x ++) {
                unsigned int mirX = width*2-1-x, mirY = height*2-1-y;
                setInnerShadowPixel(pixels, x, y, getInnerShadowValue(pixels, x, y));
                setInnerShadowPixel(pixels, mirX, y, getInnerShadowValue(pixels, mirX, y));
                setInnerShadowPixel(pixels, x, mirY, getInnerShadowValue(pixels, x, mirY));
                setInnerShadowPixel(pixels, mirX, mirY, getInnerShadowValue(pixels, mirX, mirY));
            }
        
        for(unsigned int y = 0; y < cornerRadius; y ++) {
            float value = getInnerShadowValue(pixels, cornerRadius, y);
            for(unsigned int x = 0; x < width*2-cornerRadius*2; x ++) {
                setInnerShadowPixel(pixels, cornerRadius+x, y, value);
                setInnerShadowPixel(pixels, cornerRadius+x, height*2-1-y, value);
            }
        }
        
        for(unsigned int x = 0; x < cornerRadius; x ++) {
            float value = getInnerShadowValue(pixels, x, cornerRadius);
            for(unsigned int y = 0; y < height*2-cornerRadius*2; y ++) {
                setInnerShadowPixel(pixels, x, cornerRadius+y, value);
                setInnerShadowPixel(pixels, width*2-1-x, cornerRadius+y, value);
            }
        }
    }
    
    int xMinA = (roundedCorners&GUICorner::TopLeft)?cornerRadius:0,
    xMaxA = width*2-((roundedCorners&GUICorner::TopRight)?cornerRadius:0),
    xMinB = (roundedCorners&GUICorner::BottomLeft)?cornerRadius:0,
    xMaxB = width*2-((roundedCorners&GUICorner::BottomRight)?cornerRadius:0),
    yMinA = (roundedCorners&GUICorner::TopLeft)?cornerRadius:0,
    yMaxA = height*2-((roundedCorners&GUICorner::BottomLeft)?cornerRadius:0),
    yMinB = (roundedCorners&GUICorner::TopRight)?cornerRadius:0,
    yMaxB = height*2-((roundedCorners&GUICorner::BottomRight)?cornerRadius:0);
    
    for(unsigned int x = xMinA; x < xMaxA; x ++) setBorderPixel(pixels, x, 0);
    for(unsigned int x = xMinB; x < xMaxB; x ++) setBorderPixel(pixels, x, height*2-1);
    for(unsigned int y = yMinA; y < yMaxA; y ++) setBorderPixel(pixels, 0, y);
    for(unsigned int y = yMinB; y < yMaxB; y ++) setBorderPixel(pixels, width*2-1, y);
    
    int error, x, y;
    for(unsigned int radius = cornerRadius; radius <= cornerRadius; radius ++) {
        error = -radius;
        x = radius;
        y = 0;
        while(x >= y) {
            float alpha = fmax(0.0, sqrt(x*x+(y+1)*(y+1))-cornerRadius);
            
            if(roundedCorners & GUICorner::TopLeft) {
                setBorderPixel(pixels, radius-x, radius-y);
                setBorderPixel(pixels, radius-y, radius-x);
                setBorderPixelBlended(pixels, radius-x+1, radius-y, alpha);
                setBorderPixelBlended(pixels, radius-y, radius-x+1, alpha);
            }
            if(roundedCorners & GUICorner::TopRight) {
                setBorderPixel(pixels, x+width*2-radius-1, radius-y);
                setBorderPixel(pixels, y+width*2-radius-1, radius-x);
                setBorderPixelBlended(pixels, x+width*2-radius-2, radius-y, alpha);
                setBorderPixelBlended(pixels, y+width*2-radius-1, radius-x+1, alpha);
            }
            if(roundedCorners & GUICorner::BottomLeft) {
                setBorderPixel(pixels, radius-x, y+height*2-radius-1);
                setBorderPixel(pixels, radius-y, x+height*2-radius-1);
                setBorderPixelBlended(pixels, radius-x+1, y+height*2-radius-1, alpha);
                setBorderPixelBlended(pixels, radius-y, x+height*2-radius-2, alpha);
            }
            if(roundedCorners & GUICorner::BottomRight) {
                setBorderPixel(pixels, x+width*2-radius-1, y+height*2-radius-1);
                setBorderPixel(pixels, y+width*2-radius-1, x+height*2-radius-1);
                setBorderPixelBlended(pixels, x+width*2-radius-2, y+height*2-radius-1, alpha);
                setBorderPixelBlended(pixels, y+width*2-radius-1, x+height*2-radius-2, alpha);
            }
            error += y ++;
            error += y;
            if(error >= 0) {
                error -= x --;
                error -= x;
            }
        }
    }
    
    if(!texture)
        glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*2, height*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    delete [] pixels;
}

void GUIRoundedRect::drawOnScreen(const btVector3& parentTransform, int posX, int posY, GUIClipRect &parentClipRect) {
    if(!texture) updateContent();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GUIDrawableRect::drawOnScreen(parentTransform, posX, posY, parentClipRect);
}