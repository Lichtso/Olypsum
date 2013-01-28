//
//  Menu.h
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <SDL/SDL.h>
#include "LevelManager.h"
#include "GUIScrollView.h"
#include "GUICheckBox.h"
#include "GUITabs.h"
#include "GUIProgressBar.h"
#include "GUISlider.h"
#include "GUITextField.h"
#include "GUIImage.h"

#ifndef Menu_h
#define Menu_h

enum MenuName {
    loadingMenu = 0,
    mainMenu,
    optionsMenu,
    languagesMenu,
    creditsMenu,
    inGameMenu,
    gameEscMenu,
    saveGamesMenu,
    newGameMenu,
    modalMenu
};

void openExternURL(const char* str);

void handleMenuKeyUp(SDL_keysym* key);
void setMenu(MenuName menu);

extern MenuName currentMenu;

#endif