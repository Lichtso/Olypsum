//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"
#include "AppMain.h"

OptionsState prevOptionsState;

static void updateOptionBackButton(GUIButton* button) {
    if((optionsState.fullScreenEnabled != prevOptionsState.fullScreenEnabled ||
        optionsState.vSyncEnabled != prevOptionsState.vSyncEnabled)) {
        button->buttonType = GUIButtonTypeDelete;
        static_cast<GUILabel*>(button->children[0])->text = localization.localizeString("restart");
    }else{
        button->buttonType = GUIButtonTypeNormal;
        static_cast<GUILabel*>(button->children[0])->text = localization.localizeString("back");
    }
}

static void updateGraphicOptions() {
    if(levelManager.gameStatus == noGame) return;
    loadDynamicShaderPrograms();
    if(prevOptionsState.cubemapsEnabled == optionsState.cubemapsEnabled &&
       optionsState.shadowQuality > 0) return;
    prevOptionsState.cubemapsEnabled = optionsState.cubemapsEnabled;
    for(auto lightObject : objectManager.lightObjects)
        lightObject->deleteShadowMap();
}

static void leaveOptionsMenu(GUIButton* button) {
    optionsState.saveOptions();
    if(optionsState.fullScreenEnabled != prevOptionsState.fullScreenEnabled ||
       optionsState.vSyncEnabled != prevOptionsState.vSyncEnabled)
        AppTerminate();
    updateGraphicOptions();
    setMenu((levelManager.gameStatus == noGame) ? mainMenu : gameEscMenu);
}



void handleMenuKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        switch(currentMenu) {
            case loadingMenu:
            case mainMenu:
                AppTerminate();
                break;
            case optionsMenu:
                leaveOptionsMenu(NULL);
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
            case modalMenu:
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
        image->texture = fileManager.getPackage("Default")->getResource<Texture>("background.jpg");
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
                button->onClick = onClick[i];
                view->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = currentScreenView->height*0.1;
                label->sizeAlignment = GUISizeAlignment_Height;
                label->width = view->width+view->content.innerShadow*2.2-button->paddingX*1.0;
                button->addChild(label);
            }
        } break;
        case optionsMenu: {
            GUILabel* label = new GUILabel();
            label->posY = currentScreenView->height*0.88;
            label->text = localization.localizeString("options");
            label->fontHeight = currentScreenView->height*0.2;
            currentScreenView->addChild(label);
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
            GUIButton* button = new GUIButton();
            button->posY = currentScreenView->height*-0.8;
            button->onClick = leaveOptionsMenu;
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            prevOptionsState = optionsState;
            std::function<void(GUICheckBox*)> onClick[] = {
                [button](GUICheckBox* checkBox) {
                    optionsState.fullScreenEnabled = (checkBox->state == GUIButtonStatePressed);
                    updateOptionBackButton(button);
                }, [button](GUICheckBox* checkBox) {
                    optionsState.vSyncEnabled = (checkBox->state == GUIButtonStatePressed);
                    updateOptionBackButton(button);
                }, [](GUICheckBox* checkBox) {
                    optionsState.cubemapsEnabled = (checkBox->state == GUIButtonStatePressed);
                    updateGraphicOptions();
                }, [](GUICheckBox* checkBox) {
                    optionsState.edgeSmoothEnabled = (checkBox->state == GUIButtonStatePressed);
                    updateGraphicOptions();
                }, [](GUICheckBox* checkBox) {
                    optionsState.screenBlurFactor = (checkBox->state == GUIButtonStatePressed) ? 0.0 : -1.0;
                    updateGraphicOptions();
                }
            };
            bool checkBoxActive[] = {
                optionsState.fullScreenEnabled,
                optionsState.vSyncEnabled,
                optionsState.cubemapsEnabled,
                optionsState.edgeSmoothEnabled,
                (optionsState.screenBlurFactor > -1.0)
            };
            const char* checkBoxLabels[] = {
                "fullScreenEnabled",
                "vSyncEnabled",
                "cubemapsEnabled",
                "edgeSmoothEnabled",
                "screenBlurEnabled"
            };
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
                if(i < 2 && levelManager.gameStatus != noGame)
                    checkBox->state = GUIButtonStateDisabled;
                else if(checkBoxActive[i])
                    checkBox->state = GUIButtonStatePressed;
                view->addChild(checkBox);
            }
            
            unsigned int sliderSteps[] = { 3, 3, 4, 4, 3 };
            std::function<void(GUISlider*)> onChangeGraphics[] = {
                [](GUISlider* slider) {
                    optionsState.depthOfFieldQuality = slider->value*3.0;
                    updateGraphicOptions();
                }, [](GUISlider* slider) {
                    optionsState.surfaceQuality = slider->value*3.0;
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &optionsState.anisotropy);
                    optionsState.anisotropy = fmin(optionsState.anisotropy, pow(2.0, optionsState.surfaceQuality));
                    for(auto packageIterator : fileManager.filePackages)
                        for(auto iterator : packageIterator.second->resources) {
                            auto ptr = iterator.second.lock();
                            Texture* texture = dynamic_cast<Texture*>(ptr.get());
                            if(texture)
                                texture->updateFilters();
                        }
                    updateGraphicOptions();
                }, [](GUISlider* slider) {
                    optionsState.shadowQuality = slider->value*4.0;
                    updateGraphicOptions();
                }, [](GUISlider* slider) {
                    optionsState.ssaoQuality = slider->value*4.0;
                    updateGraphicOptions();
                }, [](GUISlider* slider) {
                    optionsState.blendingQuality = slider->value*3.0;
                    updateGraphicOptions();
                }
            };
            unsigned char sliderValuesGraphics[] = {
                optionsState.depthOfFieldQuality,
                optionsState.surfaceQuality,
                optionsState.shadowQuality,
                optionsState.ssaoQuality,
                optionsState.blendingQuality
            };
            const char* sliderLabelsGraphics[] = {
                "depthOfFieldQuality",
                "surfaceQuality",
                "shadowQuality",
                "ssaoQuality",
                "blendingQuality"
            };
            for(unsigned char i = 0; i < 5; i ++) {
                label = new GUILabel();
                label->posX = view->width*0.45;
                label->posY = currentScreenView->height*(-0.06-0.12*i);
                label->width = view->width*0.5;
                label->fontHeight = currentScreenView->height*0.1;
                label->text = localization.localizeString(sliderLabelsGraphics[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUISlider* slider = new GUISlider();
                slider->posX = view->width*-0.52;
                slider->posY = label->posY;
                slider->width = view->width*0.4;
                slider->value = (float)sliderValuesGraphics[i]/(float)sliderSteps[i];
                slider->steps = sliderSteps[i];
                slider->onChange = onChangeGraphics[i];
                view->addChild(slider);
            }
            
            std::function<void(GUISlider*)> onChange[] = {
                [](GUISlider* slider) {
                    optionsState.globalVolume = slider->value;
                }, [](GUISlider* slider) {
                    optionsState.musicVolume = slider->value;
                }, [](GUISlider* slider) {
                    optionsState.mouseSensitivity = slider->value*0.01F;
                }, [](GUISlider* slider) {
                    optionsState.mouseSmoothing = 1.0F-slider->value;
                }
            };
            float sliderValues[] = {
                optionsState.globalVolume,
                optionsState.musicVolume,
                optionsState.mouseSensitivity*100.0F,
                1.0F-optionsState.mouseSmoothing
            };
            const char* sliderLabels[] = {
                "soundGlobal",
                "soundMusic",
                "mouseSensitivity",
                "mouseSmoothing"
            };
            for(char m = 0; m < 4; m += 2) {
                label = new GUILabel();
                label->posX = currentScreenView->width*0.52;
                label->posY = currentScreenView->height*(0.72-0.28*m);
                label->text = localization.localizeString((m == 0) ? "sound" : "mouse");
                label->fontHeight = currentScreenView->height*0.14;
                currentScreenView->addChild(label);
                view = new GUIFramedView();
                view->width = currentScreenView->width*0.42;
                view->height = currentScreenView->height*0.16;
                view->posX = currentScreenView->width*0.52;
                view->posY = currentScreenView->height*(0.46-0.28*m);
                currentScreenView->addChild(view);
                for(unsigned char i = 0; i < 2; i ++) {
                    label = new GUILabel();
                    label->posX = view->width*-0.4;
                    label->posY = currentScreenView->height*(0.06-0.12*i);
                    label->width = view->width*0.5;
                    label->fontHeight = currentScreenView->height*0.1;
                    label->text = localization.localizeString(sliderLabels[i+m]);
                    label->textAlign = GUITextAlign_Left;
                    label->sizeAlignment = GUISizeAlignment_Height;
                    view->addChild(label);
                    GUISlider* slider = new GUISlider();
                    slider->posX = view->width*0.4;
                    slider->posY = label->posY;
                    slider->width = view->width*0.5;
                    slider->value = sliderValues[i+m];
                    slider->onChange = onChange[i+m];
                    view->addChild(slider);
                }
            }
            
            button = new GUIButton();
            button->posX = currentScreenView->width*0.32;
            button->posY = currentScreenView->height*-0.48;
            button->onClick = [](GUIButton* button) {
                setMenu(languagesMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("language");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.15;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            button = new GUIButton();
            button->posX = currentScreenView->width*0.72;
            button->posY = currentScreenView->height*-0.48;
            button->onClick = [](GUIButton* button) {
                //setMenu(leapMotionMenu);
            };
            button->state = GUIButtonStateDisabled;
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("leapMotion");
            label->fontHeight = currentScreenView->height*0.1;
            label->width = currentScreenView->width*0.15;
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
            label->text = std::string("Core Software: Alexander Meißner\nSupporting Software: Noah Hummel");
            label->width = currentScreenView->width*0.8;
            label->fontHeight = currentScreenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment_Height;
            currentScreenView->addChild(label);
        } break;
        case inGameMenu: {
            GUILabel* label = new GUILabel();
            label->text = std::string("+");
            label->color = Color4(1.0);
            currentScreenView->addChild(label);
            
            GUIView* view = new GUIView();
            view->posX = currentScreenView->width*-0.5;
            view->posY = currentScreenView->height*0.3;
            view->width = currentScreenView->width*0.48;
            view->height = currentScreenView->height*0.68;
            currentScreenView->addChild(view);
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
                    levelManager.clear();
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
                button->onClick = [name](GUIButton* button) {
                    currentMenu = modalMenu;
                    GUIFramedView* modalView = new GUIFramedView();
                    modalView->width = currentScreenView->width*0.4;
                    modalView->height = currentScreenView->height*0.4;
                    GUILabel* label = new GUILabel();
                    label->posY = modalView->height*0.65;
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
                    button->posY = modalView->height*-0.7;
                    button->width = currentScreenView->width*0.14;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("cancel");
                    label->fontHeight = currentScreenView->height*0.1;
                    button->addChild(label);
                    button->onClick = [](GUIButton* button) {
                        currentScreenView->setModalView(NULL);
                        currentMenu = saveGamesMenu;
                    };
                    button = new GUIButton();
                    button->posX = modalView->width*0.55;
                    button->posY = modalView->height*-0.7;
                    button->width = currentScreenView->width*0.14;
                    button->sizeAlignment = GUISizeAlignment_Height;
                    button->buttonType = GUIButtonTypeDelete;
                    modalView->addChild(button);
                    label = new GUILabel();
                    label->text = localization.localizeString("ok");
                    label->fontHeight = currentScreenView->height*0.1;
                    button->addChild(label);
                    button->onClick = [name](GUIButton* button) {
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
            button->buttonType = GUIButtonTypeAdd;
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