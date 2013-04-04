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

void openExternURL(const char* str);

class Menu {
    public:
    enum Name {
        loading = 0,
        main,
        options,
        videoResolution,
        languages,
        credits,
        inGame,
        gameEsc,
        saveGames,
        newGame,
        multiplayer
    } current;
    
    void handleKeyUp(SDL_keysym* key);
    void gameTick();
    void setMenu(Name menu);
};

extern Menu menu;

#endif