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
    std::vector<GUIRect*> children;
    void addChild(GUIRect* child);
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    void handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

class GUIScreenView : public GUIView {
    public:
    GUIScreenView();
    void getLimSize(GUIClipRect* clipRect);
    void updateContent();
    void draw();
    void handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

extern GUIScreenView* currentScreenView;

#endif