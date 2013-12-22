//
//  Menu.h
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Menu_h
#define Menu_h

#include "LevelManager.h"

class Menu {
    static void openExternURL(std::string str);
    static void leaveOptionsMenu(GUIButton* button);
    public:
    GUIScreenView* screenView;
    enum Name {
        none,
        loading,
        main,
        options,
        languages,
        credits,
        inGame,
        gameEsc,
        saveGames,
        newGame,
        multiplayer
    } current = none;
    struct ConsoleEntry {
        std::string message;
        float timeLeft;
    };
    std::vector<ConsoleEntry> consoleMessages;
    float mouseX = 0.0, mouseY = 0.0,
          mouseVelocityX = 0.0, mouseVelocityY = 0.0;
    bool  mouseFixed = true;
    
    Menu();
    void updateWindow();
    void updateVideoResulution();
    void consoleAdd(const std::string& message, float duration = 10.0);
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);
    void handleMouseWheel(float deltaX, float deltaY);
    void handleKeyDown(SDL_Keycode key, const char* text);
    void handleKeyUp(SDL_Keycode key);
    void gameTick();
    void sendPauseEvent();
    void setModalView(const std::string& title, const std::string& text, std::function<void(GUIButton* button)> onContinue);
    void clear();
    void clearAndAddBackground();
    void setLoadingMenu();
    void setMainMenu();
    void setOptionsMenu();
    void setLanguagesMenu();
    void setCreditsMenu();
    void setInGameMenu();
    void setGameEscMenu();
    void setSaveGamesMenu();
    void setNewGameMenu();
    void setMultiplayerMenu();
};

extern Menu menu;

#endif