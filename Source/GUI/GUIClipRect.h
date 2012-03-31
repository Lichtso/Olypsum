//
//  GUIClipRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "TextFont.h"

#ifndef GUIClipRect_h
#define GUIClipRect_h

class GUIClipRect {
    public:
    int minPosX, minPosY, maxPosX, maxPosY;
    GUIClipRect();
    GUIClipRect(GUIClipRect const &);
    GUIClipRect& operator=(const GUIClipRect&);
};

#endif
