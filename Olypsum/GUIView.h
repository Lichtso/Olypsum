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
    protected:
    std::vector<GUIRect*> children;
    public:
    GUIView();
    virtual void addChild(GUIRect* child);
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

class GUIScreenView : public GUIView {
    public:
    GUIScreenView();
    void getLimSize(GUIClipRect* clipRect);
    void updateContent();
    void draw();
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

extern GUIScreenView* currentScreenView;

#endif