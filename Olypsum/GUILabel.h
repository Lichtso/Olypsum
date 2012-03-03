//
//  GUILabel.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIView.h"

#ifndef GUILabel_h
#define GUILabel_h

enum GUITextAlign {
    GUITextAlign_Left = 0,
    GUITextAlign_Middle = 1,
    GUITextAlign_Right = 2,
};

struct GUILabelLine {
    GLuint texture;
    int posX, posY, width, height;
    GUIClipRect clipRect;
};

class GUILabel : public GUIRect {
    std::vector<GUILabelLine> lines;
    public:
    SDL_Color color;
    TextFont* font;
    bool autoSize;
    unsigned int fontHeight;
    GUITextAlign textAlign;
    std::string text;
    GUILabel();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
};

#endif