//
//  GUIButtonList.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUICheckBox.h"

#ifndef GUIButtonList_h
#define GUIButtonList_h

enum GUIOrientation {
    GUIOrientation_Left = 1,
    GUIOrientation_Right = 2,
    GUIOrientation_Top = 4,
    GUIOrientation_Bottom = 8,
    GUIOrientation_Vertical = 3,
    GUIOrientation_Horizontal = 12
};

class GUIButtonList : public GUIView {
    public:
    void addChild(GUIButton* child);
    bool autoSize;
    GUIOrientation orientation;
    GUIButtonList();
    void updateContent();
};

#endif
