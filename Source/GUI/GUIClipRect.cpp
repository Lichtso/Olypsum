//
//  GUIClipRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
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

GUIClipRect& GUIClipRect::operator=(const GUIClipRect& b) {
    minPosX = b.minPosX;
    minPosY = b.minPosY;
    maxPosX = b.maxPosX;
    maxPosY = b.maxPosY;
    return *this;
}