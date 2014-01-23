//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void ScriptManager::ScriptLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return args.ScriptException("log(): Too few arguments");
    log(script_log, stdStrOfV8(args[0]));
}

void ScriptManager::ScriptRequire(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return args.ScriptException("require(): Too few arguments");
    
    FilePackage* filePackage = levelManager.levelPackage;
    std::string name = stdStrOfV8(args[0]->ToString());
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, stdStrOfV8(args[0]->ToString()));
    if(!scriptFile)
        return args.ScriptException("require(): Error loading file");
    else
        args.GetReturnValue().Set(scriptFile->exports);
}

void ScriptManager::ScriptLoadContainer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return args.ScriptException("loadContainer(): Too few arguments");
    if(!scriptMatrix4.isCorrectInstance(args[0]) || !args[1]->IsString())
        return args.ScriptException("loadContainer(): Invalid argument");
    
    LevelLoader levelLoader;
    levelLoader.transformation = scriptMatrix4.getDataOfInstance(args[0])->getBTTransform();
    if(levelLoader.loadContainer(stdStrOfV8(args[1]), false))
        args.GetReturnValue().Set(levelLoader.getResultsArray());
}

void ScriptManager::ScriptLocalizeString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("localizeString(): Too few arguments");
    if(!args[0]->IsString())
        return args.ScriptException("localizeString(): Invalid argument");
    else
        args.GetReturnValue().Set(v8::String::New(fileManager.localizeString(cStrOfV8(args[0])).c_str()));
}

void ScriptManager::ScriptSaveLevel(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 3)
        return args.ScriptException("saveLevel(): Too few arguments");
    LevelSaver levelSaver;
    args.GetReturnValue().Set(levelSaver.saveLevel(args[0], args[1], args[2]));
}

void ScriptManager::ScriptAccessSceneProperty(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return args.ScriptException("accessSceneProperty(): Too few arguments");
    if(!args[0]->IsString())
        return args.ScriptException("accessSceneProperty(): Invalid argument");
    const char* key = cStrOfV8(args[0]);
    
    if(strcmp(key, "Gravity") == 0) {
        if(args.Length() > 1 && scriptVector3.isCorrectInstance(args[1])) {
            objectManager.physicsWorld->setGravity(scriptVector3.getDataOfInstance(args[1]));
            args.GetReturnValue().Set(args[1]);
        }else
            args.GetReturnValue().Set(scriptVector3.newInstance(objectManager.physicsWorld->getGravity()));
    }else if(strcmp(key, "Ambient") == 0) {
        if(args.Length() > 1 && scriptVector3.isCorrectInstance(args[1])) {
            objectManager.sceneAmbient = scriptVector3.getDataOfInstance(args[1]);
            args.GetReturnValue().Set(args[1]);
        }else
            args.GetReturnValue().Set(scriptVector3.newInstance(objectManager.sceneAmbient));
    }else if(strcmp(key, "FogColor") == 0) {
        if(args.Length() > 1 && scriptVector3.isCorrectInstance(args[1])) {
            objectManager.sceneFogColor = scriptVector3.getDataOfInstance(args[1]);
            args.GetReturnValue().Set(args[1]);
        }else
            args.GetReturnValue().Set(scriptVector3.newInstance(objectManager.sceneFogColor));
    }else if(strcmp(key, "FogDistance") == 0) {
        if(args.Length() > 1 && args[1]->IsNumber()) {
            objectManager.sceneFogDistance = args[1]->NumberValue();
            args.GetReturnValue().Set(args[1]);
        }else
            args.GetReturnValue().Set(objectManager.sceneFogDistance);
    }
}

void ScriptManager::ScriptGetGamePaused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(menu.current != Menu::inGame);
}

void ScriptManager::ScriptGetLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(v8::String::New(levelManager.levelContainer.c_str()));
}

void ScriptManager::ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    levelManager.levelContainer = stdStrOfV8(value);
    LevelLoader levelLoader;
    levelLoader.loadLevel();
}

void ScriptManager::ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(profiler.animationFactor);
}

