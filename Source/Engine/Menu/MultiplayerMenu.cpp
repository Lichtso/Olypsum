//
//  MultiplayerMenu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.12.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void Menu::setMultiplayerMenu() {
    clearAndAddBackground();
    current = multiplayer;
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("multiplayer");
    label->fontHeight = screenView->height*0.2;
    screenView->addChild(label);
    
    GUIButton* button = new GUIButton();
    button->posX = screenView->width*-0.4;
    button->posY = screenView->height*-0.8;
    button->onClick = [this](GUIButton* button) {
        networkManager.setLocalScan(false);
        setMainMenu();
    };
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("back");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.14;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    
    GUIScrollView* view = new GUIScrollView();
    view->width = screenView->width*0.7;
    view->height = screenView->height*0.6;
    view->content.innerShadow = view->content.cornerRadius * 0.5;
    screenView->addChild(view);
    
    GUITabs* tabs = new GUITabs();
    tabs->posX = screenView->width*0.4;
    tabs->posY = screenView->height*-0.8;
    tabs->deactivatable = false;
    tabs->width = screenView->width*0.152;
    tabs->height = screenView->height*0.06;
    tabs->sizeAlignment = GUISizeAlignment::None;
    tabs->orientation = GUIOrientation::Horizontal;
    tabs->onChange = [](GUITabs* tabs) {
        
    };
    std::vector<std::string> options = { "ipV4", "auto", "ipV6" };
    for(unsigned char i = 0; i < options.size(); i ++) {
        GUIButton* button = new GUIButton();
        label = new GUILabel();
        label->text = fileManager.localizeString(options[i]);
        label->fontHeight = screenView->height*0.06;
        button->addChild(label);
        tabs->addChild(button);
    }
    tabs->selected = 1;
    screenView->addChild(tabs);
    
    screenView->updateContent();
    networkManager.setLocalScan(true);
}