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
    float mouseVelocityX = 0.0, mouseVelocityY = 0.0;
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
    struct ConsoleEntry {
        std::string message;
        float timeLeft;
    };
    std::vector<ConsoleEntry> consoleMessages;
    float mouseMotionX = 0.0, mouseMotionY = 0.0;
    
    void consoleAdd(const std::string& message, float duration = 10.0);
    void handleActiveEvent(bool active);
    void handleMouseDown(SDL_Event& event);
    void handleMouseUp(SDL_Event& event);
    void handleMouseMove(SDL_Event& event);
    void handleMouseWheel(SDL_Event& event);
    void handleKeyDown(SDL_Event& event);
    void handleKeyUp(SDL_Event& event);
    void gameTick();
    void setMenu(Name menu);
};

extern Menu menu;

#endif