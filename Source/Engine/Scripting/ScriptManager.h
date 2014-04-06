//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptManager_h
#define ScriptManager_h

#include "ScriptGUIButton.h"

class ScriptManager {
    ScriptDeclareMethod(ScriptLog);
    ScriptDeclareMethod(ScriptRequire);
    ScriptDeclareMethod(ScriptLoadContainer);
    ScriptDeclareMethod(ScriptLocalizeString);
    ScriptDeclareMethod(ScriptSaveLevel);
    ScriptDeclareMethod(ScriptAccessSceneProperty);
    static void ScriptGetGamePaused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void ScriptGetLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    public:
    v8::Persistent<v8::ObjectTemplate> globalTemplate;
    std::unordered_map<std::string, AnimationProperty*> animations;
    std::unordered_set<AnimationTimer*> timers;
    static v8::Handle<v8::Value> readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node);
    static void writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>*& parentNode,
                                  const char* name, v8::Handle<v8::Value> value);
    ScriptManager();
    ~ScriptManager();
    bool tryCatch(v8::TryCatch* tryCatch);
    void gameTick();
};

extern std::unique_ptr<ScriptManager> scriptManager;

#endif
