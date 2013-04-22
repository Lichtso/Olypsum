//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#include "AppMain.h"

v8::Handle<v8::Value> ScriptManager::ScriptLog(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return v8::ThrowException(v8::String::New("log(): To less arguments"));
    log(script_log, stdStrOfV8(args[0]));
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptManager::ScriptRequire(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return v8::ThrowException(v8::String::New("require(): To less arguments"));
    v8::String::AsciiValue name(args[0]->ToString());
    FilePackage* filePackage = levelManager.levelPackage;
    if(args.Length() == 2) {
        v8::String::AsciiValue filePackageName(args[1]->ToString());
        filePackage = fileManager.getPackage(*filePackageName);
    }
    if(!filePackage)
        return v8::ThrowException(v8::String::New("require(): FilePackage not found"));
    ScriptFile* script = scriptManager->getScriptFile(filePackage, *name);
    if(script->exports.IsEmpty())
        return v8::ThrowException(v8::String::New("require(): Error loading file"));
    return handleScope.Close(script->exports);
}

v8::Handle<v8::Value> ScriptManager::ScriptLoadContainer(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("loadContainer(): To less arguments"));
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
        return v8::ThrowException(v8::String::New("saveLevel(): To less arguments"));
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
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    globalTemplate->Set(v8::String::New("loadContainer"), v8::FunctionTemplate::New(ScriptLoadContainer));
    globalTemplate->Set(v8::String::New("saveLevel"), v8::FunctionTemplate::New(ScriptSaveLevel));
    globalTemplate->SetAccessor(v8::String::New("levelID"), ScriptGetLevel, ScriptSetLevel);
    globalTemplate->SetAccessor(v8::String::New("animationFactor"), ScriptGetAnimationFactor);
    scriptVector3.init(globalTemplate);
    scriptQuaternion.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
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
    scriptIntersection.init(globalTemplate);
    scriptMouse.init(globalTemplate);
    scriptKeyboard.init(globalTemplate);
    scriptGUIRect.init(globalTemplate);
    scriptGUIView.init(globalTemplate);
    scriptGUIFramedView.init(globalTemplate);
    scriptGUIScreenView.init(globalTemplate);
    scriptGUIScrollView.init(globalTemplate);
    scriptGUILabel.init(globalTemplate);
}

ScriptManager::~ScriptManager() {
    for(auto iterator : loadedScripts)
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

bool ScriptManager::checkFunctionOfScript(ScriptFile* script, const char* functionName) {
    if(!script) return false;
    v8::HandleScope handleScope;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(script->exports->GetRealNamedProperty(v8::String::New(functionName)));
    return (!function.IsEmpty() && function->IsFunction());
}

v8::Handle<v8::Value> ScriptManager::callFunctionOfScript(ScriptFile* script, const char* functionName, bool recvFirstArg, std::vector<v8::Handle<v8::Value>> args) {
    if(!script) return v8::Undefined();
    v8::HandleScope handleScope;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(script->exports->GetRealNamedProperty(v8::String::New(functionName)));
    if(function.IsEmpty() || !function->IsFunction()) return v8::Undefined();
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result;
    if(recvFirstArg && args.size() > 0 && !args[0].IsEmpty() && args[0]->IsObject())
        result = function->CallAsFunction(v8::Handle<v8::Object>::Cast(args[0]), args.size()-1, &args[1]);
    else
        result = function->CallAsFunction(script->exports, args.size(), &args[0]);
    if(scriptManager->tryCatch(&tryCatch))
        return result;
    else
        return v8::Undefined();
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

std::unique_ptr<ScriptManager> scriptManager;