//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "AppMain.h"

const std::string consoleJSCommand = "#! ";
const std::string homepage = "http://gamefortec.net/";
const float loadingScreenTime = 5.0;
float loadingScreen = loadingScreenTime;
unsigned int matrixEasterEgg_X = 0, matrixEasterEgg_Y = 0;

Menu::Menu() :current(none), screenView(NULL) {
    
}

void Menu::openExternURL(std::string str) {
#ifdef WIN32
	str = std::string("start ") + str;
#elif defined __APPLE__
    str = std::string("open \"")+str+"\"";
#else
    str = std::string("xdg-open \"")+str+"\"";
#endif
	std::system(str.c_str());
}

void Menu::leaveOptionsMenu(GUIButton* button) {
    if(levelManager.gameStatus == noGame)
        menu.setMainMenu();
    else
        menu.setGameEscMenu();
}

void Menu::updateWindow() {
    SDL_DisplayMode desktopDisplay;
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_GetDesktopDisplayMode(0, &desktopDisplay);
    
    if(optionsState.videoWidth == desktopDisplay.w && optionsState.videoHeight == desktopDisplay.h)
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    
    if(optionsState.videoScale > 1)
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
    
    if(mainWindow) SDL_HideWindow(mainWindow);
    SDL_Window* newWindow = SDL_CreateWindow("Olypsum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             optionsState.videoWidth, optionsState.videoHeight, windowFlags);
    
    if(mainWindow) SDL_DestroyWindow(mainWindow);
    mainWindow = newWindow;
    
#ifdef WIN32
	{
		std::string filePath = resourcesPath+"Icon64.png";
		SDL_Surface* surface = IMG_Load(filePath.c_str());
		if(surface) {
			SDL_SetWindowIcon(mainWindow, surface);
			SDL_FreeSurface(surface);
		}else
			log(error_log, std::string("Unable to load icon.\n")+IMG_GetError());
	}
#endif
    
    SDL_RaiseWindow(mainWindow);
    if(glContext)
        SDL_GL_MakeCurrent(mainWindow, glContext);
    else {
        glContext = SDL_GL_CreateContext(mainWindow);
        SDL_GL_SetSwapInterval(optionsState.vSyncEnabled);
    }
    
    int width, height;
    SDL_GL_GetDrawableSize(mainWindow, &width, &height);
    optionsState.videoScale = width/optionsState.videoWidth;
    if(menu.current)
        updateSSAOShaderPrograms();
}

void Menu::updateVideoResulution() {
    SDL_GetWindowSize(mainWindow, &optionsState.videoWidth, &optionsState.videoHeight);
    optionsState.videoWidth -= optionsState.videoWidth % 2;
    optionsState.videoHeight -= optionsState.videoHeight % 2;
    SDL_SetWindowSize(mainWindow, optionsState.videoWidth, optionsState.videoHeight);
    
    mainFBO.init();
    if(levelManager.gameStatus != noGame) {
        mainCam->updateViewMat();
        for(auto lightObject : objectManager.lightObjects)
            lightObject->deleteShadowMap();
    }
    
    switch(current) {
        case none:
            clear();
            break;
        case loading:
            setLoadingMenu();
            break;
        case main:
            setMainMenu();
            break;
        case options:
            setOptionsMenu();
            break;
        case languages:
            setLanguagesMenu();
            break;
        case credits:
            setCreditsMenu();
            break;
        case inGame:
            setInGameMenu();
            break;
        case console:
            setConsoleMenu();
            break;
        case gameEsc:
            setGameEscMenu();
            break;
        case saveGames:
            setSaveGamesMenu();
            break;
        case newGame:
            setNewGameMenu();
            break;
        case multiplayer:
            setMultiplayerMenu();
            break;
    }
}

void Menu::consoleAdd(const std::string& message, float duration) {
    ConsoleEntry entry;
    entry.message = message;
    for(size_t i = 0; i < entry.message.size(); i ++)
        if(entry.message[i] == 9)
            entry.message = entry.message.replace(i, 1, "    ");
    entry.timeLeft = duration;
    consoleMessages.push_back(entry);
}

void Menu::consoleHandle(const std::string& message) {
    if(message.length() == 0) return;
    
    if(message.find(consoleJSCommand) == 0) {
        ScriptString strSourceCode(message.substr(consoleJSCommand.length()));
        JSValueRef exception = NULL, result = JSEvaluateScript(scriptManager->mainScript->context, strSourceCode.str, NULL, NULL, 0, &exception);
        if(exception)
            ScriptLogException(scriptManager->mainScript->context, exception);
        else if(result && !JSValueIsUndefined(scriptManager->mainScript->context, result)) {
            ScriptString strResult(scriptManager->mainScript->context, result);
            log(script_log, strResult.getStdStr());
        }
    }else
        log(user_log, message);
}

void Menu::handleMouseDown(Uint8 button) {
    if((button == SDL_BUTTON_LEFT && screenView->handleMouseDown(mouseX, mouseY))
       || menu.current != inGame) return;
    
    JSValueRef argv[] = { JSValueMakeNumber(scriptManager->mainScript->context, button) };
    scriptManager->mainScript->callFunction("onmousedown", false, 1, argv);
}

void Menu::handleMouseUp(Uint8 button) {
    if(screenView->handleMouseUp(mouseX, mouseY) || menu.current != inGame) return;
    
    JSValueRef argv[] = { JSValueMakeNumber(scriptManager->mainScript->context, button) };
    scriptManager->mainScript->callFunction("onmouseup", false, 1, argv);
}

void Menu::handleMouseWheel(float deltaX, float deltaY) {
    if(screenView->handleMouseWheel(mouseX, mouseY, deltaX, deltaY) || menu.current != inGame) return;
    
    JSValueRef argv[] = { JSValueMakeNumber(scriptManager->mainScript->context, deltaX), JSValueMakeNumber(scriptManager->mainScript->context, deltaY) };
    scriptManager->mainScript->callFunction("onmousewheel", false, 2, argv);
}

void Menu::handleKeyDown(SDL_Keycode key, const char* text) {
    if(screenView->handleKeyDown(key, text) || menu.current != inGame) return;
    
    JSValueRef argv[] = { JSValueMakeNumber(scriptManager->mainScript->context, key) };
    scriptManager->mainScript->callFunction("onkeydown", false, 1, argv);
}

void Menu::handleKeyUp(SDL_Keycode key) {
    if(screenView->handleKeyUp(key)) return;
    
    switch(key) {
        case SDLK_ESCAPE:
            switch(current) {
                case none:
                case loading:
                case main:
                    AppTerminate();
                    return;
                case options:
                    leaveOptionsMenu(NULL);
                    return;
                case languages:
                    setOptionsMenu();
                    return;
                case multiplayer:
                    networkManager.disable();
                case credits:
                case saveGames:
                    setMainMenu();
                    return;
                case inGame:
                    setGameEscMenu();
                    sendPauseEvent();
                    return;
                case console:
                case gameEsc:
                    setInGameMenu();
                    sendPauseEvent();
                    return;
                case newGame:
                    setSaveGamesMenu();
                    return;
            }
        case SDLK_RETURN:
            switch(current) {
                case inGame:
                    setConsoleMenu();
                    sendPauseEvent();
                    return;
                case console: {
                    GUITextField* textField = static_cast<GUITextField*>(screenView->children[1]);
                    GUILabel* label = static_cast<GUILabel*>(textField->children[0]);
                    consoleHandle(label->text);
                    setInGameMenu();
                    sendPauseEvent();
                } return;
                default:
                    return;
            }
            return;
    }
    
    if(menu.current == inGame) {
        JSValueRef argv[] = { JSValueMakeNumber(scriptManager->mainScript->context, key) };
        scriptManager->mainScript->callFunction("onkeyup", false, 1, argv);
    }
}

void Menu::gameTick() {
    if(optionsState.screenBlurFactor > -1.0) {
        float speed = profiler.animationFactor*20.0;
        if(menu.current == inGame) {
            optionsState.screenBlurFactor -= min(optionsState.screenBlurFactor*speed, speed);
            if(optionsState.screenBlurFactor < 0.01) optionsState.screenBlurFactor = 0.0;
        }else
            optionsState.screenBlurFactor += min((10-optionsState.screenBlurFactor)*speed, speed);
    }
    
    switch(current) {
        case loading: {
            loadingScreen -= profiler.animationFactor * ((keyState[SDL_SCANCODE_RETURN]) ? 10.0 : 1.0);
            GUIProgressBar* progressBar = static_cast<GUIProgressBar*>(screenView->children[1]);
            progressBar->value = 1.0-loadingScreen/loadingScreenTime;
            if(loadingScreen <= 0.0)
                setMainMenu();
        } break;
        case main:
            for(unsigned int x = 0; x < matrixEasterEgg_X; x ++) {
                if(rand()%100 > 25) continue;
                for(unsigned int y = matrixEasterEgg_Y-1; y > 0; y --) {
                    GUILabel *above = reinterpret_cast<GUILabel*>(screenView->children[(y-1)*matrixEasterEgg_X+x]),
                    *label = reinterpret_cast<GUILabel*>(screenView->children[y*matrixEasterEgg_X+x]);
                    label->text = above->text;
                    label->color = above->color;
                    label->updateContent();
                }
                GUILabel* label = reinterpret_cast<GUILabel*>(screenView->children[x]);
                short character = 12353+(rand()%180);
                char buffer[4];
                buffer[0] = 0xE0 | (0x0F & (character >> 12));
                buffer[1] = 0x80 | (0x3F & (character >> 6));
                buffer[2] = 0x80 | (0x3F & character);
                buffer[3] = 0;
                label->text = buffer;
                if(rand()%10 > 8)
                    label->color = Color4(0.0, 1.0, 0.0, 1.0);
                else
                    label->color.g *= 0.85;
                label->updateContent();
            }
            break;
        case inGame:
            if(mouseFixed) {
                mouseX = optionsState.mouseSmoothing*mouseVelocityX;
                mouseY = optionsState.mouseSmoothing*mouseVelocityY;
                mouseVelocityX -= mouseX;
                mouseVelocityY -= mouseY;
                SDL_ShowCursor(0);
                SDL_WarpMouseInWindow(mainWindow, optionsState.videoWidth >> 1, optionsState.videoHeight >> 1);
            }else{
                mouseVelocityX = 0.0;
                mouseVelocityY = 0.0;
                SDL_ShowCursor(1);
            }
        case console:
        {
            GUIView* view = static_cast<GUIView*>(screenView->children[0]);
            int posY = view->height;
            for(int i = 0; i < consoleMessages.size(); i ++) {
                GUILabel* label;
                consoleMessages[i].timeLeft -= profiler.animationFactor;
                if(consoleMessages[i].timeLeft < 0.0) {
                    if(i < view->children.size())
                        view->deleteChild(i);
                    consoleMessages.erase(consoleMessages.begin()+i);
                    i --;
                    continue;
                }
                if(i < view->children.size()) {
                    label = static_cast<GUILabel*>(view->children[i]);
                    label->color.a = fmin(1, consoleMessages[i].timeLeft);
                    float fallSpeed = posY-label->height;
                    fallSpeed = (fallSpeed-label->posY)*fmin(profiler.animationFactor*5.0, 0.5);
                    label->posY += (fallSpeed > 0.0 && fallSpeed < 1.0) ? 1.0 : fallSpeed;
                    posY -= label->height*2+label->fontHeight*0.2;
                    if(label->text == consoleMessages[i].message)
                        continue;
                }else{
                    label = new GUILabel();
                    view->addChild(label);
                }
                label->width = view->width;
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                label->color = Color4(1.0);
                label->text = consoleMessages[i].message;
                label->updateContent();
                label->posX = 0.0;
                label->posY = posY-label->height;
                posY -= label->height*2+label->fontHeight*0.2;
            }
            
            for(int i = consoleMessages.size(); i < view->children.size(); i ++)
                view->deleteChild(i);
        } break;
        default:
            break;
    }
}

void Menu::sendPauseEvent() {
    if(current != Menu::inGame) {
        SDL_ShowCursor(1);
        SDL_WarpMouseInWindow(mainWindow, optionsState.videoWidth >> 1, optionsState.videoHeight >> 1);
    }
    scriptManager->mainScript->callFunction("onpause", false, 0, NULL);
}

void Menu::setModalView(const std::string& title, const std::string& text, std::function<void(GUIButton* button)> onContinue) {
    if(!onContinue)
        log(warning_log, text);
    
    if(!menu.screenView || menu.screenView->modalView) return;
    
    GUIFramedView* modalView = new GUIFramedView();
    modalView->width = menu.screenView->width*0.4;
    modalView->height = menu.screenView->height*0.4;
    GUILabel* label = new GUILabel();
    //label->color = Color4(1, 0, 0);
    label->posY = modalView->height*0.65;
    label->text = fileManager.localizeString(title);
    label->fontHeight = menu.screenView->height*0.14;
    modalView->addChild(label);
    label = new GUILabel();
    label->width = modalView->width*0.9;
    label->text = text;
    label->sizeAlignment = GUISizeAlignment::Height;
    label->fontHeight = menu.screenView->height*0.09;
    modalView->addChild(label);
    
    GUIButton* button = new GUIButton();
    if(onContinue) {
        button->posX = modalView->width*-0.55;
        button->posY = modalView->height*-0.7;
        button->width = screenView->width*0.14;
        button->sizeAlignment = GUISizeAlignment::Height;
        modalView->addChild(button);
        label = new GUILabel();
        label->text = fileManager.localizeString("cancel");
        label->fontHeight = screenView->height*0.1;
        button->addChild(label);
        button->onClick = [](GUIButton* button) {
            menu.screenView->setModalView(NULL);
        };
        
        button = new GUIButton();
        button->posX = modalView->width*0.55;
        button->type = GUIButton::Type::Delete;
        button->onClick = onContinue;
    }else{
        button->onClick = [](GUIButton* button) {
            menu.screenView->setModalView(NULL);
        };
    }
    button->posY = modalView->height*-0.7;
    button->width = screenView->width*0.14;
    button->sizeAlignment = GUISizeAlignment::Height;
    modalView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("ok");
    label->fontHeight = screenView->height*0.1;
    button->addChild(label);
    
    screenView->setModalView(modalView);
}

void Menu::clear() {
    if(screenView)
        delete screenView;
    screenView = new GUIScreenView();
    current = none;
}

void Menu::clearAndAddBackground() {
    clear();
    
    GUIImage* image = new GUIImage();
    image->texture = fileManager.getResourceByPath<Texture>(NULL, "Core/background.png");
    image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
    if((float)image->texture->width/image->texture->height <= (float)screenView->width/screenView->height) {
        image->sizeAlignment = GUISizeAlignment::Height;
        image->width = screenView->width;
    }else{
        image->sizeAlignment = GUISizeAlignment::Width;
        image->height = screenView->height;
    }
    image->updateContent();
    screenView->addChild(image);
    screenView->updateContent();
}

void Menu::setLoadingMenu() {
    clear();
    current = loading;
    
    GUIImage* image = new GUIImage();
    image->sizeAlignment = GUISizeAlignment::Height;
    image->texture = fileManager.getResourceByPath<Texture>(NULL, "Core/logo.png");
    image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
    image->width = screenView->width*0.8;
    image->posY = screenView->height*0.2;
    screenView->addChild(image);
    
    GUIProgressBar* progressBar = new GUIProgressBar();
    progressBar->posY = screenView->height*-0.7;
    screenView->addChild(progressBar);
    
    GUILabel* label = new GUILabel();
    label->text = fileManager.localizeString("loading");
    label->posY = screenView->height*-0.5;
    label->fontHeight = screenView->height*0.16;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    screenView->updateContent();
}

void Menu::setMainMenu() {
    time_t t = time(0);
    struct tm* date = localtime(&t);
    if(date->tm_mon == 2 && date->tm_mday == 31) {
        clear();
        matrixEasterEgg_X = matrixEasterEgg_Y = 20;
        for(unsigned int y = 0; y < matrixEasterEgg_Y; y ++)
            for(unsigned int x = 0; x < matrixEasterEgg_X; x ++) {
                GUILabel* label = new GUILabel();
                label->posX = screenView->width*((float)x/matrixEasterEgg_X*2.0-0.95);
                label->posY = screenView->height*(0.9-(float)y/matrixEasterEgg_Y*2.0);
                label->width = screenView->width*0.1;
                label->fontHeight = label->height = screenView->height*0.1;
                label->textAlignment = GUILabel::TextAlignment::Center;
                label->sizeAlignment = GUISizeAlignment::None;
                screenView->addChild(label);
            }
    }else
        clearAndAddBackground();
    current = main;
    
    GUIFramedView* view = new GUIFramedView();
    view->width = screenView->width*0.2;
    view->height = screenView->height*0.42;
    view->posX = screenView->width*-0.72;
    screenView->addChild(view);
    
    std::function<void(GUIButton*)> onClick[] = {
        [](GUIButton* button) {
			menu.setSaveGamesMenu();
        }, [](GUIButton* button) {
			menu.setMultiplayerMenu();
        }, [](GUIButton* button) {
			menu.setOptionsMenu();
        }, [](GUIButton* button) {
			menu.setCreditsMenu();
        }, [](GUIButton* button) {
            AppTerminate();
        }
    };
    const char* buttonLabels[] = { "saveGames", "multiplayer", "options", "credits", "quitGame" };
    for(unsigned char i = 0; i < 5; i ++) {
        GUIButton* button = new GUIButton();
        button->posY = screenView->height*(0.32-0.16*i);
        button->onClick = onClick[i];
        //if(i == 1) button->enabled = false;
        view->addChild(button);
        GUILabel* label = new GUILabel();
        label->text = fileManager.localizeString(buttonLabels[i]);
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->fontHeight = screenView->height*0.1;
        label->sizeAlignment = GUISizeAlignment::Height;
        label->width = view->width-view->content.cornerRadius*1.1-button->paddingX;
        button->addChild(label);
    }
    
    GUILabel* label = new GUILabel();
    label->text = fileManager.localizeString("version")+": "+engineVersion+"\nOpen Beta";
    label->textAlignment = GUILabel::TextAlignment::Left;
    label->sizeAlignment = GUISizeAlignment::Height;
    label->width = screenView->width*0.4;
    label->posX = screenView->width*-0.59;
    label->posY = screenView->height*-0.92;
    label->fontHeight = screenView->height*0.08;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    screenView->updateContent();
}

void Menu::setCreditsMenu() {
    clearAndAddBackground();
    current = credits;
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("credits");
    label->fontHeight = screenView->height*0.2;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    GUIFramedView* view = new GUIFramedView();
    view->width = screenView->width*0.7;
    view->height = screenView->height*0.6;
    view->content.edgeGradientBorder = 0.5;
    screenView->addChild(view);
    
    label = new GUILabel();
    label->text = std::string("Programmer: Alexander Meißner\n© 2014, Gamefortec");
    label->posY = view->height*0.6;
    label->width = screenView->width*0.8;
    label->fontHeight = screenView->height*0.1;
    label->sizeAlignment = GUISizeAlignment::Height;
    view->addChild(label);
    
    GUIButton* button = new GUIButton();
    button->posY = screenView->height*0.14;
    button->onClick = [](GUIButton* button) {
        openExternURL(homepage);
    };
    view->addChild(button);
    label = new GUILabel();
    label->text = homepage;
    label->fontHeight = screenView->height*0.1;
    label->sizeAlignment = GUISizeAlignment::All;
    button->addChild(label);
    
    label = new GUILabel();
    label->text = std::string("Using libraries:\nZlib License: SDL2, Bullet Physics\nBSD (3-Clause) License:\nOgg / Vorbis: © 2014, Xiph.Org Foundation\nWebKit JSC: © 2014, Apple, Inc.");
    label->posY = -view->height*0.5;
    label->width = screenView->width*0.8;
    label->fontHeight = screenView->height*0.1;
    label->sizeAlignment = GUISizeAlignment::Height;
    view->addChild(label);
    
    button = new GUIButton();
    button->posY = screenView->height*-0.8;
    button->onClick = [](GUIButton* button) {
		menu.setMainMenu();
    };
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("return");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.14;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    
    screenView->updateContent();
}

void Menu::setInGameMenu() {
    clear();
    current = inGame;
    
    GUIView* view = new GUIView();
    view->posX = screenView->width*-0.5;
    view->posY = screenView->height*0.3;
    view->width = screenView->width*0.48;
    view->height = screenView->height*0.68;
    screenView->addChild(view);
    
    screenView->updateContent();
}

void Menu::setConsoleMenu() {
    setInGameMenu();
    current = console;
    
    GUITextField* textField = new GUITextField();
    textField->posY = screenView->height*-0.8;
    textField->width = screenView->width*0.9;
    textField->height = screenView->height*0.07;
    screenView->addChild(textField);
    textField->setFocused(true);
    
    screenView->updateContent();
}

void Menu::setGameEscMenu() {
    clear();
    current = gameEsc;
    
    std::function<void(GUIButton*)> onClick[] = {
        [](GUIButton* button) {
			menu.setInGameMenu();
			menu.sendPauseEvent();
        }, [](GUIButton* button) {
			menu.setOptionsMenu();
        }, [](GUIButton* button) {
            levelManager.clear();
            menu.setMainMenu();
        }, [](GUIButton* button) {
            levelManager.clear();
            AppTerminate();
        }
    };
    const char* buttonLabels[] = { "return", "options", "mainMenu", "quitGame" };
    for(unsigned char i = 0; i < 4; i ++) {
        GUIButton* button = new GUIButton();
        button->posY = screenView->height*(0.32-0.16*i);
        GUILabel* label = new GUILabel();
        label->text = fileManager.localizeString(buttonLabels[i]);
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->fontHeight = screenView->height*0.1;
        label->width = screenView->width*0.14;
        label->sizeAlignment = GUISizeAlignment::Height;
        button->addChild(label);
        button->onClick = onClick[i];
        screenView->addChild(button);
    }
    
    screenView->updateContent();
}

Menu menu;