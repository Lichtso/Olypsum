//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

v8::Handle<v8::Value> ScriptManager::ScriptLog(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return v8::ThrowException(v8::String::New("log(): Too few arguments"));
    log(script_log, stdStrOfV8(args[0]));
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptManager::ScriptRequire(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return v8::ThrowException(v8::String::New("require(): Too few arguments"));
    
    FilePackage* filePackage;
    std::string name;
    if(!fileManager.readResourcePath(stdStrOfV8(args[0]->ToString()), filePackage, name))
        return v8::ThrowException(v8::String::New("require(): Error loading file"));
    
    return handleScope.Close(scriptManager->getScriptFile(filePackage, name)->exports);
}

v8::Handle<v8::Value> ScriptManager::ScriptLoadContainer(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("loadContainer(): Too few arguments"));
    if(!scriptMatrix4.isCorrectInstance(args[0]) || !args[1]->IsString())
        return v8::ThrowException(v8::String::New("loadContainer(): Invalid argument"));
    LevelLoader levelLoader;
    levelLoader.transformation = scriptMatrix4.getDataOfInstance(args[0])->getBTTransform();
    
    if(!levelLoader.loadContainer(stdStrOfV8(args[1]), false))
       return v8::Undefined();
    
    return levelLoader.getResultsArray();
}

v8::Handle<v8::Value> ScriptManager::ScriptSaveLevel(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("saveLevel(): Too few arguments"));
    std::string localData = (args[0]->IsString()) ? stdStrOfV8(args[0]) : "",
                globalData = (args[1]->IsString()) ? stdStrOfV8(args[1]) : "";
    return v8::Boolean::New(levelManager.saveLevel(localData, globalData));
}

v8::Handle<v8::Value> ScriptManager::ScriptGetLevel(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    return v8::String::New(levelManager.levelId.c_str());
}

void ScriptManager::ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    levelManager.loadLevel(levelManager.levelPackage->name, stdStrOfV8(value));
}

v8::Handle<v8::Value> ScriptManager::ScriptGetAnimationFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    return v8::Number::New(profiler.animationFactor);
}

v8::Handle<v8::Value> ScriptManager::readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    if(!node || !(node = node->first_node("Data")) || !(node = node->first_node()) || node->type() != rapidxml::node_cdata)
        return v8::Undefined();
    return v8::String::New(node->value());
}

rapidxml::xml_node<xmlUsedCharType>* ScriptManager::writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& str) {
    rapidxml::xml_node<xmlUsedCharType>* data = doc.allocate_node(rapidxml::node_element);
    data->name("Data");
    rapidxml::xml_node<xmlUsedCharType>* cdata = doc.allocate_node(rapidxml::node_cdata);
    cdata->value(doc.allocate_string(str.c_str()));
    data->append_node(cdata);
    return data;
}

ScriptManager::ScriptManager() {
    v8::HandleScope handleScope;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::Isolate::GetCurrent(), v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    globalTemplate->Set(v8::String::New("loadContainer"), v8::FunctionTemplate::New(ScriptLoadContainer));
    globalTemplate->Set(v8::String::New("saveLevel"), v8::FunctionTemplate::New(ScriptSaveLevel));
    globalTemplate->SetAccessor(v8::String::New("levelID"), ScriptGetLevel, ScriptSetLevel);
    globalTemplate->SetAccessor(v8::String::New("animationFactor"), ScriptGetAnimationFactor);
    scriptVector3.init(globalTemplate);
    scriptQuaternion.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
    scriptBaseClass.init(globalTemplate);
    scriptBaseObject.init(globalTemplate);
    scriptPhysicObject.init(globalTemplate);
    scriptModelObject.init(globalTemplate);
    scriptRigidObject.init(globalTemplate);
    scriptTerrainObject.init(globalTemplate);
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
    for(auto iterator : loadedScripts)
        delete iterator.second;
    for(auto iterator : animations)
        delete iterator.second;
    globalTemplate.Dispose();
}

ScriptFile* ScriptManager::getScriptFile(FilePackage* filePackage, const std::string& name) {
    auto iterator = loadedScripts.find(name);
    if(iterator != loadedScripts.end())
        return iterator->second;
    ScriptFile* script = new ScriptFile();
    script->load(filePackage, name);
    loadedScripts[name] = script;
    return script;
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

std::unique_ptr<ScriptManager> scriptManager;