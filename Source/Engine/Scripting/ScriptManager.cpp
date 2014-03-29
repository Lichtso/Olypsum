//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void ScriptManager::ScriptLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() == 0)
        return ScriptException("log(): Too few arguments");
    log(script_log, stdStrOfV8(args[0]));
}

void ScriptManager::ScriptRequire(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() == 0)
        return ScriptException("require(): Too few arguments");
    
    FilePackage* filePackage = levelManager.levelPackage;
    std::string name = stdStrOfV8(args[0]->ToString());
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, stdStrOfV8(args[0]->ToString()));
    if(!scriptFile)
        return ScriptException("require(): Error loading file");
    else
        ScriptReturn(scriptFile->exports);
}

void ScriptManager::ScriptLoadContainer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2)
        return ScriptException("loadContainer(): Too few arguments");
    if(!scriptMatrix4->isCorrectInstance(args[0]) || !args[1]->IsString())
        return ScriptException("loadContainer(): Invalid argument");
    
    LevelLoader levelLoader;
    levelLoader.transformation = scriptMatrix4->getDataOfInstance(args[0])->getBTTransform();
    if(levelLoader.loadContainer(stdStrOfV8(args[1]), false))
        ScriptReturn(levelLoader.getResultsArray());
}

void ScriptManager::ScriptLocalizeString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1)
        return ScriptException("localizeString(): Too few arguments");
    if(!args[0]->IsString())
        return ScriptException("localizeString(): Invalid argument");
    else
        ScriptReturn(fileManager.localizeString(cStrOfV8(args[0])).c_str());
}

void ScriptManager::ScriptSaveLevel(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 3)
        return ScriptException("saveLevel(): Too few arguments");
    LevelSaver levelSaver;
    ScriptReturn(levelSaver.saveLevel(args[0], args[1], args[2]));
}

void ScriptManager::ScriptAccessSceneProperty(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() == 0)
        return ScriptException("accessSceneProperty(): Too few arguments");
    if(!args[0]->IsString())
        return ScriptException("accessSceneProperty(): Invalid argument");
    const char* key = cStrOfV8(args[0]);
    
    if(strcmp(key, "Gravity") == 0) {
        if(args.Length() > 1 && scriptVector3->isCorrectInstance(args[1])) {
            objectManager.physicsWorld->setGravity(scriptVector3->getDataOfInstance(args[1]));
            ScriptReturn(args[1]);
        }else
            ScriptReturn(scriptVector3->newInstance(objectManager.physicsWorld->getGravity()));
    }else if(strcmp(key, "Ambient") == 0) {
        if(args.Length() > 1 && scriptVector3->isCorrectInstance(args[1])) {
            objectManager.sceneAmbient = scriptVector3->getDataOfInstance(args[1]);
            ScriptReturn(args[1]);
        }else
            ScriptReturn(scriptVector3->newInstance(objectManager.sceneAmbient));
    }else if(strcmp(key, "FogColor") == 0) {
        if(args.Length() > 1 && scriptVector3->isCorrectInstance(args[1])) {
            objectManager.sceneFogColor = scriptVector3->getDataOfInstance(args[1]);
            ScriptReturn(args[1]);
        }else
            ScriptReturn(scriptVector3->newInstance(objectManager.sceneFogColor));
    }else if(strcmp(key, "FogDistance") == 0) {
        if(args.Length() > 1 && args[1]->IsNumber()) {
            objectManager.sceneFogDistance = args[1]->NumberValue();
            ScriptReturn(args[1]);
        }else
            ScriptReturn(objectManager.sceneFogDistance);
    }
}

void ScriptManager::ScriptGetGamePaused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(menu.current != Menu::inGame);
}

void ScriptManager::ScriptGetLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(levelManager.levelContainer.c_str());
}

void ScriptManager::ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    levelManager.levelContainer = stdStrOfV8(value);
    LevelLoader levelLoader;
    levelLoader.loadLevel();
}

void ScriptManager::ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(profiler.animationFactor);
}

v8::Handle<v8::Value> ScriptManager::readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    if(!node || !(node = node->first_node("Data")) || !(node = node->first_node()) || node->type() != rapidxml::node_cdata)
        return v8::Undefined(v8::Isolate::GetCurrent());
    return ScriptString(node->value());
}

