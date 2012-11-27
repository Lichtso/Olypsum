//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Menu.h"
#import "AppMain.h"

void handleMenuKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        switch(currentMenu) {
            case mainMenu:
                AppTerminate();
                break;
            case optionsMenu:
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((worldManager.gameStatus == noGame) ? mainMenu : gameEscMenu);
                break;
            case creditsMenu:
            case saveGamesMenu:
                setMenu(mainMenu);
                break;
            case inGameMenu:
                setMenu(gameEscMenu);
                break;
            case gameEscMenu:
                setMenu(inGameMenu);
                break;
            default:
            //case loadingMenu:
            //case languagesMenu:
            //case newGameMenu:
            //case removeGameMenu:
                //TODO
                break;
        }
    }
}

void setMenu(MenuName menu) {
    currentMenu = menu;
    if(currentScreenView) delete currentScreenView;
    currentScreenView = new GUIScreenView();
    
    if(worldManager.gameStatus == noGame) {
        GUIImage* image = new GUIImage();
        image->texture = fileManager.getPackage("Default")->getResource<Texture>("background.png");
        image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
        
        image->sizeAlignment = GUISizeAlignment_Height;
        image->width = screenSize[0]*0.5;
        image->updateContent();
        currentScreenView->addChild(image);
    }
    
    switch(menu) {
        case loadingMenu: {
            GUIImage* image = new GUIImage();
            image->texture = fileManager.getPackage("Default")->getResource<Texture>("logo.png");
            image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
            
            image->sizeAlignment = GUISizeAlignment_Height;
            image->width = screenSize[0]*0.4;
            image->posY = screenSize[1]*0.1;
            currentScreenView->addChild(image);
            GUILabel* label = new GUILabel();
            label->text = localization.localizeString("loading");
            label->posY = screenSize[1]*-0.1;
            label->fontHeight = screenSize[1]*0.08;
            label->color = Color4(1.0);
            currentScreenView->addChild(label);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScreenView->draw();
            SDL_GL_SwapBuffers();
        } break;
        case mainMenu: {
            GUIFramedView* view = new GUIFramedView();
            view->width = screenSize[0]*0.1;
            view->height = screenSize[0]*0.135;
            view->posX = screenSize[0]*-0.36;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    setMenu(saveGamesMenu);
                }, [](GUIButton* button) {
                    
                }, [](GUIButton* button) {
                    setMenu(optionsMenu);
                }, [](GUIButton* button) {
                    setMenu(creditsMenu);
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "saveGames", "multiplayer", "options", "credits", "quitGame" };
            for(unsigned char i = 0; i < 5; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = screenSize[1]*(0.16-0.08*i);
                view->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = screenSize[1]*0.05;
                label->width = screenSize[0]*0.08;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case optionsMenu: {
            GUILabel* label = new GUILabel();
            label->posY = screenSize[1]*0.44;
            label->text = localization.localizeString("options");
            label->fontHeight = screenSize[1]*0.1;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = screenSize[1]*-0.37;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((worldManager.gameStatus == noGame) ? mainMenu : gameEscMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = screenSize[1]*0.05;
            label->width = screenSize[0]*0.07;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            label = new GUILabel();
            label->posX = screenSize[0]*-0.26;
            label->posY = screenSize[1]*0.36;
            label->text = localization.localizeString("graphics");
            label->fontHeight = screenSize[1]*0.07;
            currentScreenView->addChild(label);
            GUIFramedView* view = new GUIFramedView();
            view->width = screenSize[0]*0.21;
            view->height = screenSize[1]*0.31;
            view->posX = screenSize[0]*-0.26;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            
            std::function<void(GUICheckBox*)> onClick[] = {
                [](GUICheckBox* checkBox) {
                    fullScreenEnabled = (checkBox->state == GUIButtonStatePressed);
                }, [](GUICheckBox* checkBox) {
                    edgeSmoothEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    cubemapsEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    screenBlurFactor = (checkBox->state == GUIButtonStatePressed) ? 0.0 : -1.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            bool checkBoxActive[] = { fullScreenEnabled, edgeSmoothEnabled, cubemapsEnabled, (screenBlurFactor > -1.0) };
            const char* checkBoxLabels[] = { "fullScreenEnabled", "edgeSmoothEnabled", "cubemapsEnabled", "screenBlurEnabled" };
            for(unsigned char i = 0; i < 4; i ++) {
                label = new GUILabel();
                label->posX = screenSize[0]*0.12;
                label->posY = screenSize[1]*(0.27-0.06*i);
                label->width = screenSize[0]*0.15;
                label->fontHeight = screenSize[1]*0.05;
                label->text = localization.localizeString(checkBoxLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUICheckBox* checkBox = new GUICheckBox();
                checkBox->posX = screenSize[0]*-0.12;
                checkBox->posY = label->posY;
                checkBox->onClick = onClick[i];
                if(checkBoxActive[i]) checkBox->state = GUIButtonStatePressed;
                view->addChild(checkBox);
            }
            
            unsigned int sliderSteps[] = { 3, 3, 4, 5, 3 };
            std::function<void(GUISlider*)> onChange[] = {
                [](GUISlider* slider) {
                    depthOfFieldQuality = slider->value*3.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    bumpMappingQuality = slider->value*3.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    shadowQuality = slider->value*4.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    ssaoQuality = slider->value*5.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    blendingQuality = slider->value*3.0;
                    if(worldManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            unsigned char sliderValues[] = { depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, blendingQuality };
            const char* sliderLabels[] = { "depthOfFieldQuality", "bumpMappingQuality", "shadowQuality", "ssaoQuality", "blendingQuality", "particleCalcTarget" };
            for(unsigned char i = 0; i < 6; i ++) {
                label = new GUILabel();
                label->posX = screenSize[0]*0.12;
                label->posY = screenSize[1]*(0.03-0.06*i);
                label->width = screenSize[0]*0.15;
                label->fontHeight = screenSize[1]*0.05;
                label->text = localization.localizeString(sliderLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                if(i == 5) break;
                GUISlider* slider = new GUISlider();
                slider->posX = screenSize[0]*-0.12;
                slider->posY = label->posY;
                slider->width = screenSize[0]*0.08;
                slider->value = (float)sliderValues[i]/(float)sliderSteps[i];
                slider->steps = sliderSteps[i];
                slider->onChange = onChange[i];
                view->addChild(slider);
            }
            
            GUITabs* tabs = new GUITabs();
            tabs->deactivatable = false;
            tabs->posX = screenSize[0]*-0.12;
            tabs->posY = screenSize[1]*-0.27;
            tabs->width = screenSize[0]*0.08;
            tabs->sizeAlignment = GUISizeAlignment_Height;
            tabs->selectedIndex = particleCalcTarget;
            tabs->orientation = GUIOrientation_Horizontal;
            tabs->onChange = [](GUITabs* tabs) {
                particleCalcTarget = tabs->selectedIndex;
            };
            view->addChild(tabs);
            
            const char* buttonLabels[] = { "off", "cpu", "gpu" };
            for(unsigned char i = 0; i < 3; i ++) {
                GUIButton* button = new GUIButton();
                label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->fontHeight = screenSize[1]*0.03;
                button->addChild(label);
                tabs->addChild(button);
            }
            
            label = new GUILabel();
            label->posX = screenSize[0]*0.26;
            label->posY = screenSize[1]*0.36;
            label->text = localization.localizeString("language");
            label->fontHeight = screenSize[1]*0.07;
            currentScreenView->addChild(label);
            view = new GUIFramedView();
            view->width = screenSize[0]*0.21;
            view->height = screenSize[1]*0.08;
            view->posX = screenSize[0]*0.26;
            view->posY = screenSize[1]*0.23;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            label = new GUILabel();
            label->posY = screenSize[1]*0.04;
            label->width = screenSize[0]*0.15;
            label->fontHeight = screenSize[1]*0.05;
            label->text = std::string(localization.localizeString("selectedLanguage"))+": "+localization.title;
            label->textAlign = GUITextAlign_Left;
            label->sizeAlignment = GUISizeAlignment_Height;
            view->addChild(label);
            button = new GUIButton();
            button->posY = screenSize[1]*-0.03;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                setMenu(languagesMenu);
            };
            view->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("selectLanguage");
            label->fontHeight = screenSize[1]*0.05;
            label->width = screenSize[0]*0.1;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
        } break;
        case languagesMenu: {
            GUILabel* label = new GUILabel();
            label->posY = screenSize[1]*0.44;
            label->text = localization.localizeString("selectLanguage");
            label->fontHeight = screenSize[1]*0.1;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = screenSize[1]*-0.37;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                setMenu(optionsMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = screenSize[1]*0.05;
            label->width = screenSize[0]*0.07;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            std::vector<std::string> languages;
            localization.getLocalizableLanguages(languages);
            GUITabs* tabs = new GUITabs();
            tabs->deactivatable = false;
            tabs->width = screenSize[0]*0.1;
            tabs->sizeAlignment = GUISizeAlignment_Height;
            tabs->orientation = GUIOrientation_Vertical;
            tabs->onChange = [](GUITabs* tabs) {
                std::vector<std::string> languages;
                localization.getLocalizableLanguages(languages);
                localization.strings.clear();
                localization.selected = languages[tabs->selectedIndex];
                localization.loadLocalization(resourcesDir+"Packages/Default/Languages/"+localization.selected+".xml");
            };
            currentScreenView->addChild(tabs);
            for(unsigned char i = 0; i < languages.size(); i ++) {
                GUIButton* button = new GUIButton();
                label = new GUILabel();
                label->text = languages[i].c_str();
                label->fontHeight = screenSize[1]*0.03;
                button->addChild(label);
                tabs->addChild(button);
                if(languages[i] == localization.selected) {
                    tabs->selectedIndex = i;
                    tabs->updateContent();
                }
            }
        } break;
        case creditsMenu:
            
        break;
        case inGameMenu: {
            GUILabel* label = new GUILabel();
            label->text = std::string("FPS: --");
            label->posX = -screenSize[0]*0.9;
            label->posY = screenSize[1]*0.9;
            label->width = screenSize[0]*0.15;
            label->textAlign = GUITextAlign_Left;
            label->sizeAlignment = GUISizeAlignment_Height;
            currentScreenView->addChild(label);
        } break;
        case gameEscMenu: {
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    setMenu(inGameMenu);
                }, [](GUIButton* button) {
                    setMenu(optionsMenu);
                }, [](GUIButton* button) {
                    worldManager.leaveGame();
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "back", "options", "mainMenu", "quitGame" };
            for(unsigned char i = 0; i < 4; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = screenSize[1]*(0.16-0.08*i);
                currentScreenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = screenSize[1]*0.05;
                label->width = screenSize[0]*0.07;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case saveGamesMenu: {
            GUILabel* label = new GUILabel();
            label->posY = screenSize[1]*0.44;
            label->text = localization.localizeString("saveGames");
            label->fontHeight = screenSize[1]*0.1;
            currentScreenView->addChild(label);
            GUIScrollView* scrollView = new GUIScrollView();
            scrollView->width = screenSize[0]*0.475;
            scrollView->height = screenSize[1]*0.35;
            scrollView->posY = screenSize[1]*0.01;
            currentScreenView->addChild(scrollView);
            std::vector<std::string> files;
            scanDir(gameDataDir+"Saves/", files);
            for(unsigned int i = 0; i < files.size(); i ++) {
                if(files[i][files[i].length()-1] != '/') {
                    files.erase(files.begin()+i);
                    i --;
                    continue;
                }
                std::string name = files[i];
                name.pop_back();
                
                GUIButton* button = new GUIButton();
                button->posX = scrollView->width*-0.38;
                button->posY = scrollView->height*(0.9-i*0.25);
                scrollView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = name;
                label->fontHeight = screenSize[1]*0.05;
                label->width = scrollView->width*0.6;
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = [&menu, name](GUIButton* button) {
                    worldManager.loadGame(name);
                };
                
                button = new GUIButton();
                button->posX = scrollView->width*0.75;
                button->posY = scrollView->height*(0.9-i*0.25);
                button->width = scrollView->width*0.2;
                button->sizeAlignment = GUISizeAlignment_Height;
                scrollView->addChild(button);
                label = new GUILabel();
                label->text = localization.localizeString("removeGame");
                label->fontHeight = screenSize[1]*0.05;
                button->addChild(label);
                button->onClick = [&menu, name](GUIButton* button) {
                    menu = newGameMenu;
                    GUIFramedView* modalView = new GUIFramedView();
                    modalView->innerShadow = -8;
                    modalView->width = screenSize[0]*0.2;
                    modalView->height = screenSize[1]*0.2;
                    GUILabel* label = new GUILabel();
                    label->posY = modalView->height*0.75;
                    label->text = localization.localizeString("removeGame");
                    label->fontHeight = screenSize[1]*0.07;
                    modalView->addChild(label);
                    label = new GUILabel();
                    label->width = modalView->width*0.9;
                    label->textAlign = GUITextAlign_Left;
                    label->sizeAlignment = GUISizeAlignment_Height;
                    label->text = localization.localizeString("name")+": "+name;
                    label->fontHeight = screenSize[1]*0.07;
                    modalView->addChild(label);
                    button = new GUIButton();
                    button->posX = modalView->width*-0.55;
                    button->posY = modalView->height*-0.75;
                    button->width = screenSize[0]*0.07;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("cancel");
                    label->fontHeight = screenSize[1]*0.05;
                    button->addChild(label);
                    button->onClick = [&menu](GUIButton* button) {
                        currentScreenView->setModalView(NULL);
                        menu = saveGamesMenu;
                    };
                    button = new GUIButton();
                    button->posX = modalView->width*0.55;
                    button->posY = modalView->height*-0.75;
                    button->width = screenSize[0]*0.07;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("ok");
                    label->fontHeight = screenSize[1]*0.05;
                    button->addChild(label);
                    button->onClick = [&menu, name](GUIButton* button) {
                        worldManager.removeGame(name);
                        setMenu(saveGamesMenu);
                    };
                    currentScreenView->setModalView(modalView);
                };
            }
            scrollView->scrollHeight = (files.size()*0.25-1.0)*scrollView->height;
            
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    setMenu(mainMenu);
                }, [&menu](GUIButton* button) {
                    menu = newGameMenu;
                    GUIFramedView* modalView = new GUIFramedView();
                    modalView->innerShadow = -8;
                    modalView->width = screenSize[0]*0.2;
                    modalView->height = screenSize[1]*0.2;
                    GUILabel* label = new GUILabel();
                    label->posY = modalView->height*0.75;
                    label->text = localization.localizeString("newGame");
                    label->fontHeight = screenSize[1]*0.07;
                    modalView->addChild(label);
                    label = new GUILabel();
                    label->posX = modalView->width*-0.5;
                    label->posY = modalView->height*0.1;
                    label->width = modalView->width*0.4;
                    label->textAlign = GUITextAlign_Left;
                    label->sizeAlignment = GUISizeAlignment_Height;
                    label->text = localization.localizeString("name");
                    label->fontHeight = screenSize[1]*0.07;
                    modalView->addChild(label);
                    label = new GUILabel();
                    label->posX = modalView->width*-0.5;
                    label->posY = modalView->height*-0.3;
                    label->width = modalView->width*0.4;
                    label->textAlign = GUITextAlign_Left;
                    label->sizeAlignment = GUISizeAlignment_Height;
                    label->text = localization.localizeString("package");
                    label->fontHeight = screenSize[1]*0.07;
                    modalView->addChild(label);
                    GUIScrollView* scrollView = new GUIScrollView();
                    scrollView->posX = modalView->width*0.4;
                    scrollView->posY = modalView->height*-0.3;
                    scrollView->width = modalView->width*0.5;
                    scrollView->height = modalView->height*0.25;
                    modalView->addChild(scrollView);
                    GUITabs* buttonList = new GUITabs();
                    buttonList->deactivatable = false;
                    buttonList->orientation = GUIOrientation_Horizontal;
                    std::vector<std::string> files;
                    scanDir(resourcesDir+"Packages/", files);
                    scanDir(gameDataDir+"Packages/", files);
                    for(unsigned int i = 0; i < files.size(); i ++) {
                        if(files[i][files[i].length()-1] != '/' || files[i] == "Default/") {
                            files.erase(files.begin()+i);
                            i --;
                            continue;
                        }
                        files[i].pop_back();
                        std::string name = files[i];
                        button = new GUIButton();
                        label = new GUILabel();
                        label->text = name;
                        label->fontHeight = screenSize[1]*0.05;
                        button->addChild(label);
                        buttonList->addChild(button);
                    }
                    buttonList->posX = fmax((buttonList->width-scrollView->width), 0.0);
                    scrollView->scrollWidth = buttonList->width+buttonList->posX;
                    scrollView->scrollHeight = 0;
                    scrollView->addChild(buttonList);
                    GUITextField* textField = new GUITextField();
                    textField->posX = modalView->width*0.4;
                    textField->posY = modalView->height*0.1;
                    textField->width = modalView->width*0.5;
                    textField->height = modalView->height*0.15;
                    textField->label->fontHeight = screenSize[1]*0.05;
                    modalView->addChild(textField);
                    button = new GUIButton();
                    button->posX = modalView->width*-0.55;
                    button->posY = modalView->height*-0.75;
                    button->width = screenSize[0]*0.07;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("cancel");
                    label->fontHeight = screenSize[1]*0.05;
                    button->addChild(label);
                    button->onClick = [&menu](GUIButton* button) {
                        currentScreenView->setModalView(NULL);
                        menu = saveGamesMenu;
                    };
                    button = new GUIButton();
                    button->posX = modalView->width*0.55;
                    button->posY = modalView->height*-0.75;
                    button->state = GUIButtonStateDisabled;
                    button->width = screenSize[0]*0.07;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    if(files.size() > 0)
                        textField->onChange = [button](GUITextField* textField) {
                            button->state = textField->label->text.size() > 3 ? GUIButtonStateNormal : GUIButtonStateDisabled;
                            button->updateContent();
                        };
                    label = new GUILabel();
                    label->text = localization.localizeString("ok");
                    label->fontHeight = screenSize[1]*0.05;
                    button->addChild(label);
                    button->onClick = [&menu, files, buttonList, textField](GUIButton* button) {
                        worldManager.newGame(files[buttonList->selectedIndex], textField->label->text);
                    };
                    currentScreenView->setModalView(modalView);
                }
            };
            const char* buttonLabels[] = { "back", "newGame" };
            for(unsigned char i = 0; i < 2; i ++) {
                GUIButton* button = new GUIButton();
                button->posX = screenSize[0]*(-0.4+i*0.8);
                button->posY = screenSize[1]*-0.4;
                button->width = screenSize[0]*0.07;
                button->sizeAlignment = GUISizeAlignment_Height;
                currentScreenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->fontHeight = screenSize[1]*0.05;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        default:
            log(error_log, "Tried to call setMenu() with invalid menu id.");
            break;
    }
}

MenuName currentMenu;