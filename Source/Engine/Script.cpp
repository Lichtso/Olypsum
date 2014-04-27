//
//  Script.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "Scripting/ScriptManager.h"

JSObjectRef ScriptException(JSContextRef context, JSValueRef* exception, const std::string& message) {
    ScriptString strMessage(message);
    JSValueRef str = strMessage.getJSStr(context);
    *exception = JSObjectMakeError(context, 1, &str, NULL);
    return NULL;
}

void ScriptLogException(JSContextRef context, JSValueRef exception) {
    if(!exception) return;
    JSObjectRef errorObject = JSValueToObject(context, exception, NULL);
    if(!errorObject) return;
    JSValueRef name = JSObjectGetProperty(context, errorObject, ScriptStringName.str, NULL),
               message = JSObjectGetProperty(context, errorObject, ScriptStringMessage.str, NULL),
               stack = JSObjectGetProperty(context, errorObject, ScriptStringStack.str, NULL);
    ScriptString strName(context, name);
    ScriptString strMessage(context, message);
    ScriptString strStack(context, stack);
    log(script_log, strName.getStdStr()+"\n"+strMessage.getStdStr()+"\n"+strStack.getStdStr());
}



ScriptString::ScriptString(const char* _str) {
    str = JSStringCreateWithUTF8CString(_str);
}

ScriptString::ScriptString(const std::string& _str) {
    str = JSStringCreateWithUTF8CString(_str.c_str());
}

ScriptString::ScriptString(JSContextRef context, JSValueRef value) {
    str = JSValueToStringCopy(context, value, NULL);
}

ScriptString::~ScriptString() {
    if(str) JSStringRelease(str);
}

std::string ScriptString::getStdStr() {
    auto size = JSStringGetMaximumUTF8CStringSize(str);
    char* buffer = new char[size];
    JSStringGetUTF8CString(str, buffer, size);
    std::string string(buffer);
    delete [] buffer;
    return string;
}

JSValueRef ScriptString::getJSStr(JSContextRef context) {
    return JSValueMakeString(context, str);
}



ScriptFile::ScriptFile() :context(NULL), exports(NULL) {
    
}

ScriptFile::~ScriptFile() {
    if(context)
        JSGlobalContextRelease(context);
}

FileResourcePtr<FileResource> ScriptFile::load(FilePackage* _filePackage, const std::string& name) {
    auto pointer = FileResource::load(_filePackage, name);
    if(context) return NULL;
    
    std::string filePath = filePackage->getPathOfFile("Scripts/", name)+".js";
    std::unique_ptr<char[]> data = readFile(filePath, true);
    if(!data) return NULL;
    
    JSValueRef exception = NULL;
    ScriptString strSourceCode(data.get());
    ScriptString strScriptName(fileManager.getPathOfResource(filePackage, name));
    
    context = JSGlobalContextCreateInGroup(scriptManager->contextGroup, NULL);
    JSObjectRef globalObject = JSContextGetGlobalObject(context);
    for(unsigned i = 0; i < ScriptStaticsCount; i ++)
        JSObjectSetProperty(context, globalObject, ScriptStringClassNames[i].str, scriptManager->staticObjects[i], kJSPropertyAttributeDontDelete, 0);
    JSObjectSetProperty(context, globalObject, ScriptStringExports.str, JSObjectMake(context, NULL, NULL), kJSPropertyAttributeDontDelete, 0);
    JSEvaluateScript(context, strSourceCode.str, NULL, strScriptName.str, 0, &exception);
    
    if(exception) {
        ScriptLogException(context, exception);
        return NULL;
    }else{
        exports = JSValueToObject(context, JSObjectGetProperty(context, globalObject, ScriptStringExports.str, NULL), NULL);
        retainCount ++; //Deny unload
        return pointer;
    }
}

bool ScriptFile::checkFunction(const char* functionName) {
    if(!exports) return false;
    ScriptString strFunctionName(functionName);
    JSValueRef function = JSObjectGetProperty(context, exports, strFunctionName.str, NULL);
    JSObjectRef functionObject = JSValueToObject(context, function, NULL);
    return (functionObject && JSObjectIsFunction(context, functionObject));
}

JSValueRef ScriptFile::callFunction(const char* functionName, bool recvFirstArg, int argc, JSValueRef* argv) {
    if(!exports) return NULL;
    ScriptString strFunctionName(functionName);
    JSValueRef exception = NULL, result, function = JSObjectGetProperty(context, exports, strFunctionName.str, NULL);
    JSObjectRef functionObject = JSValueToObject(context, function, NULL);
    if(!functionObject || !JSObjectIsFunction(context, functionObject)) return NULL;
    
    if(recvFirstArg && argc > 0 && argv[0] && JSValueIsObject(context, argv[0]))
        result = JSObjectCallAsFunction(context, functionObject, JSValueToObject(context, argv[0], NULL), argc-1, &argv[1], &exception);
    else
        result = JSObjectCallAsFunction(context, functionObject, exports, argc, argv, &exception);
    
    if(exception) {
        ScriptLogException(context, exception);
        return NULL;
    }else
        return result;
}



ScriptString
ScriptStringClassNames[ScriptStaticsCount],
ScriptStringAdd("add"),
ScriptStringAll("all"),
ScriptStringBottom("bottom"),
ScriptStringCenter("center"),
ScriptStringChildren("children"),
ScriptStringDelete("delete"),
ScriptStringDirection("direction"),
ScriptStringDispose("dispose"),
ScriptStringDistance("distance"),
ScriptStringEdit("edit"),
ScriptStringExports("exports"),
ScriptStringFrames("frames"),
ScriptStringHeight("height"),
ScriptStringHold("hold"),
ScriptStringHorizontal("horizontal"),
ScriptStringLeft("left"),
ScriptStringLength("length"),
ScriptStringLockable("lockable"),
ScriptStringLooping("looping"),
ScriptStringMessage("message"),
ScriptStringName("name"),
ScriptStringNone("none"),
ScriptStringNormal("normal"),
ScriptStringNormals("normals"),
ScriptStringObjects("objects"),
ScriptStringOrigin("origin"),
ScriptStringPositions("positions"),
ScriptStringRadii("radii"),
ScriptStringRadius("radius"),
ScriptStringRight("right"),
ScriptStringSize("size"),
ScriptStringStack("stack"),
ScriptStringTime("time"),
ScriptStringTop("top"),
ScriptStringTransformations("transformations"),
ScriptStringType("type"),
ScriptStringVertical("vertical"),
ScriptStringW("w"),
ScriptStringWidth("width"),
ScriptStringX("x"),
ScriptStringY("y"),
ScriptStringZ("z");

JSStringRef ScriptLinearAlgebraPropertyNames[] = { ScriptStringX.str, ScriptStringY.str, ScriptStringZ.str, ScriptStringW.str };