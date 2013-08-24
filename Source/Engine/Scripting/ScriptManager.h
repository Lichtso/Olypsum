//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptManager_h
#define ScriptManager_h
#define MainScriptFileName "Main"

#include "ScriptGUIButton.h"

class ScriptManager {
    static v8::Handle<v8::Value> ScriptLog(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptRequire(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptLoadContainer(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptLocalizeString(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptSaveLevel(const v8::Arguments& args);
    static v8::Handle<v8::Value> ScriptGetLevel(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    public:
    v8::Persistent<v8::ObjectTemplate> globalTemplate;
    std::map<std::string, ScriptFile*> loadedScripts;
    std::map<std::string, AnimationProperty*> animations;
    std::set<AnimationTimer*> timers;
    static v8::Handle<v8::Value> readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node);
    static void writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>*& parentNode,
                                  const char* name, v8::Handle<v8::Value> value);
    ScriptManager();
    ~ScriptManager();
    ScriptFile* getScriptFile(FilePackage* filePackage, const std::string& name);
    bool tryCatch(v8::TryCatch* tryCatch);
    void gameTick();
};

extern std::unique_ptr<ScriptManager> scriptManager;

#endif
