//
//  Profiler.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 02.02.13.
//
//

#include "Profiler.h"
#include <sys/time.h>

Profiler::Profiler() :lastSec(0), newFPS(0), FPS(0) {
    gettimeofday(&lastFrame, 0);
    lastSection = lastFrame;
}

void Profiler::leaveSection(const char* name) {
    timeval now;
    gettimeofday(&now, 0);
    float time = now.tv_sec - lastSection.tv_sec;
    time += (now.tv_usec - lastSection.tv_usec) / 1000000.0;
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
    timeval now;
    gettimeofday(&now, 0);
    animationFactor = now.tv_sec - lastFrame.tv_sec;
    animationFactor += (now.tv_usec - lastFrame.tv_usec) / 1000000.0;
    lastFrame = now;
    lastSection = now;
    
    newFPS ++;
    lastSec += animationFactor;
    if(lastSec >= 1.0) {
        lastSec -= 1.0;
        FPS = newFPS;
        newFPS = 0;
        
        std::ostringstream ss;
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
        //printf("%s", ss.str().c_str());
    }
    
    //usleep(1000);
}

bool Profiler::isFirstFrameInSec() {
    return newFPS == 0;
}

Profiler profiler;