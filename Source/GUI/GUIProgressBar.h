//
//  GUIProgressBar.h
//  Olypsum
//
//  Created by Alexander Meißner on 17.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUITabs.h"

#ifndef GUIProgressBar_h
#define GUIProgressBar_h

class GUIProgressBar : public GUIRect {
    GLuint textureL, textureR;
    void generateBar(bool filled);
    void drawBar(GUIClipRect* clipRect, unsigned int barLength, bool filled);
    public:
    float value;
    GUIOrientation orientation;
    GUIProgressBar();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
};

#endif
