//
//  Menu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

const float loadingScreenTime = 3.0;
float loadingScreen = loadingScreenTime;

static void openExternURL(std::string str) {
#ifdef __APPLE__
    str = std::string("open \"")+str+"\"";
#else
    str = std::string("xdg-open \"")+str+"\"";
#endif
    std::system(str.c_str());
}

static void leaveOptionsMenu(GUIButton* button) {
    menu.setMenu((levelManager.gameStatus == noGame) ? Menu::Name::main : Menu::Name::gameEsc);
}

static GUIImage* getThumbnailOfPackage(FilePackage* package) {
    std::string name = "thumbnail";
    package->findFileByNameInSubdir("Textures/", name);
    if(name.size() > 0) {
        FileResourcePtr<Texture> thumbnail = package->getResource<Texture>(name);
        if(thumbnail) {
            GUIImage* image = new GUIImage();
            image->sizeAlignment = GUISizeAlignment::Width;
            image->texture = thumbnail;
            image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
            return image;
        }
    }
    return NULL;
}



Menu::Menu() :current(none), screenView(NULL) {
    
}

void Menu::updateWindow() {
    SDL_DisplayMode desktopDisplay;
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_GetDesktopDisplayMode(0, &desktopDisplay);
    
    if(optionsState.videoWidth == desktopDisplay.w && optionsState.videoHeight == desktopDisplay.h)
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    
    if(optionsState.videoScale > 1)
        windowFlags |= SDL_WINDOW_HIDPI;
    
    if(mainWindow) SDL_HideWindow(mainWindow);
    SDL_Window* newWindow = SDL_CreateWindow("Olypsum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             optionsState.videoWidth, optionsState.videoHeight, windowFlags);
    
    if(mainWindow) SDL_DestroyWindow(mainWindow);
    mainWindow = newWindow;
    SDL_RaiseWindow(mainWindow);
    if(glContext)
        SDL_GL_MakeCurrent(mainWindow, glContext);
    else {
        glContext = SDL_GL_CreateContext(mainWindow);
        SDL_GL_SetSwapInterval(optionsState.vSyncEnabled);
    }
    
    float videoScale = 1.0;
    SDL_GetWindowScale(mainWindow, &videoScale, &videoScale);
    optionsState.videoScale = videoScale;
    updateSSAOShaderPrograms();
}

void Menu::updateVideoResulution() {
    SDL_GetWindowSize(mainWindow, &optionsState.videoWidth, &optionsState.videoHeight);
    if(levelManager.gameStatus != noGame) {
        mainFBO.init();
        mainCam->aspect = (float)optionsState.videoWidth/optionsState.videoHeight;
        mainCam->updateViewMat();
        for(auto lightObject : objectManager.lightObjects)
            lightObject->deleteShadowMap();
    }
    menu.setMenu(menu.current);
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

void Menu::handleMouseDown(Uint8 button) {
    if((button == SDL_BUTTON_LEFT && screenView->handleMouseDown(mouseX, mouseY))
       || menu.current != inGame) return;
    
    v8::HandleScope handleScope;
    ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
    if(script) script->callFunction("onmousedown", false, { v8::Number::New(button) });
}

void Menu::handleMouseUp(Uint8 button) {
    if(screenView->handleMouseUp(mouseX, mouseY) || menu.current != inGame) return;
    
    v8::HandleScope handleScope;
    ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
    if(script) script->callFunction("onmouseup", false, { v8::Number::New(button) });
}

void Menu::handleMouseWheel(float deltaX, float deltaY) {
    if(screenView->handleMouseWheel(mouseX, mouseY, deltaX, deltaY) || menu.current != inGame) return;
    
    v8::HandleScope handleScope;
    ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
    if(script) script->callFunction("onmousewheel", false, { v8::Number::New(deltaX), v8::Number::New(deltaY) });
}

void Menu::handleKeyDown(SDL_Keycode key, const char* text) {
    if(screenView->handleKeyDown(key, text) || menu.current != inGame) return;
    
    v8::HandleScope handleScope;
    ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
    if(script) script->callFunction("onkeydown", false, { v8::Integer::New(key) });
}

void Menu::handleKeyUp(SDL_Keycode key) {
    if(screenView->handleKeyUp(key)) return;
    
    if(key == SDLK_ESCAPE) {
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
                setMenu(options);
                return;
            case multiplayer:
                networkManager.setLocalScan(false);
            case credits:
            case saveGames:
                setMenu(main);
                return;
            case inGame:
                setPause(true);
                return;
            case gameEsc:
                setPause(false);
                return;
            case newGame:
                setMenu(saveGames);
                return;
        }
    }
    
    if(menu.current == inGame) {
        v8::HandleScope handleScope;
        ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
        if(script) script->callFunction("onkeyup", false, { v8::Integer::New(key) });
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
            if(!shaderPrograms[solidGSP]->linked())
                updateGBufferShaderPrograms();
            else if(!shaderPrograms[directionalShadowLightSP]->linked())
                updateIlluminationShaderPrograms();
            else if(optionsState.ssaoQuality && !shaderPrograms[ssaoSP]->linked())
                updateSSAOShaderPrograms();
            else if(!shaderPrograms[depthOfFieldSP]->linked())
                updateDOFShaderProgram();
            
            loadingScreen -= profiler.animationFactor;
            GUIProgressBar* progressBar = static_cast<GUIProgressBar*>(screenView->children[0]);
            progressBar->value = 1.0-loadingScreen/loadingScreenTime;
            if(loadingScreen <= 0.0)
                setMenu(main);
        } break;
        case inGame: {
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
            
            if(mouseFixed) {
                mouseX = optionsState.mouseSmoothing*mouseVelocityX;
                mouseY = optionsState.mouseSmoothing*mouseVelocityY;
                mouseVelocityX -= mouseX;
                mouseVelocityY -= mouseY;
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }else{
                mouseVelocityX = 0.0;
                mouseVelocityY = 0.0;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        } break;
        default:
            break;
    }
}

void Menu::clear() {
    if(screenView)
        delete screenView;
    screenView = new GUIScreenView();
    current = none;
}

void Menu::setPause(bool active) {
    if(levelManager.gameStatus == noGame) return;
    setMenu((active) ? gameEsc : inGame);
    
    v8::HandleScope handleScope;
    ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
    if(script) script->callFunction("onpause", false, { });
}

void Menu::setModalView(const std::string& title, const std::string& text, std::function<void(GUIButton* button)> onContinue) {
    if(menu.screenView->modalView) return;
    
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

void Menu::setMenu(Name menu) {
    clear();
    current = menu;
    
    if(levelManager.gameStatus == noGame && menu != loading) {
        GUIImage* image = new GUIImage();
        image->texture = fileManager.getResourceByPath<Texture>(NULL, "/Core/background.jpg");
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
    }
    
    switch(menu) {
        case loading: {
            GUIProgressBar* progressBar = new GUIProgressBar();
            progressBar->posY = screenView->height*-0.7;
            screenView->addChild(progressBar);
            
            GUIImage* image = new GUIImage();
            image->sizeAlignment = GUISizeAlignment::Height;
            image->texture = fileManager.getResourceByPath<Texture>(NULL, "/Core/logo.png");
            image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
            image->width = screenView->width*0.8;
            image->posY = screenView->height*0.2;
            screenView->addChild(image);
            
            GUILabel* label = new GUILabel();
            label->text = fileManager.localizeString("loading");
            label->posY = screenView->height*-0.5;
            label->fontHeight = screenView->height*0.16;
            screenView->addChild(label);
        } break;
        case main: {
            GUIFramedView* view = new GUIFramedView();
            view->width = screenView->width*0.2;
            view->height = screenView->height*0.42;
            view->posX = screenView->width*-0.72;
            screenView->addChild(view);
            
            std::function<void(GUIButton*)> onClick[] = {
                [this](GUIButton* button) {
                    setMenu(saveGames);
                }, [this](GUIButton* button) {
                    setMenu(multiplayer);
                }, [this](GUIButton* button) {
                    setMenu(options);
                }, [this](GUIButton* button) {
                    setMenu(credits);
                }, [](GUIButton* button) {
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "saveGames", "multiplayer", "options", "credits", "quitGame" };
            for(unsigned char i = 0; i < 5; i ++) {
                GUIButton* button = new GUIButton();
                button->posY = screenView->height*(0.32-0.16*i);
                //if(i == 1) button->state = GUIButtonStateDisabled; //Multiplayer is not implemented yet
                button->onClick = onClick[i];
                view->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = fileManager.localizeString(buttonLabels[i]);
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->fontHeight = screenView->height*0.1;
                label->sizeAlignment = GUISizeAlignment::Height;
                label->width = view->width+view->content.innerShadow*2.2-button->paddingX*1.0;
                button->addChild(label);
            }
            
            GUILabel* label = new GUILabel();
            label->text = fileManager.localizeString("version")+": "+VERSION;
            label->textAlignment = GUILabel::TextAlignment::Left;
            label->sizeAlignment = GUISizeAlignment::Height;
            label->width = screenView->width*0.4;
            label->posX = screenView->width*-0.59;
            label->posY = screenView->height*-0.96;
            label->fontHeight = screenView->height*0.08;
            screenView->addChild(label);
        } break;
        case options: {
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("options");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            
            label = new GUILabel();
            label->posX = screenView->width*-0.5;
            label->posY = screenView->height*0.65;
            label->text = fileManager.localizeString("graphics");
            label->fontHeight = screenView->height*0.14;
            screenView->addChild(label);
            
            GUIFramedView* view = new GUIFramedView();
            view->width = screenView->width*0.4;
            view->height = screenView->height*0.5;
            view->posX = screenView->width*-0.5;
            screenView->addChild(view);
            
            std::function<void(GUICheckBox*)> onClick[] = {
                [](GUICheckBox* checkBox) {
                    optionsState.vSyncEnabled = (checkBox->state == GUIButton::State::Pressed);
                    SDL_GL_SetSwapInterval(optionsState.vSyncEnabled);
                }, [this](GUICheckBox* checkBox) {
                    SDL_SetWindowFullscreen(mainWindow, (SDL_GetWindowFlags(mainWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP)
                                            ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
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
                "retina",
                "cubemapsEnabled",
                "edgeSmoothEnabled",
                "screenBlurEnabled"
            };
            for(unsigned char i = 0; i < 6; i ++) {
                label = new GUILabel();
                label->posX = view->width*((i < 3) ? -0.25 : 0.55);
                label->posY = screenView->height*(0.42-0.12*(i%3));
                label->width = view->width*0.5;
                label->fontHeight = screenView->height*0.1;
                label->text = fileManager.localizeString(checkBoxLabels[i]);
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                view->addChild(label);
                GUICheckBox* checkBox = new GUICheckBox();
                checkBox->posX = view->width*((i < 3) ? -0.85 : -0.07);
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
                slider->posX = view->width*-0.45;
                slider->posY = label->posY;
                slider->width = view->width*0.4;
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
                    optionsState.mouseSensitivity = slider->value*2.0F;
                }, [](GUISlider* slider, bool dragging) {
                    optionsState.mouseSmoothing = max(0.01F, 1.0F-slider->value);
                }
            };
            float sliderValues[] = {
                optionsState.globalVolume,
                optionsState.musicVolume,
                optionsState.mouseSensitivity*0.5F,
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
                screenView->addChild(label);
                view = new GUIFramedView();
                view->width = screenView->width*0.4;
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
            label->text = fileManager.localizeString("back");
            label->fontHeight = screenView->height*0.1;
            label->width = screenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment::Height;
            button->addChild(label);
            button = new GUIButton();
            button->posX = screenView->width*0.5;
            button->posY = screenView->height*-0.8;
            button->onClick = [this](GUIButton* button) {
                setMenu(languages);
            };
            screenView->addChild(button);
            label = new GUILabel();
            label->text = fileManager.localizeString("language");
            label->fontHeight = screenView->height*0.1;
            label->width = screenView->width*0.15;
            label->sizeAlignment = GUISizeAlignment::Height;
            button->addChild(label);
        } break;
        case languages: {
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("language");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            GUIButton* button = new GUIButton();
            button->posY = screenView->height*-0.8;
            button->onClick = [this](GUIButton* button) {
                setMenu(options);
            };
            screenView->addChild(button);
            label = new GUILabel();
            label->text = fileManager.localizeString("back");
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
                label->text = languages[i].c_str();
                label->fontHeight = screenView->height*0.06;
                button->addChild(label);
                tabs->addChild(button);
                if(languages[i] == optionsState.language) {
                    tabs->selected = i;
                    tabs->updateContent();
                }
            }
            view->addChild(tabs);
            tabs->posY = view->height-tabs->height;
            view->contentHeight = tabs->height*2;
        } break;
        case credits: {
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("credits");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            
            GUIScrollView* view = new GUIScrollView();
            view->width = screenView->width*0.7;
            view->height = screenView->height*0.6;
            view->content.innerShadow = view->content.cornerRadius * 0.5;
            screenView->addChild(view);
            
            label = new GUILabel();
            label->text = std::string("Engine: Alexander Meißner\nDeveloper Kit: Noah Hummel\n© 2012, Gamefortec");
            label->posY = view->height*0.6;
            label->width = screenView->width*0.8;
            label->fontHeight = screenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment::Height;
            view->addChild(label);
            
            GUIButton* button = new GUIButton();
            button->posY = screenView->height*0.14;
            button->onClick = [](GUIButton* button) {
                openExternURL("http://gamefortec.net/");
            };
            view->addChild(button);
            label = new GUILabel();
            label->text = "http://gamefortec.net/";
            label->fontHeight = screenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment::All;
            button->addChild(label);
            
            label = new GUILabel();
            label->text = std::string("Using: SDL2, SDL2-TTF, SDL2-Image,\nBullet Physics: © 2012, Advanced Micro Devices, Inc.\nOgg / Vorbis: © 2012, Xiph.Org Foundation\nLeap SDK: © 2012, Leap Motion, Inc.\nV8: © 2012, Google, Inc.");
            label->posY = -view->height*0.5;
            label->width = screenView->width*0.8;
            label->fontHeight = screenView->height*0.1;
            label->sizeAlignment = GUISizeAlignment::Height;
            view->addChild(label);
            
            button = new GUIButton();
            button->posY = screenView->height*-0.8;
            button->onClick = [this](GUIButton* button) {
                setMenu(main);
            };
            screenView->addChild(button);
            label = new GUILabel();
            label->text = fileManager.localizeString("back");
            label->fontHeight = screenView->height*0.1;
            label->width = screenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment::Height;
            button->addChild(label);
        } break;
        case inGame: {
            GUIView* view = new GUIView();
            view->posX = screenView->width*-0.5;
            view->posY = screenView->height*0.3;
            view->width = screenView->width*0.48;
            view->height = screenView->height*0.68;
            screenView->addChild(view);
        } break;
        case gameEsc: {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(mainWindow, optionsState.videoWidth >> 1, optionsState.videoHeight >> 1);
            std::function<void(GUIButton*)> onClick[] = {
                [this](GUIButton* button) {
                    setMenu(inGame);
                }, [this](GUIButton* button) {
                    setMenu(options);
                }, [this](GUIButton* button) {
                    levelManager.clear();
                    setMenu(main);
                }, [](GUIButton* button) {
                    levelManager.clear();
                    AppTerminate();
                }
            };
            const char* buttonLabels[] = { "back", "options", "mainMenu", "quitGame" };
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
        } break;
        case saveGames: {
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("saveGames");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            
            GUIScrollView* scrollView = new GUIScrollView();
            scrollView->width = screenView->width*0.97;
            scrollView->height = screenView->height*0.7;
            screenView->addChild(scrollView);
            
            size_t validSaveGames = 0;
            forEachInDir(supportPath+"Saves/", NULL, [this, &validSaveGames, &scrollView](const std::string& directoryPath, std::string name) {
                std::string path = supportPath+"Saves/"+name;
                rapidxml::xml_document<xmlUsedCharType> doc;
                std::unique_ptr<char[]> fileData = readXmlFile(doc, supportPath+"Saves/"+name+"Status.xml", false);
                if(!fileData) return false;
                rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("Status");
                FilePackage* package = fileManager.loadPackage(node->first_node("Package")->first_attribute()->value());
                if(!package) return false;
                name.pop_back();
                
                std::string container = node->first_node("Level")->first_attribute()->value();
                GUIButton* button = new GUIButton();
                button->posX = screenView->width*-0.18;
                button->posY = scrollView->height*(0.8-validSaveGames*0.4);
                button->onClick = [name, package, container](GUIButton* button) {
                    levelManager.loadGame(package, name, container);
                };
                scrollView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = name;
                label->posX = screenView->width*-0.25;
                label->posY = screenView->height*0.05;
                label->width = screenView->width*0.3;
                label->fontHeight = screenView->height*0.1;
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                button->addChild(label);
                label = new GUILabel();
                label->text = package->name;
                label->posX = screenView->width*0.1;
                label->posY = -screenView->height*0.05;
                label->width = screenView->width*0.65;
                label->fontHeight = screenView->height*0.1;
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                button->addChild(label);
                if((node = node->first_node("Description")) && (node = node->first_node()) && node->type() == rapidxml::node_cdata) {
                    label = new GUILabel();
                    label->text = node->value();
                    label->posX = screenView->width*0.45;
                    label->posY = screenView->height*0.05;
                    label->width = screenView->width*0.3;
                    label->fontHeight = screenView->height*0.1;
                    label->textAlignment = GUILabel::TextAlignment::Right;
                    label->sizeAlignment = GUISizeAlignment::Height;
                    button->addChild(label);
                }
                GUIImage* image = getThumbnailOfPackage(package);
                if(image) {
                    image->height = screenView->height*0.09;
                    image->posX = screenView->width*-0.65;
                    button->addChild(image);
                }
                
                button = new GUIButton();
                button->posX = screenView->width*0.8;
                button->posY = scrollView->height*(0.8-validSaveGames*0.4);
                button->width = screenView->width*0.14;
                button->sizeAlignment = GUISizeAlignment::Height;
                scrollView->addChild(button);
                label = new GUILabel();
                label->text = fileManager.localizeString("remove");
                label->fontHeight = screenView->height*0.1;
                button->addChild(label);
                button->onClick = [this, name](GUIButton* button) {
                    setModalView("remove", fileManager.localizeString("name")+": "+name,
                                 [this, name](GUIButton* button) {
                        removeDir(supportPath+"Saves/"+name+'/');
                        setMenu(saveGames);
                    });
                };
                
                validSaveGames ++;
                return false;
            }, NULL);
            scrollView->contentHeight = validSaveGames*0.4*scrollView->height;
            
            std::function<void(GUIButton*)> onClick[] = {
                [this](GUIButton* button) {
                    setMenu(main);
                }, [this](GUIButton* button) {
                    setMenu(newGame);
                }
            };
            const char* buttonLabels[] = { "back", "newGame" };
            for(unsigned char i = 0; i < 2; i ++) {
                GUIButton* button = new GUIButton();
                button->posX = screenView->width*(-0.8+i*1.6);
                button->posY = screenView->height*-0.8;
                button->width = screenView->width*0.14;
                button->sizeAlignment = GUISizeAlignment::Height;
                screenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = fileManager.localizeString(buttonLabels[i]);
                label->fontHeight = screenView->height*0.1;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case newGame: {
            fileManager.loadAllPackages();
            
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("newGame");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            
            GUIScrollView* scrollView = new GUIScrollView();
            scrollView->visible = false;
            scrollView->width = screenView->width*0.97;
            scrollView->height = screenView->height*0.7;
            scrollView->contentHeight = (fileManager.filePackages.size()-1)*0.4*scrollView->height;
            screenView->addChild(scrollView);
            
            GUITextField* textField = new GUITextField();
            label = static_cast<GUILabel*>(textField->children[0]);
            label->text = fileManager.localizeString("newGame");
            textField->posX = 0.0;
            textField->posY = screenView->height*-0.8;
            textField->width = screenView->width*0.4;
            textField->height = screenView->height*0.07;
            //textField->label->fontHeight = screenView->height*0.1;
            textField->onChange = [scrollView](GUITextField* textField) {
                GUILabel* label = static_cast<GUILabel*>(textField->children[0]);
                std::string path = supportPath+"Saves/"+label->text+'/';
                scrollView->visible = !checkDir(path) && label->getLength() >= 3;
            };
            screenView->addChild(textField);
            
            size_t i = 0;
            for(auto packageIterator : fileManager.filePackages) {
                FilePackage* package = packageIterator.second;
                if(package->name == "Core") continue;
                
                GUIButton* button = new GUIButton();
                button->posY = scrollView->height*(0.8-(i ++)*0.4);
                button->onClick = [package, textField](GUIButton* button) {
                    levelManager.newGame(package, static_cast<GUILabel*>(textField->children[0])->text);
                };
                scrollView->addChild(button);
                label = new GUILabel();
                label->text = package->name;
                label->posX = screenView->width*0.1;
                label->posY = screenView->height*0.05;
                label->width = screenView->width*0.8;
                label->fontHeight = screenView->height*0.1;
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                button->addChild(label);
                label = new GUILabel();
                label->text = package->description;
                label->posX = screenView->width*0.1;
                label->posY = -screenView->height*0.05;
                label->width = screenView->width*0.8;
                label->fontHeight = screenView->height*0.1;
                label->textAlignment = GUILabel::TextAlignment::Left;
                label->sizeAlignment = GUISizeAlignment::Height;
                button->addChild(label);
                GUIImage* image = getThumbnailOfPackage(package);
                if(image) {
                    image->height = screenView->height*0.09;
                    image->posX = screenView->width*-0.8;
                    button->addChild(image);
                }
            }
            
            std::function<void(GUIButton*)> onClick[] = {
                [this](GUIButton* button) {
                    openExternURL((std::string("file://")+supportPath+"Packages").c_str());
                }, [this](GUIButton* button) {
                    setMenu(saveGames);
                }
            };
            const char* buttonLabels[] = { "packages", "cancel" };
            for(unsigned char i = 0; i < 2; i ++) {
                GUIButton* button = new GUIButton();
                button->posX = screenView->width*(-0.8+i*1.6);
                button->posY = screenView->height*-0.8;
                button->width = screenView->width*0.14;
                button->sizeAlignment = GUISizeAlignment::Height;
                screenView->addChild(button);
                GUILabel* label = new GUILabel();
                label->text = fileManager.localizeString(buttonLabels[i]);
                label->fontHeight = screenView->height*0.1;
                button->addChild(label);
                button->onClick = onClick[i];
            }
        } break;
        case multiplayer: {
            GUILabel* label = new GUILabel();
            label->posY = screenView->height*0.88;
            label->text = fileManager.localizeString("multiplayer");
            label->fontHeight = screenView->height*0.2;
            screenView->addChild(label);
            
            GUIButton* button = new GUIButton();
            button->posY = screenView->height*-0.8;
            button->onClick = [this](GUIButton* button) {
                networkManager.setLocalScan(false);
                setMenu(main);
            };
            screenView->addChild(button);
            label = new GUILabel();
            label->text = fileManager.localizeString("back");
            label->fontHeight = screenView->height*0.1;
            label->width = screenView->width*0.14;
            label->sizeAlignment = GUISizeAlignment::Height;
            button->addChild(label);
            
            GUIScrollView* view = new GUIScrollView();
            view->width = screenView->width*0.7;
            view->height = screenView->height*0.6;
            view->content.innerShadow = view->content.cornerRadius * 0.5;
            screenView->addChild(view);
            
            networkManager.setLocalScan(true);
        } break;
        default:
            log(error_log, "Tried to call setMenu() with invalid menu id.");
            break;
    }
    
    screenView->updateContent();
}

Menu menu;