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
    log(script_log, stdStringOf(args[0]->ToString()));
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

v8::Handle<v8::Value> ScriptManager::ScriptSaveLevel(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("saveLevel(): To less arguments"));
    std::string localData = (args[0]->IsString()) ? stdStringOf(args[0]->ToString()) : "",
                globalData = (args[1]->IsString()) ? stdStringOf(args[1]->ToString()) : "";
    return v8::Boolean::New(levelManager.saveLevel(localData, globalData));
}

v8::Handle<v8::Value> ScriptManager::ScriptGetLevel(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    return v8::String::New(levelManager.levelId.c_str());
}

void ScriptManager::ScriptSetLevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString())
        v8::ThrowException(v8::String::New("levelID=: Invalid argument"));
    levelManager.loadLevel(stdStringOf(value->ToString()));
}

const char* ScriptManager::cStringOf(v8::Handle<v8::String> string) {
    v8::String::Utf8Value value(string);
    return *value ? *value : "<string conversion failed>";
}

std::string ScriptManager::stdStringOf(v8::Handle<v8::String> string) {
    v8::String::Utf8Value value(string);
    return std::string(*value ? *value : "<string conversion failed>");
}

v8::Handle<v8::Value> ScriptManager::readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    if(!node || !(node = node->first_node()) || node->type() != rapidxml::node_cdata)
        return v8::Undefined();
    return v8::String::New(node->value());
}

ScriptManager::ScriptManager() {
    v8::HandleScope handleScope;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    globalTemplate->Set(v8::String::New("saveLevel"), v8::FunctionTemplate::New(ScriptSaveLevel));
    globalTemplate->SetAccessor(v8::String::New("levelID"), ScriptGetLevel, ScriptSetLevel);
    scriptVector3.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
    scriptIntersection.init(globalTemplate);
    scriptBaseObject.init(globalTemplate);
    scriptPhysicObject.init(globalTemplate);
    scriptModelObject.init(globalTemplate);
    scriptRigidObject.init(globalTemplate);
    scriptHeightfieldTerrain.init(globalTemplate);
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
    if(recvFirstArg && args.size() > 0 && !args[0].IsEmpty() && args[0]->IsObject())
        return function->CallAsFunction(v8::Handle<v8::Object>::Cast(args[0]), args.size()-1, &args[1]);
    else
        return function->CallAsFunction(script->exports, args.size(), &args[0]);
}

bool ScriptManager::tryCatch(v8::TryCatch* tryCatch) {
    if(!tryCatch->HasCaught()) return true;
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Message> message = tryCatch->Message();
    if(message.IsEmpty()) {
        log(script_log, *v8::String::Utf8Value(tryCatch->StackTrace()));
    }else{
        std::ostringstream stream;
        stream << stdStringOf(message->GetScriptResourceName()->ToString());
        stream << ':' << message->GetLineNumber() << ": ";
        stream << stdStringOf(tryCatch->Exception()->ToString()) << '\n';
        stream << stdStringOf(message->GetSourceLine()) << '\n';
        int start = message->GetStartColumn(), end = message->GetEndColumn();
        for(int i = 0; i < start; i ++)
            stream << ' ';
        for (int i = start; i < end; i ++)
            stream << '^';
        stream << '\n';
        if(!tryCatch->StackTrace().IsEmpty())
            stream << stdStringOf(tryCatch->StackTrace()->ToString());
        log(script_log, stream.str());
    }
    return false;
}

std::unique_ptr<ScriptManager> scriptManager;