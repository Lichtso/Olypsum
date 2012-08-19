//
//  Menu.h
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIImage.h"

#ifndef Menu_h
#define Menu_h

enum MenuName {
    loadingMenu = 0,
    mainMenu = 1,
    optionsMenu = 2,
    languagesMenu = 3,
    creditsMenu = 4,
    inGameMenu = 5,
    gameEscMenu = 6
};

enum GameStatusName {
    noGame = 0,
    localGame = 1,
    networkGame = 2
};

void handleMenuKeyUp(SDL_keysym* key);
void setMenu(MenuName menu);

extern MenuName currentMenu;
extern GameStatusName gameStatus;

#endif