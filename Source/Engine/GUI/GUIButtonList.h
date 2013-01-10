//
//  GUIButtonList.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIButton.h"

#ifndef GUIButtonList_h
#define GUIButtonList_h

class GUIButtonList : public GUIView {
    public:
    void addChild(GUIButton* child);
    GUISizeAlignment sizeAlignment;
    GUIOrientation orientation;
    GUIButtonList();
    void updateContent();
};

#endif
