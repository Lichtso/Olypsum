//
//  GUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIRoundedRect.h"

#ifndef GUIView_h
#define GUIView_h

class GUIView : public GUIRect {
    protected:
    std::vector<GUIRect*> children;
    public:
    GUIView();
    ~GUIView();
    virtual void addChild(GUIRect* child);
    void removeChild(unsigned int index);
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
};

class GUIScreenView : public GUIView {
    public:
    GUIRect* firstResponder;
    GUIScreenView();
    bool getLimSize(GUIClipRect& clipRect);
    void updateContent();
    void draw();
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
    bool handleKeyDown(SDL_keysym* key);
    bool handleKeyUp(SDL_keysym* key);
};

extern GUIScreenView* currentScreenView;

#endif