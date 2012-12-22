//
//  GUILabel.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "TextFont.h"
#import "GUIRect.h"

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
    std::string text;
    GUIClipRect clipRect;
};

class GUILabel : public GUIRect {
    public:
    std::vector<GUILabelLine> lines;
    Color4 color;
    TextFont* font;
    GUISizeAlignment sizeAlignment;
    unsigned int fontHeight;
    GUITextAlign textAlign;
    std::string text;
    GUILabel();
    ~GUILabel();
    void updateContent();
    void draw(btVector3 transform, GUIClipRect& parentClipRect);
    unsigned char getCharSizeAt(unsigned int byteIndex);
    unsigned int getUTF8Length();
    void getPosOfChar(unsigned int charIndex, unsigned int lineIndex, int& posX, int& posY);
};

#endif