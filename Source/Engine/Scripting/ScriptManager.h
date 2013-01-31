//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#include "ScriptLinearAlgebra.h"

#ifndef ScriptManager_h
#define ScriptManager_h

class ScriptManager {
    static v8::Handle<v8::Value> ScriptLog(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptRequire(const v8::Arguments& args);
    public:
    v8::Persistent<v8::ObjectTemplate> globalTemplate;
    std::map<std::string, ScriptFile*> loadedScripts;
    static const char* cStringOf(const v8::String::Utf8Value& value);
    static std::string stdStringOf(const v8::String::Utf8Value& value);
    ScriptManager();
    ~ScriptManager();
    ScriptFile* getScriptFile(FilePackage* filePackage, const std::string& name);
    bool tryCatch(v8::TryCatch* tryCatch);
};

extern std::unique_ptr<ScriptManager> scriptManager;

#endif
