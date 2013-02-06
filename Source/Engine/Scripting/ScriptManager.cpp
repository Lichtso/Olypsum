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
    v8::String::Utf8Value message(args[0]->ToString());
    log(script_log, stdStringOf(message));
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

const char* ScriptManager::cStringOf(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

std::string ScriptManager::stdStringOf(const v8::String::Utf8Value& value) {
    return std::string(*value ? *value : "<string conversion failed>");
}

ScriptManager::ScriptManager() {
    v8::HandleScope handleScope;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    scriptVector3.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
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

bool ScriptManager::tryCatch(v8::TryCatch* tryCatch) {
    if(!tryCatch->HasCaught()) return true;
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Message> message = tryCatch->Message();
    if(message.IsEmpty()) {
        log(script_log, *v8::String::Utf8Value(tryCatch->StackTrace()));
    }else{
        std::ostringstream stream;
        stream << stdStringOf(v8::String::Utf8Value(message->GetScriptResourceName()));
        stream << ':' << message->GetLineNumber() << ": ";
        stream << stdStringOf(v8::String::Utf8Value(tryCatch->Exception())) << '\n';
        stream << stdStringOf(v8::String::Utf8Value(message->GetSourceLine())) << '\n';
        int start = message->GetStartColumn(), end = message->GetEndColumn();
        for(int i = 0; i < start; i ++)
            stream << ' ';
        for (int i = start; i < end; i ++)
            stream << '^';
        stream << '\n';
        v8::String::Utf8Value stackTrace(tryCatch->StackTrace());
        if(stackTrace.length() > 0)
            stream << stdStringOf(stackTrace);
        log(script_log, stream.str());
    }
    return false;
}

std::unique_ptr<ScriptManager> scriptManager;