void ScriptManager::writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>*& parentNode,
                                      const char* name, v8::Handle<v8::Value> value) {
    rapidxml::xml_node<xmlUsedCharType>* dataNode = parentNode->first_node(name);
    if(value.IsEmpty() || value->IsUndefined()) {
        if(dataNode)
            parentNode->remove_node(dataNode);
        return;
    }
    if(!dataNode) {
        dataNode = doc.allocate_node(rapidxml::node_element);
        dataNode->name(name);
        parentNode->append_node(dataNode);
    }
    rapidxml::xml_node<xmlUsedCharType>* cdata = dataNode->first_node();
    if(!cdata) {
        cdata = doc.allocate_node(rapidxml::node_cdata);
        dataNode->append_node(cdata);
    }
    cdata->value(doc.allocate_string(cStrOfV8(value->ToString())));
}

ScriptManager::ScriptManager() {
    ScriptScope();
    globalTemplate.Reset(v8::Isolate::GetCurrent(), v8::ObjectTemplate::New());
    ScriptMethod(*globalTemplate, "log", ScriptLog);
    ScriptMethod(*globalTemplate, "require", ScriptRequire);
    ScriptMethod(*globalTemplate, "loadContainer", ScriptLoadContainer);
    ScriptMethod(*globalTemplate, "localizeString", ScriptLocalizeString);
    ScriptMethod(*globalTemplate, "saveLevel", ScriptSaveLevel);
    ScriptMethod(*globalTemplate, "accessSceneProperty", ScriptAccessSceneProperty);
    ScriptAccessor(*globalTemplate, "gamePaused", ScriptGetGamePaused, 0);
    ScriptAccessor(*globalTemplate, "levelID", ScriptGetLevel, ScriptSetLevel);
    ScriptAccessor(*globalTemplate, "animationFactor", ScriptGetAnimationFactor, 0);
    ScriptClassInit(scriptBaseClass);
    ScriptClassInit(scriptBaseObject);
    ScriptClassInit(scriptVector3);
    ScriptClassInit(scriptQuaternion);
    ScriptClassInit(scriptMatrix4);
    ScriptClassInit(scriptPhysicObject);
    ScriptClassInit(scriptMatterObject);
    ScriptClassInit(scriptRigidObject);
    ScriptClassInit(scriptTerrainObject);
    ScriptClassInit(scriptSimpleObject);
    ScriptClassInit(scriptCamObject);
    ScriptClassInit(scriptSoundObject);
    ScriptClassInit(scriptParticlesObject);
    ScriptClassInit(scriptLightObject);
    ScriptClassInit(scriptDirectionalLight);
    ScriptClassInit(scriptSpotLight);
    ScriptClassInit(scriptPositionalLight);
    ScriptClassInit(scriptBaseLink);
    ScriptClassInit(scriptPhysicLink);
    ScriptClassInit(scriptPointPhysicLink);
    ScriptClassInit(scriptGearPhysicLink);
    ScriptClassInit(scriptHingePhysicLink);
    ScriptClassInit(scriptSliderPhysicLink);
    ScriptClassInit(scriptDof6PhysicLink);
    ScriptClassInit(scriptConeTwistPhysicLink);
    ScriptClassInit(scriptTransformLink);
    ScriptClassInit(scriptBoneLink);
    ScriptClassInit(scriptIntersection);
    ScriptClassInit(scriptAnimation);
    ScriptClassInit(scriptMouse);
    ScriptClassInit(scriptKeyboard);
    ScriptClassInit(scriptGUIRect);
    ScriptClassInit(scriptGUIView);
    ScriptClassInit(scriptGUIFramedView);
    ScriptClassInit(scriptGUIScreenView);
    ScriptClassInit(scriptGUIScrollView);
    ScriptClassInit(scriptGUILabel);
    ScriptClassInit(scriptGUIProgressBar);
    ScriptClassInit(scriptGUIImage);
    ScriptClassInit(scriptGUISlider);
    ScriptClassInit(scriptGUITextField);
    ScriptClassInit(scriptGUIButton);
    ScriptClassInit(scriptGUICheckBox);
    ScriptClassInit(scriptGUITabs);
}

ScriptManager::~ScriptManager() {
    for(auto iterator : animations)
        delete iterator.second;
    for(auto iterator : timers)
        delete iterator;
    //globalTemplate.Reset();
}

