//
//  GUITabs.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButtonList.h"

#ifndef GUITabs_h
#define GUITabs_h

class GUITabs : public GUIButtonList {
    public:
    int selectedIndex;
    bool deactivatable;
    void (*onChange)(GUITabs* tabs);
    GUITabs();
    void addChild(GUIButton* child);
    void updateContent();
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif
