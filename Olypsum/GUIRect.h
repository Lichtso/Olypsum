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
    bool visible;
    int posX, posY, width, height;
    GUIRect();
    virtual void getLimSize(GUIClipRect* parentClipRect, GUIClipRect* clipRect);
    virtual void updateContent();
    virtual void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    virtual bool handleMouseDown(int mouseX, int mouseY);
    virtual void handleMouseUp(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY);
};

#endif