bool ScriptManager::tryCatch(v8::TryCatch* tryCatch) {
    if(!tryCatch->HasCaught()) return true;
    
    ScriptScope();
    v8::Handle<v8::Message> message = tryCatch->Message();
    if(message.IsEmpty()) {
        log(script_log, *v8::String::Utf8Value(tryCatch->StackTrace()));
    }else{
        std::ostringstream stream;
        stream << stdStrOfV8(message->GetScriptResourceName());
        stream << ':' << message->GetLineNumber() << ": ";
        stream << stdStrOfV8(tryCatch->Exception()) << '\n';
        stream << stdStrOfV8(message->GetSourceLine()) << '\n';
        int start = message->GetStartColumn(), end = message->GetEndColumn();
        for(int i = 0; i < start; i ++)
            stream << ' ';
        for (int i = start; i < end; i ++)
            stream << '^';
        stream << '\n';
        if(!tryCatch->StackTrace().IsEmpty())
            stream << stdStrOfV8(tryCatch->StackTrace());
        log(script_log, stream.str());
    }
    return false;
}

void ScriptManager::gameTick() {
    foreach_e(animations, iterator)
        if(iterator->second->gameTick(iterator->first.c_str())) {
            delete iterator->second;
            animations.erase(iterator);
        }
    
    auto timeNow = getTime();
    foreach_e(timers, iterator)
        if((*iterator)->timeNext < timeNow && (*iterator)->gameTick(timeNow)) {
            delete *iterator;
            timers.erase(iterator);
        }
}

std::unique_ptr<ScriptVector3> scriptVector3;
std::unique_ptr<ScriptQuaternion> scriptQuaternion;
std::unique_ptr<ScriptMatrix4> scriptMatrix4;
std::unique_ptr<ScriptBaseClass> scriptBaseClass;
std::unique_ptr<ScriptBaseObject> scriptBaseObject;
std::unique_ptr<ScriptPhysicObject> scriptPhysicObject;
std::unique_ptr<ScriptMatterObject> scriptMatterObject;
std::unique_ptr<ScriptRigidObject> scriptRigidObject;
std::unique_ptr<ScriptTerrainObject> scriptTerrainObject;
std::unique_ptr<ScriptSimpleObject> scriptSimpleObject;
std::unique_ptr<ScriptCamObject> scriptCamObject;
std::unique_ptr<ScriptSoundObject> scriptSoundObject;
std::unique_ptr<ScriptParticlesObject> scriptParticlesObject;
std::unique_ptr<ScriptLightObject> scriptLightObject;
std::unique_ptr<ScriptDirectionalLight> scriptDirectionalLight;
std::unique_ptr<ScriptSpotLight> scriptSpotLight;
std::unique_ptr<ScriptPositionalLight> scriptPositionalLight;
std::unique_ptr<ScriptBaseLink> scriptBaseLink;
std::unique_ptr<ScriptPhysicLink> scriptPhysicLink;
std::unique_ptr<ScriptPointPhysicLink> scriptPointPhysicLink;
std::unique_ptr<ScriptGearPhysicLink> scriptGearPhysicLink;
std::unique_ptr<ScriptHingePhysicLink> scriptHingePhysicLink;
std::unique_ptr<ScriptSliderPhysicLink> scriptSliderPhysicLink;
std::unique_ptr<ScriptDof6PhysicLink> scriptDof6PhysicLink;
std::unique_ptr<ScriptConeTwistPhysicLink> scriptConeTwistPhysicLink;
std::unique_ptr<ScriptTransformLink> scriptTransformLink;
std::unique_ptr<ScriptBoneLink> scriptBoneLink;
std::unique_ptr<ScriptIntersection> scriptIntersection;
std::unique_ptr<ScriptAnimation> scriptAnimation;
std::unique_ptr<ScriptMouse> scriptMouse;
std::unique_ptr<ScriptKeyboard> scriptKeyboard;
std::unique_ptr<ScriptGUIRect> scriptGUIRect;
std::unique_ptr<ScriptGUIView> scriptGUIView;
std::unique_ptr<ScriptGUIFramedView> scriptGUIFramedView;
std::unique_ptr<ScriptGUIScreenView> scriptGUIScreenView;
std::unique_ptr<ScriptGUIScrollView> scriptGUIScrollView;
std::unique_ptr<ScriptGUILabel> scriptGUILabel;
std::unique_ptr<ScriptGUIProgressBar> scriptGUIProgressBar;
std::unique_ptr<ScriptGUIImage> scriptGUIImage;
std::unique_ptr<ScriptGUISlider> scriptGUISlider;
std::unique_ptr<ScriptGUITextField> scriptGUITextField;
std::unique_ptr<ScriptGUIButton> scriptGUIButton;
std::unique_ptr<ScriptGUICheckBox> scriptGUICheckBox;
std::unique_ptr<ScriptGUITabs> scriptGUITabs;
std::unique_ptr<ScriptManager> scriptManager;