v8::Handle<v8::Value> ScriptManager::readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    if(!node || !(node = node->first_node("Data")) || !(node = node->first_node()) || node->type() != rapidxml::node_cdata)
        return v8::Undefined();
    return v8::String::New(node->value());
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
    v8::HandleScope handleScope;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::Isolate::GetCurrent(), v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    globalTemplate->Set(v8::String::New("loadContainer"), v8::FunctionTemplate::New(ScriptLoadContainer));
    globalTemplate->Set(v8::String::New("localizeString"), v8::FunctionTemplate::New(ScriptLocalizeString));
    globalTemplate->Set(v8::String::New("saveLevel"), v8::FunctionTemplate::New(ScriptSaveLevel));
    globalTemplate->Set(v8::String::New("accessSceneProperty"), v8::FunctionTemplate::New(ScriptAccessSceneProperty));
    globalTemplate->SetAccessor(v8::String::New("gamePaused"), ScriptGetGamePaused);
    globalTemplate->SetAccessor(v8::String::New("levelID"), ScriptGetLevel, ScriptSetLevel);
    globalTemplate->SetAccessor(v8::String::New("animationFactor"), ScriptGetAnimationFactor);
    scriptVector3.init(globalTemplate);
    scriptQuaternion.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
    scriptBaseClass.init(globalTemplate);
    scriptBaseObject.init(globalTemplate);
    scriptPhysicObject.init(globalTemplate);
    scriptRigidObject.init(globalTemplate);
    scriptTerrainObject.init(globalTemplate);
    scriptSimpleObject.init(globalTemplate);
    scriptCamObject.init(globalTemplate);
    scriptSoundObject.init(globalTemplate);
    scriptParticlesObject.init(globalTemplate);
    scriptLightObject.init(globalTemplate);
    scriptDirectionalLight.init(globalTemplate);
    scriptSpotLight.init(globalTemplate);
    scriptPositionalLight.init(globalTemplate);
    scriptBaseLink.init(globalTemplate);
    scriptPhysicLink.init(globalTemplate);
    scriptPointPhysicLink.init(globalTemplate);
    scriptGearPhysicLink.init(globalTemplate);
    scriptHingePhysicLink.init(globalTemplate);
    scriptSliderPhysicLink.init(globalTemplate);
    scriptDof6PhysicLink.init(globalTemplate);
    scriptConeTwistPhysicLink.init(globalTemplate);
    scriptTransformLink.init(globalTemplate);
    scriptBoneLink.init(globalTemplate);
    scriptIntersection.init(globalTemplate);
    scriptAnimation.init(globalTemplate);
    scriptMouse.init(globalTemplate);
    scriptKeyboard.init(globalTemplate);
    scriptGUIRect.init(globalTemplate);
    scriptGUIView.init(globalTemplate);
    scriptGUIFramedView.init(globalTemplate);
    scriptGUIScreenView.init(globalTemplate);
    scriptGUIScrollView.init(globalTemplate);
    scriptGUILabel.init(globalTemplate);
    scriptGUIProgressBar.init(globalTemplate);
    scriptGUIImage.init(globalTemplate);
    scriptGUISlider.init(globalTemplate);
    scriptGUITextField.init(globalTemplate);
    scriptGUIButton.init(globalTemplate);
    scriptGUICheckBox.init(globalTemplate);
    scriptGUITabs.init(globalTemplate);
}

ScriptManager::~ScriptManager() {
    for(auto iterator : animations)
        delete iterator.second;
    globalTemplate.Dispose();
}

bool ScriptManager::tryCatch(v8::TryCatch* tryCatch) {
    if(!tryCatch->HasCaught()) return true;
    
    v8::HandleScope handleScope;
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

ScriptVector3 scriptVector3;
ScriptQuaternion scriptQuaternion;
ScriptMatrix4 scriptMatrix4;
ScriptBaseClass scriptBaseClass;
ScriptBaseObject scriptBaseObject;
ScriptPhysicObject scriptPhysicObject;
ScriptMatterObject scriptMatterObject;
ScriptRigidObject scriptRigidObject;
ScriptTerrainObject scriptTerrainObject;
ScriptSimpleObject scriptSimpleObject;
ScriptCamObject scriptCamObject;
ScriptSoundObject scriptSoundObject;
ScriptParticlesObject scriptParticlesObject;
ScriptLightObject scriptLightObject;
ScriptDirectionalLight scriptDirectionalLight;
ScriptSpotLight scriptSpotLight;
ScriptPositionalLight scriptPositionalLight;
ScriptBaseLink scriptBaseLink;
ScriptPhysicLink scriptPhysicLink;
ScriptPointPhysicLink scriptPointPhysicLink;
ScriptGearPhysicLink scriptGearPhysicLink;
ScriptHingePhysicLink scriptHingePhysicLink;
ScriptSliderPhysicLink scriptSliderPhysicLink;
ScriptDof6PhysicLink scriptDof6PhysicLink;
ScriptConeTwistPhysicLink scriptConeTwistPhysicLink;
ScriptTransformLink scriptTransformLink;
ScriptBoneLink scriptBoneLink;
ScriptIntersection scriptIntersection;
ScriptAnimation scriptAnimation;
ScriptMouse scriptMouse;
ScriptKeyboard scriptKeyboard;
ScriptGUIRect scriptGUIRect;
ScriptGUIView scriptGUIView;
ScriptGUIFramedView scriptGUIFramedView;
ScriptGUIScreenView scriptGUIScreenView;
ScriptGUIScrollView scriptGUIScrollView;
ScriptGUILabel scriptGUILabel;
ScriptGUIProgressBar scriptGUIProgressBar;
ScriptGUIImage scriptGUIImage;
ScriptGUISlider scriptGUISlider;
ScriptGUITextField scriptGUITextField;
ScriptGUIButton scriptGUIButton;
ScriptGUICheckBox scriptGUICheckBox;
ScriptGUITabs scriptGUITabs;
std::unique_ptr<ScriptManager> scriptManager;