//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"
#include "AppMain.h"

void handleMenuKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        switch(currentMenu) {
            case loadingMenu:
            case mainMenu:
                AppTerminate();
                break;
            case optionsMenu:
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((levelManager.gameStatus == noGame) ? mainMenu : gameEscMenu);
                break;
            case languagesMenu:
                setMenu(optionsMenu);
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
            case newGameMenu:
                setMenu(saveGamesMenu);
                break;
            case removeGameMenu:
                currentScreenView->setModalView(NULL);
                currentMenu = saveGamesMenu;
                break;
        }
    }
}

void setMenu(MenuName menu) {
    currentMenu = menu;
    if(currentScreenView) delete currentScreenView;
    currentScreenView = new GUIScreenView();
    
    if(levelManager.gameStatus == noGame && menu != loadingMenu) {
        GUIImage* image = new GUIImage();
        image->texture = fileManager.getPackage("Default")->getResource<Texture>("background.png");
        image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
        image->sizeAlignment = GUISizeAlignment_Height;
        image->width = currentScreenView->width;
        image->updateContent();
        currentScreenView->addChild(image);
    }
    
    switch(menu) {
        case loadingMenu: {
            GUIProgressBar* progressBar = new GUIProgressBar();
            progressBar->posY = currentScreenView->height*-0.7;
            currentScreenView->addChild(progressBar);
            
            GUIImage* image = new GUIImage();
            image->sizeAlignment = GUISizeAlignment_Height;
            image->texture = fileManager.getPackage("Default")->getResource<Texture>("logo.png");
            image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
            image->width = currentScreenView->width*0.8;
            image->posY = currentScreenView->height*0.2;
            currentScreenView->addChild(image);
            
            GUILabel* label = new GUILabel();
            label->text = localization.localizeString("loading");
            label->posY = currentScreenView->height*-0.5;
            label->fontHeight = currentScreenView->height*0.16;
            currentScreenView->addChild(label);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScreenView->draw();
            SDL_GL_SwapBuffers();
        } break;
        case mainMenu: {
            GUIFramedView* view = new GUIFramedView();
            view->width = currentScreenView->width*0.2;
            view->height = currentScreenView->width*0.27;
            view->posX = currentScreenView->width*-0.72;
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
                button->posY = currentScreenView->height*(0.32-0.16*i);
                if(i == 1) button->state = GUIButtonStateDisabled; //Multiplayer is not implemented yet
                view->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = currentScreenView->height*0.1;
                label->width = currentScreenView->width*0.16;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case optionsMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("options");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = currentScreenView->height*-0.8;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((levelManager.gameStatus == noGame) ? mainMenu : gameEscMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            label = new GUILabel();
            label->posX = currentScreenView->width*-0.52;
            label->posY = currentScreenView->height*0.72;
            label->text = localization.localizeString("graphics");
            label->fontHeight = currentScreenView->height*0.14;
            currentScreenView->addChild(label);
            GUIFramedView* view = new GUIFramedView();
            view->width = currentScreenView->width*0.42;
            view->height = currentScreenView->height*0.62;
            view->posX = currentScreenView->width*-0.52;
            currentScreenView->addChild(view);
            
            std::function<void(GUICheckBox*)> onClick[] = {
                [](GUICheckBox* checkBox) {
                    fullScreenEnabled = (checkBox->state == GUIButtonStatePressed);
                }, [](GUICheckBox* checkBox) {
                    edgeSmoothEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    vSyncEnabled = (checkBox->state == GUIButtonStatePressed);
                }, [](GUICheckBox* checkBox) {
                    cubemapsEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    screenBlurFactor = (checkBox->state == GUIButtonStatePressed) ? 0.0 : -1.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            bool checkBoxActive[] = { fullScreenEnabled, edgeSmoothEnabled, vSyncEnabled, cubemapsEnabled, (screenBlurFactor > -1.0) };
            const char* checkBoxLabels[] = { "fullScreenEnabled", "edgeSmoothEnabled", "vSyncEnabled", "cubemapsEnabled", "screenBlurEnabled" };
            for(unsigned char i = 0; i < 5; i ++) {
                label = new GUILabel();
                label->posX = view->width*0.45;
                label->posY = currentScreenView->height*(0.54-0.12*i);
                label->width = view->width*0.5;
                label->fontHeight = currentScreenView->height*0.1;
                label->text = localization.localizeString(checkBoxLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUICheckBox* checkBox = new GUICheckBox();
                checkBox->posX = view->width*-0.52;
                checkBox->posY = label->posY;
                checkBox->onClick = onClick[i];
                if(checkBoxActive[i]) checkBox->state = GUIButtonStatePressed;
                view->addChild(checkBox);
            }
            
            unsigned int sliderSteps[] = { 3, 3, 4, 4, 3 };
            std::function<void(GUISlider*)> onChange[] = {
                [](GUISlider* slider) {
                    depthOfFieldQuality = slider->value*3.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    bumpMappingQuality = slider->value*3.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    shadowQuality = slider->value*4.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    ssaoQuality = slider->value*4.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    blendingQuality = slider->value*3.0;
                    if(levelManager.gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            unsigned char sliderValues[] = { depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, blendingQuality };
            const char* sliderLabels[] = { "depthOfFieldQuality", "bumpMappingQuality", "shadowQuality", "ssaoQuality", "blendingQuality" };
            for(unsigned char i = 0; i < 5; i ++) {
                label = new GUILabel();
                label->posX = view->width*0.45;
                label->posY = currentScreenView->height*(-0.06-0.12*i);
                label->width = view->width*0.5;
                label->fontHeight = currentScreenView->height*0.1;
                label->text = localization.localizeString(sliderLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUISlider* slider = new GUISlider();
                slider->posX = view->width*-0.52;
                slider->posY = label->posY;
                slider->width = view->width*0.4;
                slider->value = (float)sliderValues[i]/(float)sliderSteps[i];
                slider->steps = sliderSteps[i];
                slider->onChange = onChange[i];
                view->addChild(slider);
            }
            
            label = new GUILabel();
            label->posX = currentScreenView->width*0.52;
            label->posY = currentScreenView->height*0.72;
            label->text = localization.localizeString("sound");
            label->fontHeight = currentScreenView->height*0.14;
            currentScreenView->addChild(label);
            view = new GUIFramedView();
            view->width = currentScreenView->width*0.42;
            view->height = currentScreenView->height*0.16;
            view->posX = currentScreenView->width*0.52;
            view->posY = currentScreenView->height*0.46;
            currentScreenView->addChild(view);
            std::function<void(GUISlider*)> onChangeSound[] = {
                [](GUISlider* slider) {
                    globalVolume = slider->value;
                }, [](GUISlider* slider) {
                    musicVolume = slider->value;
                }
            };
            float sliderValuesSound[] = { globalVolume, musicVolume };
            const char* sliderLabelsSound[] = { "soundGlobal", "soundMusic" };
            for(unsigned char i = 0; i < 2; i ++) {
                label = new GUILabel();
                label->posX = view->width*-0.4;
                label->posY = currentScreenView->height*(0.06-0.12*i);
                label->width = view->width*0.5;
                label->fontHeight = currentScreenView->height*0.1;
                label->text = localization.localizeString(sliderLabelsSound[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUISlider* slider = new GUISlider();
                slider->posX = view->width*0.4;
                slider->posY = label->posY;
                slider->width = view->width*0.5;
                slider->value = sliderValuesSound[i];
                slider->onChange = onChangeSound[i];
                view->addChild(slider);
            }
            
            button = new GUIButton();
            button->posX = currentScreenView->width*0.52;
            button->posY = currentScreenView->height*-0.1;
            button->onClick = [](GUIButton* button) {
                setMenu(languagesMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("language");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.2;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
        } break;
        case languagesMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("language");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = currentScreenView->height*-0.8;
            button->onClick = [](GUIButton* button) {
                setMenu(optionsMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            std::vector<std::string> languages;
            localization.getLocalizableLanguages(languages);
            GUITabs* tabs = new GUITabs();
            tabs->deactivatable = false;
            tabs->width = currentScreenView->width*0.2;
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
                label->fontHeight = currentScreenView->height*0.06;
                button->addChild(label);
                tabs->addChild(button);
                if(languages[i] == localization.selected) {
                    tabs->selectedIndex = i;
                    tabs->updateContent();
                }
            }
        } break;
        case creditsMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("credits");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
            
            GUIButton* button = new GUIButton();
            button->posX = currentScreenView->width*-0.4;
            button->posY = currentScreenView->height*-0.8;
            button->onClick = [](GUIButton* button) {
                setMenu(mainMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            button = new GUIButton();
            button->posX = currentScreenView->width*0.4;
            button->posY = currentScreenView->height*-0.8;
            button->onClick = [](GUIButton* button) {
                openExternURL("http://gamefortec.net/");
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = "http://gamefortec.net/";
            label->fontHeight = currentScreenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment_All;
            button->addChild(label);
            
            label = new GUILabel();
            label->text = std::string("Programming: Alexander Meißner\nMental Support: Noah Hummel");
            label->width = currentScreenView->width*0.8;
            label->fontHeight = currentScreenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment_Height;
            currentScreenView->addChild(label);
        } break;
        case inGameMenu: {
            GUILabel* label = new GUILabel();
            label->text = std::string("FPS: --");
            label->posX = currentScreenView->width*-0.77;
            label->posY = currentScreenView->height*0.95;
            label->width = currentScreenView->width*0.2;
            label->color = Color4(1.0);
            label->textAlign = GUITextAlign_Left;
            label->sizeAlignment = GUISizeAlignment_Height;
            currentScreenView->addChild(label);
            
            label = new GUILabel();
            label->text = std::string("Collisions: --");
            label->posX = currentScreenView->width*-0.67;
            label->posY = currentScreenView->height*0.90;
            label->width = currentScreenView->width*0.3;
            label->color = Color4(1.0);
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
                    levelManager.leaveGame();
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "back", "options", "mainMenu", "quitGame" };
            for(unsigned char i = 0; i < 4; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = currentScreenView->height*(0.32-0.16*i);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = currentScreenView->height*0.1;
                label->width = currentScreenView->width*0.14;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
                currentScreenView->addChild(button);
            }
        } break;
        case saveGamesMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("saveGames");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
            GUIScrollView* scrollView = new GUIScrollView();
            scrollView->width = currentScreenView->width*0.95;
            scrollView->height = currentScreenView->height*0.7;
            scrollView->posY = currentScreenView->height*0.02;
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
                label->fontHeight = currentScreenView->height*0.1;
                label->width = scrollView->width*0.6;
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = [&menu, name](GUIButton* button) {
                    levelManager.loadGame(name);
                };
                
                button = new GUIButton();
                button->posX = scrollView->width*0.75;
                button->posY = scrollView->height*(0.9-i*0.25);
                button->width = scrollView->width*0.2;
                button->sizeAlignment = GUISizeAlignment_Height;
                scrollView->addChild(button);
                label = new GUILabel();
                label->text = localization.localizeString("removeGame");
                label->fontHeight = currentScreenView->height*0.1;
                button->addChild(label);
                button->onClick = [&menu, name](GUIButton* button) {
                    currentMenu = removeGameMenu;
                    GUIFramedView* modalView = new GUIFramedView();
                    modalView->width = currentScreenView->width*0.4;
                    modalView->height = currentScreenView->height*0.4;
                    GUILabel* label = new GUILabel();
                    label->posY = modalView->height*0.75;
                    label->text = localization.localizeString("removeGame");
                    label->fontHeight = currentScreenView->height*0.14;
                    modalView->addChild(label);
                    label = new GUILabel();
                    label->width = modalView->width*0.9;
                    label->textAlign = GUITextAlign_Left;
                    label->sizeAlignment = GUISizeAlignment_Height;
                    label->text = localization.localizeString("name")+": "+name;
                    label->fontHeight = currentScreenView->height*0.14;
                    modalView->addChild(label);
                    button = new GUIButton();
                    button->posX = modalView->width*-0.55;
                    button->posY = modalView->height*-0.75;
                    button->width = currentScreenView->width*0.14;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("cancel");
                    label->fontHeight = currentScreenView->height*0.1;
                    button->addChild(label);
                    button->onClick = [&menu](GUIButton* button) {
                        currentScreenView->setModalView(NULL);
                        currentMenu = saveGamesMenu;
                    };
                    button = new GUIButton();
                    button->posX = modalView->width*0.55;
                    button->posY = modalView->height*-0.75;
                    button->width = currentScreenView->width*0.14;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("ok");
                    label->fontHeight = currentScreenView->height*0.1;
                    button->addChild(label);
                    button->onClick = [&menu, name](GUIButton* button) {
                        levelManager.removeGame(name);
                        setMenu(saveGamesMenu);
                    };
                    currentScreenView->setModalView(modalView);
                };
            }
            scrollView->scrollHeight = (files.size()*0.25-1.0)*scrollView->height;
            
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    setMenu(mainMenu);
                }, [](GUIButton* button) {
                    setMenu(newGameMenu);
                }
            };
            const char* buttonLabels[] = { "back", "newGame" };
            for(unsigned char i = 0; i < 2; i ++) {
                GUIButton* button = new GUIButton();
                button->posX = currentScreenView->width*(-0.8+i*1.6);
                button->posY = currentScreenView->height*-0.8;
                button->width = currentScreenView->width*0.14;
                button->sizeAlignment = GUISizeAlignment_Height;
                currentScreenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->fontHeight = currentScreenView->height*0.1;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case newGameMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("newGame");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
            GUIScrollView* scrollView = new GUIScrollView();
            scrollView->posX = 0.0;
            scrollView->posY = currentScreenView->height*0.1;
            scrollView->width = currentScreenView->width*0.9;
            scrollView->height = currentScreenView->height*0.7;
            currentScreenView->addChild(scrollView);
            GUITabs* buttonList = new GUITabs();
            buttonList->deactivatable = false;
            buttonList->orientation = GUIOrientation_Vertical;
            GUIButton* button;
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
                label->fontHeight = currentScreenView->height*0.1;
                button->addChild(label);
                buttonList->addChild(button);
            }
            buttonList->posY = fmax((buttonList->height-scrollView->height), 0.0);
            scrollView->scrollWidth = 0;
            scrollView->scrollHeight = buttonList->height+buttonList->posY;
            scrollView->addChild(buttonList);
            
            GUITextField* textField = new GUITextField();
            textField->posX = 0.0;
            textField->posY = currentScreenView->height*-0.8;
            textField->width = currentScreenView->width*0.4;
            textField->height = currentScreenView->height*0.07;
            textField->label->fontHeight = currentScreenView->height*0.1;
            textField->label->text = localization.localizeString("newGame");
            currentScreenView->addChild(textField);
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    setMenu(saveGamesMenu);
                }, [&menu, files, buttonList, textField](GUIButton* button) {
                    levelManager.newGame(files[buttonList->selectedIndex], textField->label->text);
                }
            };
            const char* buttonLabels[] = { "cancel", "ok" };
            for(unsigned char i = 0; i < 2; i ++) {
                button = new GUIButton();
                button->posX = currentScreenView->width*(-0.8+i*1.6);
                button->posY = currentScreenView->height*-0.8;
                button->width = currentScreenView->width*0.14;
                button->sizeAlignment = GUISizeAlignment_Height;
                currentScreenView->addChild(button);
                label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->fontHeight = currentScreenView->height*0.1;
                button->addChild(label);
                button->onClick = onClick[i];
            }
            button->state = GUIButtonStateDisabled;
            textField->onChange = [button](GUITextField* textField) {
                std::string path = gameDataDir+"Saves/"+textField->label->text+'/';
                button->state = (!checkDir(path) && getUTF8Length(textField->label->text.c_str()) > 3) ? GUIButtonStateNormal : GUIButtonStateDisabled;
                button->updateContent();
            };
        } break;
        default:
            log(error_log, "Tried to call setMenu() with invalid menu id.");
            break;
    }
    
    currentScreenView->updateContent();
}

MenuName currentMenu;