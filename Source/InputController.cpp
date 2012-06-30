//
//  InputController.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "InputController.h"

void handleKeyDown(SDL_keysym* key) {
    
}

void handleKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        AppTerminate();
        return;
    }
    
}


void handleMouseDown(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseUp(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseMove(SDL_MouseMotionEvent* mouse) {
    float x = 0.5-0.5*mouse->x/currentScreenView->width,
          y = 0.5-0.5*mouse->y/currentScreenView->height;
    
    //TODO: Skeleton-Test
    /*if(!skeletonPose) return;
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.rotateX(y*2.0);
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.rotateY(2.0*x-1.0);
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.rotateZ(-2.0*y);
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.rotateY(2.0*x+1.0);*/
    
    PositionalLight* light = (PositionalLight*)lightManager.lights[0];
    //light->position = Vector3(-x*4.0, 1.0, -y*4.0);
    light->position = Vector3(-x*4.0, 1.0+y*3.0, 1.0);
}