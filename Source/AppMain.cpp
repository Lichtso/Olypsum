//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "AppMain.h"

Texture* tex;

float animationTime = 0;

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
    currentScreenView = NULL;
    currentScreenView = new GUIScreenView();
    
    GUIScrollView* scrollView = new GUIScrollView();
    scrollView->width = 500;
    scrollView->height = 300;
    scrollView->scrollWidth = 0;
    scrollView->scrollHeight = 1000;
    currentScreenView->addChild(scrollView);
    
    GUILabel* labelD = new GUILabel();
    labelD->text = std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\n!?\"='.:,;-_+#*$%&/([{}])");
    labelD->posY = 0;
    scrollView->addChild(labelD);
    
    //Init Game {
    tex = fileManager.getPackage(NULL)->getTexture("logo.png");
    //tex = new Texture();
    //tex->loadImageInRAM("Textures/logo.png");
    tex->magFilter = GL_NEAREST;
    tex->uploadToVRAM();
    tex->unloadFromRAM();
    tex->use();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1, 1, 1);
    glTexCoord2f(1, 1);
    glVertex3f(1.6, -0.2, -1);
    glTexCoord2f(1, 0);
    glVertex3f(1.6, 0.2, -1);
    glTexCoord2f(0, 0);
    glVertex3f(-1.6, 0.2, -1);
    glTexCoord2f(0, 1);
    glVertex3f(-1.6, -0.2, -1);
    glEnd();
    
    SDL_Color color = { 200, 200, 255, 255 };
    guiCam->use();
    mainFont->renderStringToScreen("Loading ...", btVector3(0.0, -250.0, -1.0), 1.0, color, true);
    SDL_GL_SwapBuffers();
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
        animationTime += animationFactor;
        mainCam->camMat.setIdentity();
        mainCam->camMat.translate(Vector3(0,-0.5,2));
        mainCam->camMat.rotateY(animationTime);
        mainCam->calculate();
        mainCam->use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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