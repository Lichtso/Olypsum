//
//  GUILabel.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef GUILabel_h
#define GUILabel_h

#include "GUIScrollView.h"

struct GUILabelLine {
    GLuint texture;
    GUIDrawableRect content;
    float scale;
    int posX, posY;
    std::string text;
};

class GUILabel : public GUIRect {
    void addSegment(unsigned int& newWidth, unsigned int& newHeight, std::string text);
    void addLine(unsigned int& newWidth, unsigned int& newHeight, std::string text);
    public:
    std::vector<GUILabelLine> lines;
    Color4 color;
    FileResourcePtr<TextFont> font;
    GUISizeAlignment sizeAlignment;
    unsigned int fontHeight;
    enum TextAlignment {
        Left = 0,
        Middle = 1,
        Right = 2,
    } textAlignment;
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