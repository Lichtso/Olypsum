//
//  Ray3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LinearMaths.h"

Ray3::Ray3() {
    
}

Ray3::Ray3(btVector3 originB, btVector3 directionB) :origin(originB), direction(directionB) {
    
}

void Ray3::set(btVector3 originB, btVector3 directionB) {
    origin = originB;
    direction = directionB;
}