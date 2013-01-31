//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//
//

#include "Controls.h"
#include <v8.h>

#ifndef Script_h
#define Script_h

class Script {
    protected:
    Script();
    public:
    v8::Persistent<v8::Script> script;
    Script(const std::string& sourceCode, const std::string& name);
    ~Script();
    v8::Handle<v8::Value> run();
};

class ScriptFile : public Script {
    public:
    ~ScriptFile();
    FilePackage* filePackage;
    std::string name;
    v8::Persistent<v8::Object> exports;
    bool load(FilePackage* filePackage, const std::string& name);
};

#endif