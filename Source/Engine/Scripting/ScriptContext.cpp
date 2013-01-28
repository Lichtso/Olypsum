//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#include "AppMain.h"

static v8::Handle<v8::Value> ScriptLog(const v8::Arguments& args) {
    v8::String::AsciiValue message(args[0]->ToString());
    log(script_log, *message);
    return v8::Undefined();
}

ScriptContext::ScriptContext() {
    v8::HandleScope handleScope;
    globalTemplate = v8::ObjectTemplate::New();
    globalTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(ScriptLog));
    context = v8::Context::New(NULL, globalTemplate);
    context->Enter();
    
    /*v8::Local<v8::Object> globalObject = context->Global();
    v8::Local<v8::FunctionTemplate> logTemplate = v8::FunctionTemplate::New(ScriptLog);
    globalObject->Set(v8::String::New("log"), logTemplate->GetFunction());*/
    
    Script script("(function() { log('Hello' + ', World!'); return true; })();", "Test.js");
    v8::String::AsciiValue ascii(script.run());
    log(script_log, *ascii);
}

ScriptContext::~ScriptContext() {
    context.Dispose();
}

const char* ScriptContext::cStringOf(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

std::string ScriptContext::stdStringOf(const v8::String::Utf8Value& value) {
    return std::string(*value ? *value : "<string conversion failed>");
}

bool ScriptContext::tryCatch(v8::TryCatch* tryCatch) {
    if(!tryCatch->HasCaught()) return true;
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Message> message = tryCatch->Message();
    if(message.IsEmpty()) {
        log(script_log, *v8::String::Utf8Value(tryCatch->StackTrace()));
    }else{
        std::ostringstream stream;
        stream << stdStringOf(v8::String::Utf8Value(message->GetScriptResourceName()));
        stream << ':';
        stream << message->GetLineNumber();
        stream << ": ";
        stream << stdStringOf(v8::String::Utf8Value(tryCatch->Exception()));
        stream << '\n';
        stream << stdStringOf(v8::String::Utf8Value(message->GetSourceLine()));
        stream << '\n';
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

std::unique_ptr<ScriptContext> scriptContext;