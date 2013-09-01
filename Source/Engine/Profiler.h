//
//  Profiler.h
//  Olypsum
//
//  Created by Alexander Meißner on 02.02.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Profiler_h
#define Profiler_h

#include "Network.h"

class Profiler {
    double lastFrame, lastSection;
    unsigned int newFPS;
    float lastSec;
    public:
    struct Section {
        float time, secAverage;
    };
    float animationFactor;
    unsigned int FPS;
    std::map<const char*, Section> sections;
    Profiler();
    void leaveSection(const char* name);
    void markFrame();
    bool isFirstFrameInSec();
};

extern Profiler profiler;

#endif
