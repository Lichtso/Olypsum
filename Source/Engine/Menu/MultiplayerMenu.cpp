//
//  MultiplayerMenu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.12.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void Menu::setMultiplayerMenu() {
    clearAndAddBackground();
    current = multiplayer;
    
    if(optionsState.nickname.size() == 0) {
		char whoami[256], nickname[512];
		FILE* f = popen("whoami", "r");
		fgets(whoami, 128, f);
		pclose(f);
		size_t len = strlen(whoami)-1;
		whoami[len] = 0;
#ifdef WIN32
		char* username = strchr(whoami, '\\');
		*(username ++) = 0;
		sprintf(nickname, "%s@%s", username, whoami);
#else
		char hostname[128];
        f = popen("hostname -s", "r");
        fgets(hostname, 128, f);
        pclose(f);
        len = strlen(hostname)-1;
        hostname[len] = 0;
		sprintf(nickname, "%s@%s", whoami, hostname);
#endif
        optionsState.nickname = nickname;
    }
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("multiplayer");
    label->fontHeight = screenView->height*0.2;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    GUIScrollView* view = new GUIScrollView();
    view->width = screenView->width*0.8;
    view->height = screenView->height*0.6;
    view->content.edgeGradientBorder = 0.5;
    screenView->addChild(view);
    
    GUIButton* button = new GUIButton();
    button->posX = screenView->width*-0.6;
    button->posY = screenView->height*-0.8;
    button->onClick = [this](GUIButton* button) {
        networkManager.disable();
        setMainMenu();
    };
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("return");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.14;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    
    GUITextField* textField = new GUITextField();
    label = static_cast<GUILabel*>(textField->children[0]);
    label->text = optionsState.nickname;
    textField->posY = screenView->height*-0.8;
    textField->width = screenView->width*0.3;
    textField->height = screenView->height*0.07;
    textField->onChange = [label](GUITextField* textField) {
        optionsState.nickname = label->text;
    };
    screenView->addChild(textField);
    
    GUITabs* tabs = new GUITabs();
    tabs->posX = screenView->width*0.6;
    tabs->posY = screenView->height*-0.8;
    tabs->deactivatable = false;
    tabs->width = screenView->width*0.152;
    tabs->height = screenView->height*0.06;
    tabs->sizeAlignment = GUISizeAlignment::None;
    tabs->orientation = GUIOrientation::Horizontal;
    std::vector<std::string> options = { "ipV4", "auto", "ipV6" };
    tabs->onChange = [options](GUITabs* tabs) {
        optionsState.ipVersion = options[tabs->selected];
        networkManager.enable();
    };
    for(unsigned char i = 0; i < options.size(); i ++) {
        GUIButton* button = new GUIButton();
        label = new GUILabel();
        label->text = fileManager.localizeString(options[i]);
        label->fontHeight = screenView->height*0.06;
        button->addChild(label);
        tabs->addChild(button);
        if(options[i] == optionsState.ipVersion)
            tabs->selected = i;
    }
    screenView->addChild(tabs);
    
    screenView->updateContent();
    networkManager.enable();
}