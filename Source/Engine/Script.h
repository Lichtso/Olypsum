//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef Script_h
#define Script_h

#include "FBO.h"
#define ScriptMethodAttributes (kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete)
#define ScriptInstance(name) scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[Script##name], this);\
JSValueProtect(scriptManager->mainScript->context, scriptInstance)
#define ScriptClassStaticDefinition(name) \
static bool Script##name##InstanceofCallback(JSContextRef context, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {\
    return JSValueIsObjectOfClass(context, possibleInstance, ScriptClasses[Script##name]);\
}\
JSClassDefinition Script##name##Static = {\
    0, kJSClassAttributeNone, #name,\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,\
    Script##name##Constructor, Script##name##InstanceofCallback, NULL\
};
#define ScriptClassDefinition(name, properties, methods) \
JSClassDefinition Script##name##Definition = {\
    0, kJSClassAttributeNone, #name,\
    NULL, properties, methods,\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL\
};

#define ScriptClassesCount 39
#define ScriptStaticsCount (ScriptClassesCount+5)

JSObjectRef ScriptException(JSContextRef context, JSValueRef* exception, const std::string& message);
void ScriptLogException(JSContextRef context, JSValueRef exception);

template<typename T> static T* getDataOfInstance(JSObjectRef instance) {
    return (instance) ? static_cast<T*>(JSObjectGetPrivate(instance)) : NULL;
}

template<typename T> static T* getDataOfInstance(JSContextRef context, JSValueRef value) {
    if(!value) return NULL;
    JSObjectRef instance = JSValueToObject(context, value, NULL);
    return (instance) ? static_cast<T*>(JSObjectGetPrivate(instance)) : NULL;
}

class ScriptString {
    public:
    JSStringRef str;
    ScriptString() :str(0) { };
    ScriptString(const char* str);
    ScriptString(const std::string& str);
    ScriptString(JSContextRef context, JSValueRef value);
    ~ScriptString();
    std::string getStdStr();
    JSValueRef getJSStr(JSContextRef context);
};

class ScriptFile : public FileResource {
    public:
    ScriptFile();
    ~ScriptFile();
    JSObjectRef exports;
    JSGlobalContextRef context;
    FileResourcePtr<FileResource> load(FilePackage* filePackage, const std::string& name);
    bool checkFunction(const char* functionName);
    JSValueRef callFunction(const char* functionName, bool recvFirstArg, int argc, JSValueRef* argv);
};

extern ScriptString ScriptStringClassNames[],
ScriptStringAll,
ScriptStringChildren,
ScriptStringDirection,
ScriptStringDistance,
ScriptStringExports,
ScriptStringHeight,
ScriptStringLength,
ScriptStringMessage,
ScriptStringName,
ScriptStringNormal,
ScriptStringNormals,
ScriptStringObjects,
ScriptStringOrigin,
ScriptStringPositions,
ScriptStringRadii,
ScriptStringRadius,
ScriptStringSize,
ScriptStringStack,
ScriptStringTransformations,
ScriptStringType,
ScriptStringW,
ScriptStringWidth,
ScriptStringX,
ScriptStringY,
ScriptStringZ;
extern JSStringRef ScriptLinearAlgebraPropertyNames[4];

#endif