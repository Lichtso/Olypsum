//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

void handleMenuKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        switch(currentMenu) {
            case mainMenu:
                AppTerminate();
                break;
            case optionsMenu:
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((gameStatus == noGame) ? mainMenu : gameEscMenu);
                break;
            case creditsMenu:
                setMenu(mainMenu);
                break;
            case inGameMenu:
                setMenu(gameEscMenu);
                break;
            case gameEscMenu:
                setMenu(inGameMenu);
                break;
        }
    }
}

void setMenu(MenuName menu) {
    currentMenu = menu;
    if(currentScreenView) delete currentScreenView;
    currentScreenView = new GUIScreenView();
    
    if(gameStatus == noGame) {
        GUIImage* image = new GUIImage();
        image->texture = fileManager.getPackage("Default")->getTexture("background.png", GL_COMPRESSED_RGB);
        image->sizeAlignment = GUISizeAlignment_Height;
        image->width = videoInfo->current_w*0.5;
        image->updateContent();
        currentScreenView->addChild(image);
    }
    
    switch(menu) {
        case loadingMenu: {
            GUIImage* image = new GUIImage();
            image->texture = fileManager.getPackage("Default")->getTexture("logo.png", GL_COMPRESSED_RGBA);
            image->sizeAlignment = GUISizeAlignment_Height;
            image->width = videoInfo->current_w*0.4;
            image->posY = videoInfo->current_h*0.1;
            currentScreenView->addChild(image);
            GUILabel* label = new GUILabel();
            label->text = localization.localizeString("loading");
            label->posY = videoInfo->current_h*-0.1;
            label->fontHeight = videoInfo->current_h*0.08;
            label->color = GUIColor(255, 255);
            currentScreenView->addChild(label);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            currentScreenView->draw();
            SDL_GL_SwapBuffers();
        } break;
        case mainMenu: {
            GUIFramedView* view = new GUIFramedView();
            view->width = videoInfo->current_w*0.1;
            view->height = videoInfo->current_w*0.135;
            view->posX = videoInfo->current_w*-0.36;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            
            std::function<void(GUIButton*)> onClick[] = {
                [](GUIButton* button) {
                    gameStatus = localGame;
                    initScene();
                    setMenu(inGameMenu);
                }, [](GUIButton* button) {
                    gameStatus = localGame;
                    initScene();
                    setMenu(inGameMenu);
                }, [](GUIButton* button) {
                    setMenu(optionsMenu);
                }, [](GUIButton* button) {
                    setMenu(creditsMenu);
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "newGame", "loadGame", "options", "credits", "quitGame" };
            for(unsigned char i = 0; i < 5; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = videoInfo->current_h*(0.16-0.08*i);
                view->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = videoInfo->current_h*0.05;
                label->width = videoInfo->current_w*0.08;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case optionsMenu: {
            GUILabel* label = new GUILabel();
            label->posY = videoInfo->current_h*0.44;
            label->text = localization.localizeString("options");
            label->fontHeight = videoInfo->current_h*0.1;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = videoInfo->current_h*-0.37;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                loadDynamicShaderPrograms();
                setMenu((gameStatus == noGame) ? mainMenu : gameEscMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = videoInfo->current_h*0.05;
            label->width = videoInfo->current_w*0.07;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            label = new GUILabel();
            label->posX = videoInfo->current_w*-0.26;
            label->posY = videoInfo->current_h*0.36;
            label->text = localization.localizeString("graphics");
            label->fontHeight = videoInfo->current_h*0.07;
            currentScreenView->addChild(label);
            GUIFramedView* view = new GUIFramedView();
            view->width = videoInfo->current_w*0.21;
            view->height = videoInfo->current_h*0.31;
            view->posX = videoInfo->current_w*-0.26;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            
            std::function<void(GUICheckBox*)> onClick[] = {
                [](GUICheckBox* checkBox) {
                    fullScreenEnabled = (checkBox->state == GUIButtonStatePressed);
                }, [](GUICheckBox* checkBox) {
                    edgeSmoothEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    cubemapsEnabled = (checkBox->state == GUIButtonStatePressed);
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUICheckBox* checkBox) {
                    screenBlurFactor = (checkBox->state == GUIButtonStatePressed) ? 0.0 : -1.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            bool checkBoxActive[] = { fullScreenEnabled, edgeSmoothEnabled, cubemapsEnabled, (screenBlurFactor > -1.0) };
            const char* checkBoxLabels[] = { "fullScreenEnabled", "edgeSmoothEnabled", "cubemapsEnabled", "screenBlurEnabled" };
            for(unsigned char i = 0; i < 4; i ++) {
                label = new GUILabel();
                label->posX = videoInfo->current_w*0.12;
                label->posY = videoInfo->current_h*(0.27-0.06*i);
                label->width = videoInfo->current_w*0.15;
                label->fontHeight = videoInfo->current_h*0.05;
                label->text = localization.localizeString(checkBoxLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                GUICheckBox* checkBox = new GUICheckBox();
                checkBox->posX = videoInfo->current_w*-0.12;
                checkBox->posY = label->posY;
                checkBox->onClick = onClick[i];
                if(checkBoxActive[i]) checkBox->state = GUIButtonStatePressed;
                view->addChild(checkBox);
            }
            
            unsigned int sliderSteps[] = { 3, 3, 4, 5, 3 };
            std::function<void(GUISlider*)> onChange[] = {
                [](GUISlider* slider) {
                    depthOfFieldQuality = slider->value*3.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    bumpMappingQuality = slider->value*3.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    shadowQuality = slider->value*4.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    ssaoQuality = slider->value*5.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }, [](GUISlider* slider) {
                    blendingQuality = slider->value*3.0;
                    if(gameStatus != noGame) loadDynamicShaderPrograms();
                }
            };
            unsigned char sliderValues[] = { depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, blendingQuality };
            const char* sliderLabels[] = { "depthOfFieldQuality", "bumpMappingQuality", "shadowQuality", "ssaoQuality", "blendingQuality", "particleCalcTarget" };
            for(unsigned char i = 0; i < 6; i ++) {
                label = new GUILabel();
                label->posX = videoInfo->current_w*0.12;
                label->posY = videoInfo->current_h*(0.03-0.06*i);
                label->width = videoInfo->current_w*0.15;
                label->fontHeight = videoInfo->current_h*0.05;
                label->text = localization.localizeString(sliderLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->sizeAlignment = GUISizeAlignment_Height;
                view->addChild(label);
                if(i == 5) break;
                GUISlider* slider = new GUISlider();
                slider->posX = videoInfo->current_w*-0.12;
                slider->posY = label->posY;
                slider->width = videoInfo->current_w*0.08;
                slider->value = (float)sliderValues[i]/(float)sliderSteps[i];
                slider->steps = sliderSteps[i];
                slider->onChange = onChange[i];
                view->addChild(slider);
            }
            
            GUITabs* tabs = new GUITabs();
            tabs->deactivatable = false;
            tabs->posX = videoInfo->current_w*-0.12;
            tabs->posY = videoInfo->current_h*-0.27;
            tabs->width = videoInfo->current_w*0.08;
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
                label->fontHeight = videoInfo->current_h*0.03;
                button->addChild(label);
                tabs->addChild(button);
            }
            
            label = new GUILabel();
            label->posX = videoInfo->current_w*0.26;
            label->posY = videoInfo->current_h*0.36;
            label->text = localization.localizeString("language");
            label->fontHeight = videoInfo->current_h*0.07;
            currentScreenView->addChild(label);
            view = new GUIFramedView();
            view->width = videoInfo->current_w*0.21;
            view->height = videoInfo->current_h*0.08;
            view->posX = videoInfo->current_w*0.26;
            view->posY = videoInfo->current_h*0.23;
            view->innerShadow = -8;
            currentScreenView->addChild(view);
            label = new GUILabel();
            label->posY = videoInfo->current_h*0.04;
            label->width = videoInfo->current_w*0.15;
            label->fontHeight = videoInfo->current_h*0.05;
            label->text = std::string(localization.localizeString("selectedLanguage"))+": "+localization.title;
            label->textAlign = GUITextAlign_Left;
            label->sizeAlignment = GUISizeAlignment_Height;
            view->addChild(label);
            button = new GUIButton();
            button->posY = videoInfo->current_h*-0.03;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                setMenu(languagesMenu);
            };
            view->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("selectLanguage");
            label->fontHeight = videoInfo->current_h*0.05;
            label->width = videoInfo->current_w*0.1;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
        } break;
        case languagesMenu: {
            GUILabel* label = new GUILabel();
            label->posY = videoInfo->current_h*0.44;
            label->text = localization.localizeString("selectLanguage");
            label->fontHeight = videoInfo->current_h*0.1;
            currentScreenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = videoInfo->current_h*-0.37;
            button->onClick = [](GUIButton* button) {
                fileManager.saveOptions();
                setMenu(optionsMenu);
            };
            currentScreenView->addChild(button);
            label = new GUILabel();
            label->text = localization.localizeString("back");
            label->fontHeight = videoInfo->current_h*0.05;
            label->width = videoInfo->current_w*0.07;
            label->sizeAlignment = GUISizeAlignment_Height;
            button->addChild(label);
            
            std::vector<std::string> languages;
            localization.getLocalizableLanguages(languages);
            GUITabs* tabs = new GUITabs();
            tabs->deactivatable = false;
            tabs->width = videoInfo->current_w*0.1;
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
                label->fontHeight = videoInfo->current_h*0.03;
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
            label->posX = -videoInfo->current_w*0.9;
            label->posY = videoInfo->current_h*0.9;
            label->width = videoInfo->current_w*0.15;
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
                    gameStatus = noGame;
                    clearCurrentWorld();
                    setMenu(mainMenu);
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "back", "options", "mainMenu", "quitGame" };
            for(unsigned char i = 0; i < 4; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = videoInfo->current_h*(0.16-0.08*i);
                currentScreenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = localization.localizeString(buttonLabels[i]);
                label->textAlign = GUITextAlign_Left;
                label->fontHeight = videoInfo->current_h*0.05;
                label->width = videoInfo->current_w*0.07;
                label->sizeAlignment = GUISizeAlignment_Height;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
    }
}

MenuName currentMenu;
GameStatusName gameStatus = noGame;