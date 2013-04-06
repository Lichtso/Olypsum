//
//  GUILabel.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "TextFont.h"
#include "GUIView.h"

#ifndef GUILabel_h
#define GUILabel_h

enum GUITextAlign {
    GUITextAlign_Left = 0,
    GUITextAlign_Middle = 1,
    GUITextAlign_Right = 2,
};

struct GUILabelLine {
    GLuint texture;
    GUIDrawableRect content;
    int posX, posY;
    std::string text;
};

class GUILabel : public GUIRect {
    void addSegment(std::string text);
    void addLine(unsigned int warpWidth, std::string text);
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
    bool handleMouseDown(int mouseX, int mouseY);
    void getPosOfChar(unsigned int charIndex, unsigned int lineIndex, int& posX, int& posY);
    unsigned int getCharCountThatFitsIn(unsigned int width, const std::string& text);
};

unsigned char getNextCharSize(const char* str);
unsigned int getUTF8Length(const char* str);

#endif