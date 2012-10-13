//
//  Menu.h
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <SDL/SDL.h>
#import "WorldManager.h"
#import "GUIScrollView.h"
#import "GUICheckBox.h"
#import "GUITabs.h"
#import "GUISlider.h"
#import "GUITextField.h"
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
    gameEscMenu = 6,
    saveGamesMenu = 7,
    newGameMenu = 8,
    removeGameMenu = 9
};

void handleMenuKeyUp(SDL_keysym* key);
void setMenu(MenuName menu);

extern MenuName currentMenu;

#endif