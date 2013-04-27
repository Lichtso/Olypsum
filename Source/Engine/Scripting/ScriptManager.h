//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#ifndef ScriptManager_h
#define ScriptManager_h
#define MainScriptFileName "Main"

#include "ScriptGUIButton.h"

class ScriptManager {
    static v8::Handle<v8::Value> ScriptLog(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptRequire(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptLoadContainer(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptSaveLevel(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptGetLevel(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    public:
    v8::Persistent<v8::ObjectTemplate> globalTemplate;
    std::map<std::string, ScriptFile*> loadedScripts;
    static v8::Handle<v8::Value> readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node);
    static rapidxml::xml_node<xmlUsedCharType>* writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& str);
    ScriptManager();
    ~ScriptManager();
    ScriptFile* getScriptFile(FilePackage* filePackage, const std::string& name);
    bool checkFunctionOfScript(ScriptFile* script, const char* functionName);
    v8::Handle<v8::Value> callFunctionOfScript(ScriptFile* scriptFile, const char* functionName, bool recvFirstArg, std::vector<v8::Handle<v8::Value>> args);
    bool tryCatch(v8::TryCatch* tryCatch);
};

extern std::unique_ptr<ScriptManager> scriptManager;

#endif
