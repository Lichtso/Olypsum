//
//  OptionsMenu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.12.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void Menu::setOptionsMenu() {
    clearAndAddBackground();
    current = options;
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("options");
    label->fontHeight = screenView->height*0.2;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    label = new GUILabel();
    label->posX = screenView->width*-0.5;
    label->posY = screenView->height*0.65;
    label->text = fileManager.localizeString("graphics");
    label->fontHeight = screenView->height*0.14;
    label->color = Color4(1.0);
    screenView->addChild(label);
    
    GUIFramedView* view = new GUIFramedView();
    view->width = screenView->width*0.45;
    view->height = screenView->height*0.5;
    view->posX = screenView->width*-0.5;
    screenView->addChild(view);
    
    std::function<void(GUICheckBox*)> onClick[] = {
        [](GUICheckBox* checkBox) {
            optionsState.vSyncEnabled = (checkBox->state == GUIButton::State::Pressed);
            SDL_GL_SetSwapInterval(optionsState.vSyncEnabled);
        }, [this](GUICheckBox* checkBox) {
            SDL_SetWindowFullscreen(mainWindow, (checkBox->state == GUIButton::State::Pressed)
                                    ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
            updateVideoResulution();
        }, [this](GUICheckBox* checkBox) {
            optionsState.videoScale = (optionsState.videoScale > 1) ? 1 : 2;
            updateWindow();
            updateVideoResulution();
        }, [](GUICheckBox* checkBox) {
            optionsState.cubemapsEnabled = (checkBox->state == GUIButton::State::Pressed);
            updateIlluminationShaderPrograms();
        }, [](GUICheckBox* checkBox) {
            optionsState.edgeSmoothEnabled = (checkBox->state == GUIButton::State::Pressed);
        }, [](GUICheckBox* checkBox) {
            optionsState.screenBlurFactor = (checkBox->state == GUIButton::State::Pressed) ? 0.0 : -1.0;
        }
    };
    bool checkBoxActive[] = {
        optionsState.vSyncEnabled,
        (SDL_GetWindowFlags(mainWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP) > 0,
        (optionsState.videoScale > 1),
        optionsState.cubemapsEnabled,
        optionsState.edgeSmoothEnabled,
        (optionsState.screenBlurFactor > -1.0)
    };
    const char* checkBoxLabels[] = {
        "vSyncEnabled",
        "fullScreen",
        "highDefinition",
        "cubemapsEnabled",
        "edgeSmoothEnabled",
        "screenBlurEnabled"
    };
    for(unsigned char i = 0; i < 6; i ++) {
        label = new GUILabel();
        label->posX = view->width*((i < 3) ? -0.3 : 0.55);
        label->posY = screenView->height*(0.42-0.12*(i%3));
        label->width = view->width*0.5;
        label->fontHeight = screenView->height*0.1;
        label->text = fileManager.localizeString(checkBoxLabels[i]);
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        view->addChild(label);
        GUICheckBox* checkBox = new GUICheckBox();
        checkBox->posX = view->width*((i < 3) ? -0.89 : -0.05);
        checkBox->posY = label->posY;
        checkBox->onClick = onClick[i];
        checkBox->state = (checkBoxActive[i]) ? GUIButton::State::Pressed : GUIButton::State::Released;
        checkBox->enabled = (i != 2 || getMaxVideoScale() > 1.0);
        view->addChild(checkBox);
    }
    
    unsigned int sliderSteps[] = { 3, 3, 4, 4, 3 };
    std::function<void(GUISlider*, bool)> onChangeGraphics[] = {
        [](GUISlider* slider, bool dragging) {
            if(dragging) return;
            optionsState.depthOfFieldQuality = slider->value*3.0;
            updateDOFShaderProgram();
        }, [](GUISlider* slider, bool dragging) {
            if(dragging) return;
            optionsState.surfaceQuality = slider->value*3.0;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &optionsState.anisotropy);
            optionsState.anisotropy = fmin(optionsState.anisotropy, pow(2.0, optionsState.surfaceQuality));
            for(auto packageIterator : fileManager.filePackages)
                for(auto iterator : packageIterator.second->resources) {
                    Texture* texture = dynamic_cast<Texture*>(iterator.second);
                    if(texture)
                        texture->updateFilters();
                }
            updateGBufferShaderPrograms();
        }, [](GUISlider* slider, bool dragging) {
            if(dragging) return;
            optionsState.shadowQuality = slider->value*4.0;
            updateIlluminationShaderPrograms();
        }, [](GUISlider* slider, bool dragging) {
            if(dragging) return;
            optionsState.ssaoQuality = slider->value*4.0;
            updateSSAOShaderPrograms();
        }, [](GUISlider* slider, bool dragging) {
            if(dragging) return;
            optionsState.blendingQuality = slider->value*3.0;
            updateGBufferShaderPrograms();
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
        label->posX = view->width*0.55;
        label->posY = screenView->height*(0.06-0.12*i);
        label->width = view->width*0.5;
        label->fontHeight = screenView->height*0.1;
        label->text = fileManager.localizeString(sliderLabelsGraphics[i]);
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        view->addChild(label);
        GUISlider* slider = new GUISlider();
        slider->posX = view->width*-0.47;
        slider->posY = label->posY;
        slider->width = view->width*0.48;
        slider->value = (float)sliderValuesGraphics[i]/(float)sliderSteps[i];
        slider->steps = sliderSteps[i];
        slider->onChange = onChangeGraphics[i];
        view->addChild(slider);
    }
    
    std::function<void(GUISlider*, bool)> onChange[] = {
        [](GUISlider* slider, bool dragging) {
            optionsState.globalVolume = slider->value;
        }, [](GUISlider* slider, bool dragging) {
            optionsState.musicVolume = slider->value;
        }, [](GUISlider* slider, bool dragging) {
            optionsState.mouseSensitivity = slider->value;
        }, [](GUISlider* slider, bool dragging) {
            optionsState.mouseSmoothing = max(0.01F, 1.0F-slider->value);
        }
    };
    float sliderValues[] = {
        optionsState.globalVolume,
        optionsState.musicVolume,
        optionsState.mouseSensitivity,
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
        label->posX = screenView->width*0.5;
        label->posY = screenView->height*(0.65-0.35*m);
        label->text = fileManager.localizeString((m == 0) ? "sound" : "mouse");
        label->fontHeight = screenView->height*0.14;
        label->color = Color4(1.0);
        screenView->addChild(label);
        view = new GUIFramedView();
        view->width = screenView->width*0.45;
        view->height = screenView->height*0.15;
        view->posX = screenView->width*0.5;
        view->posY = screenView->height*(0.35-0.35*m);
        screenView->addChild(view);
        for(unsigned char i = 0; i < 2; i ++) {
            label = new GUILabel();
            label->posX = view->width*-0.4;
            label->posY = screenView->height*(0.06-0.12*i);
            label->width = view->width*0.5;
            label->fontHeight = screenView->height*0.1;
            label->text = fileManager.localizeString(sliderLabels[i+m]);
            label->textAlignment = GUILabel::TextAlignment::Left;
            label->sizeAlignment = GUISizeAlignment::Height;
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
    
    GUIButton* button = new GUIButton();
    button->posX = screenView->width*-0.5;
    button->posY = screenView->height*-0.8;
    button->onClick = leaveOptionsMenu;
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("return");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.14;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    button = new GUIButton();
    button->posX = screenView->width*0.5;
    button->posY = screenView->height*-0.8;
    button->onClick = [this](GUIButton* button) {
        setLanguagesMenu();
    };
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("language");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.15;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    
    screenView->updateContent();
}

void Menu::setLanguagesMenu() {
    clearAndAddBackground();
    current = languages;
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("language");
    label->fontHeight = screenView->height*0.2;
    label->color = Color4(1.0);
    screenView->addChild(label);
    GUIButton* button = new GUIButton();
    button->posY = screenView->height*-0.8;
    button->onClick = [this](GUIButton* button) {
        setOptionsMenu();
    };
    screenView->addChild(button);
    label = new GUILabel();
    label->text = fileManager.localizeString("return");
    label->fontHeight = screenView->height*0.1;
    label->width = screenView->width*0.14;
    label->sizeAlignment = GUISizeAlignment::Height;
    button->addChild(label);
    
    std::vector<std::string> languages;
    fileManager.loadPackage("Core")->getLocalizableLanguages(languages);
    GUIScrollView* view = new GUIScrollView();
    view->width = screenView->width*0.3;
    view->height = screenView->height*0.6;
    screenView->addChild(view);
    GUITabs* tabs = new GUITabs();
    tabs->deactivatable = false;
    tabs->width = screenView->width*0.2;
    tabs->sizeAlignment = GUISizeAlignment::Height;
    tabs->orientation = GUIOrientation::Vertical;
    tabs->onChange = [languages](GUITabs* tabs) {
        optionsState.language = languages[tabs->selected];
        for(auto packageIterator : fileManager.filePackages)
            packageIterator.second->loadLocalization();
    };
    for(unsigned char i = 0; i < languages.size(); i ++) {
        GUIButton* button = new GUIButton();
        label = new GUILabel();
        label->text = languages[i];
        label->fontHeight = screenView->height*0.06;
        button->addChild(label);
        tabs->addChild(button);
        if(languages[i] == optionsState.language)
            tabs->selected = i;
    }
    view->addChild(tabs);
    tabs->posY = view->height-view->content.cornerRadius-tabs->height;
    view->contentHeight = tabs->height*2;
    
    screenView->updateContent();
}