//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "AppMain.h"

float animationTime = 0;

Model* duckModel;

void AppMain(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL, Quit.\n");
        exit(1);
    }
    
    videoInfo = SDL_GetVideoInfo();
    if(!videoInfo) {
        printf("Coudn't get video information, Quit.\n");
        exit(2);
    }
    
    if(TTF_Init() == -1) {
        printf("Coudn't init TTF lib, Quit.\n");
        exit(3);
    }
    
    SDL_EnableUNICODE(1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    screen = SDL_SetVideoMode(videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel, SDL_OPENGL);//SDL_FULLSCREEN
    if(!screen) {
        printf("Coudn't set fullscreen, Quit.\n");
        exit(4);
    }
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1, 1, 1, 0);
    
    //Init Cams
    mainCam = new Cam();
    mainCam->camMat.translate(Vector3(0,0,1));
    mainCam->calculate();
    mainCam->use();
    guiCam = new Cam();
    guiCam->fov = 0.0;
    guiCam->width = guiCam->viewport[2]/2;
    guiCam->height = guiCam->viewport[3]/2;
    guiCam->calculate();
    
    //Init Resources
    fileManager.loadPackage(NULL);
    mainFont = new TextFont();
    mainFont->size = 20;
    mainFont->loadTTF("font");
    titleFont = new TextFont();
    titleFont->size = 30;
    titleFont->loadTTF("font");
    mainShaderProgram = new ShaderProgram();
    mainShaderProgram->loadShaderProgram("main");
    mainShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    mainShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    mainShaderProgram->addAttribute(NORMAL_ATTRIBUTE, "normal");
    mainShaderProgram->link();
    spriteShaderProgram = new ShaderProgram();
    spriteShaderProgram->loadShaderProgram("sprite");
    spriteShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    spriteShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    spriteShaderProgram->link();
    shadowShaderProgram = new ShaderProgram();
    shadowShaderProgram->loadShaderProgram("shadow");
    shadowShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    shadowShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shadowShaderProgram->link();
    currentScreenView = new GUIScreenView();
    
    duckModel = fileManager.getPackage(NULL)->getModel("Duck.dae");
    
    GUIImage* image = new GUIImage();
    image->texture = fileManager.getPackage(NULL)->getTexture("logo.png");
    image->sizeAlignment = GUISizeAlignment_Height;
    image->width = 400;
    image->posY = -350;
    currentScreenView->addChild(image);
    
    GUILabel* labelD = new GUILabel();
    labelD->text = std::string("COLLADA Tests");
    labelD->posY = 400;
    currentScreenView->addChild(labelD);
    
    //Init Game {
    
    //}
    
    unsigned long thenTicks = 0, nowTicks;
    while(true) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    handleKeyDown(&event.key.keysym);
                    break;
                case SDL_KEYUP:
                    handleKeyUp(&event.key.keysym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handleMouseDown(&event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handleMouseUp(&event.button);
                    break;
                case SDL_MOUSEMOTION:
                    handleMouseMove(&event.motion);
                    break;
                case SDL_QUIT:
                    AppTerminate();
                    break;
                default:
                    break;
            }
        }
        updateKeyState();
        
        //Step Game {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        animationTime += animationFactor;
        mainCam->camMat.setIdentity();
        mainCam->camMat.translate(Vector3(0,0,5));
        mainCam->calculate();
        mainCam->use();
        modelMat.setIdentity();
        modelMat.scale(Vector3(0.05, 0.05, 0.05));
        modelMat.translate(Vector3(0,-1.5,0));
        modelMat.rotateY(animationTime);
        
        mainShaderProgram->use();
        duckModel->draw();
        
        /*char fpsStr[32];
        sprintf(fpsStr, "FPS: %d", (int)round(1.0/animationFactor));
        mainFont->renderStringToScreen(fpsStr, btVector3(1.0, -0.5, 0.0), 0.002, color, true);
        mainFont->renderStringToScreen(fpsStr, btVector3(-1.0, -0.5, 0.0), 0.002, color, true);
        mainFont->renderStringToScreen(fpsStr, btVector3(0.0, 0.5, 0.0), 0.002, color, true);
        */
        
        if(currentScreenView)
            currentScreenView->draw();
        SDL_GL_SwapBuffers();
        //}
        
        if(thenTicks > 0) {
			nowTicks = clock();
			animationFactor = (float)(nowTicks-thenTicks)/CLOCKS_PER_SEC;
			if(animationFactor < 1.0/50.0) animationFactor = 1.0/50.0;
            thenTicks = nowTicks;
        }else{
            thenTicks = clock();
			animationFactor = 0.0;
        }
    }
}

void AppTerminate() {
    TTF_Quit();
    SDL_Quit();
    exit(0);
}

float animationFactor;
std::string resourcesDir, gameDataDir, parentDir;