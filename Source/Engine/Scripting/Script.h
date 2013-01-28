//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//
//

#include "ScriptContext.h"

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

class ScriptFile : public Script, FilePackageResource {
    public:
    ScriptFile();
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackage, const std::string& name);
};

#endif