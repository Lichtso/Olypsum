//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

Aabb3 boundingBox(Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 1.0));

void renderScene() {
    boundingBox.drawWireFrame(Vector3(1.0, 1.0, 0.0));
}

void calculateFrame() {
    
}