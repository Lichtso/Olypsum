//
//  GUIScrollView.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIView.h"

#ifndef GUIScrollView_h
#define GUIScrollView_h

class GUIScrollView : public GUIFramedView {
    GLuint textureV, textureH;
    int mouseDragPosX, mouseDragPosY;
    int getBarWidth();
    int getBarHeight();
    int getBarPosX();
    int getBarPosY();
    public:
    bool hideSliderX, hideSliderY;
    int scrollPosX, scrollPosY, scrollWidth, scrollHeight;
    GUIScrollView();
    ~GUIScrollView();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect& parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
};

#endif
