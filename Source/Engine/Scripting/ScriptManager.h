//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#include "ScriptObject.h"

#ifndef ScriptManager_h
#define ScriptManager_h

#define MainScriptFileName "Main"

class ScriptManager {
    static v8::Handle<v8::Value> ScriptLog(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptRequire(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptSaveLevel(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptGetLevel(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    v8::Persistent<v8::ObjectTemplate> globalTemplate;
    std::map<std::string, ScriptFile*> loadedScripts;
    static const char* cStringOf(v8::Handle<v8::String> string);
    static std::string stdStringOf(v8::Handle<v8::String> string);
    static v8::Handle<v8::Value> readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node);
    ScriptManager();
    ~ScriptManager();
    ScriptFile* getScriptFile(FilePackage* filePackage, const std::string& name);
    v8::Handle<v8::Value> callFunctionOfScript(ScriptFile* scriptFile, const char* functionName, bool recvFirstArg, std::vector<v8::Handle<v8::Value>> args);
    bool tryCatch(v8::TryCatch* tryCatch);
};

extern std::unique_ptr<ScriptManager> scriptManager;

#endif
