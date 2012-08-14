//
//  GUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIRoundedRect.h"

#ifndef GUIView_h
#define GUIView_h

class GUIView : public GUIRect {
    public:
    std::vector<GUIRect*> children;
    GUIView();
    ~GUIView();
    virtual void addChild(GUIRect* child);
    void removeChild(unsigned int index);
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
};

class GUIFramedView : public GUIView {
    protected:
    GLuint texture;
    public:
    int innerShadow;
    GUIFramedView();
    ~GUIFramedView();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
};

class GUIScreenView : public GUIView {
    public:
    GUIRect *modalView, *firstResponder;
    GUIScreenView();
    bool getLimSize(GUIClipRect& clipRect);
    void updateContent();
    void draw();
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
    bool handleKeyDown(SDL_keysym* key);
    bool handleKeyUp(SDL_keysym* key);
    void setModalView(GUIRect* modalView);
};

extern GUIScreenView* currentScreenView;

#endif