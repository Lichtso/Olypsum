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

const char* ScriptManager::cStringOf(v8::Handle<v8::String> string) {
    v8::String::Utf8Value value(string);
    return *value ? *value : "<string conversion failed>";
}

std::string ScriptManager::stdStringOf(v8::Handle<v8::String> string) {
    v8::String::Utf8Value value(string);
    return std::string(*value ? *value : "<string conversion failed>");
}

ScriptManager::ScriptManager() {
    v8::HandleScope handleScope;
    globalTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    globalTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(ScriptRequire));
    scriptVector3.init(globalTemplate);
    scriptMatrix4.init(globalTemplate);
    scriptBaseObject.init(globalTemplate);
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