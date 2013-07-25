//
//  GUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUIView_h
#define GUIView_h

#include "GUIRoundedRect.h"

class GUIView : public GUIRect {
    public:
    std::vector<GUIRect*> children;
    GUIView();
    ~GUIView();
    //! Adopts a child with or without a parent
    virtual bool addChild(GUIRect* child);
    //! Returns the index of a given child or -1 if this is not the parent
    int getIndexOfChild(GUIRect* child);
    //! Deletes the child at given index
    void deleteChild(unsigned int index);
    void updateContent();
    void draw(btVector3 parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY);
};

class GUIFramedView : public GUIView {
    public:
    GUIRoundedRect content;
    GUIFramedView();
    void updateContent();
    void draw(btVector3 parentTransform, GUIClipRect& parentClipRect);
};

class GUIScreenView : public GUIView {
    public:
    GUIRect *modalView, *focused;
    GUIScreenView();
    ~GUIScreenView();
    bool getLimSize(GUIClipRect& clipRect);
    void updateContent();
    void draw();
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY);
    bool handleKeyDown(SDL_keysym* key);
    bool handleKeyUp(SDL_keysym* key);
    void setModalView(GUIRect* modalView);
};

#endif