//
//  GUILabel.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
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
    void draw(const btVector3& parentTransform, GUIClipRect& parentClipRect);
    void getPosOfChar(unsigned int charIndex, unsigned int lineIndex, int& posX, int& posY);
    //! Returns the number of characters that fit in the given width
    unsigned int getCharCountThatFitsIn(unsigned int width, const std::string& text);
    //! Returns the size in UTF8 characters of text
    unsigned int getLength();
};

//! Returns the size in bytes of the next UTF8 character
unsigned char getNextCharSize(const char* str);

//! Returns the size in UTF8 characters of the given string up to byte offset "until"
unsigned int toUTF8Length(const char* str, unsigned int until);

//! Returns the size in bytes of the given string up to UTF8 offset "until"
unsigned int fromUTF8Length(const char* str, unsigned int until);

#endif