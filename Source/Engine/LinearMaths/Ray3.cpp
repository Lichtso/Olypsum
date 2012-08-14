//
//  Ray3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Ray3.h"

Ray3::Ray3() {
    
}

Ray3::Ray3(Vector3 originB, Vector3 directionB) {
    origin = originB;
    direction = directionB;
}

void Ray3::set(Vector3 originB, Vector3 directionB) {
    origin = originB;
    direction = directionB;
}