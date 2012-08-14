//
//  GUIClipRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIClipRect.h"

GUIClipRect::GUIClipRect() {
    
}

GUIClipRect::GUIClipRect(GUIClipRect const &b) {
    minPosX = b.minPosX;
    minPosY = b.minPosY;
    maxPosX = b.maxPosX;
    maxPosY = b.maxPosY;
}

bool GUIClipRect::getLimSize(int posX, int posY, int width, int height, GUIClipRect& parentClipRect) {
    minPosX = -width;
    minPosY = -height;
    maxPosX = width;
    maxPosY = height;
    
    if(parentClipRect.minPosX > posX-width)
        minPosX = parentClipRect.minPosX-posX;
    if(parentClipRect.maxPosX < posX+width)
        maxPosX = parentClipRect.maxPosX-posX;
    if(parentClipRect.minPosY > posY-height)
        minPosY = parentClipRect.minPosY-posY;
    if(parentClipRect.maxPosY < posY+height)
        maxPosY = parentClipRect.maxPosY-posY;
    
    return (minPosX < maxPosX && minPosY < maxPosY);
}

GUIClipRect& GUIClipRect::operator=(const GUIClipRect& b) {
    minPosX = b.minPosX;
    minPosY = b.minPosY;
    maxPosX = b.maxPosX;
    maxPosY = b.maxPosY;
    return *this;
}