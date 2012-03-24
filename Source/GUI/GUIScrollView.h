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

class GUIScrollView : public GUIView {
    public:
    int scrollPosX, scrollPosY, scrollWidth, scrollHeight;
    GUIScrollView();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    bool handleMouseWheel(int mouseX, int mouseY, float delta);
};

#endif
