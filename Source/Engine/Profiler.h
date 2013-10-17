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

//! Singleton class to maintain time behavior
class Profiler {
    double lastFrame, lastSection;
    unsigned int newFPS;
    float lastSec;
    public:
    struct Section {
        float time, secAverage;
    };
    std::map<const char*, Section> sections;
    float animationFactor; //!< Seconds since last frame
    unsigned int FPS; //!< Frames in last second
    Profiler();
    //! Marks the section [name] as done
    void leaveSection(const char* name);
    //! Marks a entire frame as done
    void markFrame();
    //! Returns true if we are currently in the first frame of a second
    bool isFirstFrameInSec();
};

extern Profiler profiler;

#endif
