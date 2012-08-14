//
//  GUIClipRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "TextFont.h"

#ifndef GUIClipRect_h
#define GUIClipRect_h

enum GUISizeAlignment {
    GUISizeAlignment_None = 0,
    GUISizeAlignment_Width = 1,
    GUISizeAlignment_Height = 2,
    GUISizeAlignment_All = 3
};

class GUIClipRect {
    public:
    int minPosX, minPosY, maxPosX, maxPosY;
    GUIClipRect();
    GUIClipRect(GUIClipRect const &);
    bool getLimSize(int posX, int posY, int width, int height, GUIClipRect& parentClipRect);
    GUIClipRect& operator=(const GUIClipRect&);
};

#endif
