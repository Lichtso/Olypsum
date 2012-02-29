//
//  GUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIRect.h"

#ifndef GUIView_h
#define GUIView_h

class GUIView : public GUIRect {
    public:
    unsigned int viewPosX, viewPosY;
    std::vector<GUIRect*> children;
    void addChild(GUIRect* child);
    void handleMouseDown(Vector3 relativePos);
    void handleMouseUp(Vector3 relativePos);
    void handleMouseMove(Vector3 relativePos);
    void draw(Matrix4& parentTransform);
};

class GUIScreenView : public GUIView {
    public:
    GUIScreenView();
    void recalculateSize();
    void getLimSize(GUIClipRect& clipRect);
    void draw();
};

extern GUIScreenView* currentScreenView;

#endif