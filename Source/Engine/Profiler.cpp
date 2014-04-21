//
//  Profiler.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 02.02.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "Profiler.h"

Profiler::Profiler() :lastSec(0), newFPS(0), FPS(0) {
    lastSection = lastFrame = getTime();
}

void Profiler::leaveSection(const char* name) {
    double now = getTime(), time = now-lastSection;
    lastSection = now;
    
    auto iterator = sections.find(name);
    if(iterator == sections.end())
        sections[name] = Section();
    else{
        iterator->second.time = time;
        iterator->second.secAverage += time;
    }
}

void Profiler::markFrame() {
    double now = getTime();
    animationFactor = now-lastFrame;
    lastSection = lastFrame = now;
    
    newFPS ++;
    lastSec += animationFactor;
    if(lastSec >= 1.0) {
        lastSec -= 1.0;
        FPS = newFPS;
        newFPS = 0;
        
        /*std::ostringstream ss;
        ss << "Profile (" << FPS << " FPS)\n";
        foreach_e(sections, iterator) {
            if(iterator->second.secAverage == 0) {
                sections.erase(iterator);
                continue;
            }
            ss << iterator->first << ": " << (iterator->second.secAverage/FPS) << "\n";
            iterator->second.secAverage = 0;
        }
        ss << "\n";
        log(info_log, ss);*/
    }
    
    //usleep(1000);
}

bool Profiler::isFirstFrameInSec() {
    return newFPS == 0;
}

Profiler profiler;