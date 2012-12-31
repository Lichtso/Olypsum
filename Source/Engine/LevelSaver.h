//
//  LevelSaver.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//
//

#import "ObjectManager.h"

#ifndef LevelSaver_h
#define LevelSaver_h

//! @cond
struct LinkSaver {
    int index[2];
    std::string name[2];
};
//! @endcond

class LevelSaver {
    int objectCounter;
    std::map<BaseLink*, LinkSaver*> linkingMap;
    public:
    LevelSaver();
    ~LevelSaver();
    void pushObject(BaseObject* object);
    bool saveLevel();
};

#endif