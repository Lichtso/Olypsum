//
//  GUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "TextFont.h"

#ifndef GUIRect_h
#define GUIRect_h

struct GUIClipRect {
    int minPosX, minPosY, maxPosX, maxPosY;
};

class GUIRect {
    public:
    GUIRect* parent;
    unsigned int posX, posY, width, height;
    GUIRect();
    virtual void recalculateSize();
    virtual void getLimSize(GUIClipRect& clipRect);
    virtual void handleMouseDown(Vector3 relativePos);
    virtual void handleMouseUp(Vector3 relativePos);
    virtual void handleMouseMove(Vector3 relativePos);
    virtual void draw(Matrix4& parentTransform);
};

#endif