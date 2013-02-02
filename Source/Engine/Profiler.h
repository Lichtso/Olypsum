//
//  Profiler.h
//  Olypsum
//
//  Created by Alexander Meißner on 02.02.13.
//
//

#include "Utilities.h"

#ifndef Profiler_h
#define Profiler_h

class Profiler {
    timeval lastFrame, lastSection;
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
