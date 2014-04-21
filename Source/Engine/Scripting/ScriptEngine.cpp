//
//  ScriptEngine.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 19.04.14.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSValueRef ScriptEngineGetGamePaused(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, menu.current != Menu::inGame);
}

static JSValueRef ScriptEngineGetLevel(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    ScriptString strLevel(levelManager.levelContainer);
    return strLevel.getJSStr(context);
}

static bool ScriptEngineSetLevel(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "Engine setLevel(): Expected String");
        return false;
    }
    ScriptString strLevel(context, value);
    levelManager.levelContainer = strLevel.getStdStr();
    LevelLoader levelLoader;
    return levelLoader.loadLevel();
}

static JSValueRef ScriptEngineLog(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0)
        return ScriptException(context, exception, "Engine log(): Expected at least one argument");
    ScriptString strMessage(context, argv[0]);
    log(script_log, strMessage.getStdStr());
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptEngineGetScript(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsString(context, argv[0]))
        return ScriptException(context, exception, "Engine getScript(): Expected String");
    FilePackage* filePackage = levelManager.levelPackage;
    ScriptString strName(context, argv[0]);
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, strName.getStdStr());
    if(!scriptFile)
        return ScriptException(context, exception, "Engine getScript(): Error loading file");
    else
        return scriptFile->exports;
}

static JSValueRef ScriptEngineLoadContainer(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 2 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) || !JSValueIsString(context, argv[1]))
        return ScriptException(context, exception, "Engine loadContainer(): Expected Matrix4, String");
    LevelLoader levelLoader;
    levelLoader.transformation = getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform();
    ScriptString strName(context, argv[1]);
    if(levelLoader.loadContainer(strName.getStdStr(), false))
        return levelLoader.getResultsArray(context);
    else
        return JSValueMakeNull(context);
}

static JSValueRef ScriptEngineLocalizeString(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsString(context, argv[0]))
        return ScriptException(context, exception, "Engine localizeString(): Expected String");
    ScriptString strKey(context, argv[0]);
    ScriptString strValue(fileManager.localizeString(strKey.getStdStr()));
    return strValue.getJSStr(context);
}

static JSValueRef ScriptEngineSaveLevel(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 || !JSValueIsString(context, argv[0]))
        return ScriptException(context, exception, "Engine saveLevel(): Expected 3 arguments (localData, globalData, description)");
    LevelSaver levelSaver;
    return JSValueMakeBoolean(context, levelSaver.saveLevel(context, argv[0], argv[1], argv[2]));
}

static JSValueRef ScriptEngineAccessSceneProperty(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsString(context, argv[0]))
        return ScriptException(context, exception, "Engine accessSceneProperty(): Expected String");
    ScriptString srtKey(context, argv[0]);
    std::string key = srtKey.getStdStr();
    if(key == "Gravity") {
        if(argc > 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
            objectManager.physicsWorld->setGravity(getScriptVector3(context, argv[1]));
            return argv[1];
        }else
            return newScriptVector3(context, objectManager.physicsWorld->getGravity());
    }else if(key == "Ambient") {
        if(argc > 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
            objectManager.sceneAmbient = getScriptVector3(context, argv[1]);
            return argv[1];
        }else
            return newScriptVector3(context, objectManager.sceneAmbient);
    }else if(key == "FogColor") {
        if(argc > 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
            objectManager.sceneFogColor = getScriptVector3(context, argv[1]);
            return argv[1];
        }else
            return newScriptVector3(context, objectManager.sceneFogColor);
    }else if(key == "FogDistance") {
        if(argc > 1 && JSValueIsNumber(context, argv[1])) {
            objectManager.sceneFogDistance = JSValueToNumber(context, argv[1], NULL);
            return argv[1];
        }else
            return JSValueMakeNumber(context, objectManager.sceneFogDistance);
    }else
        return ScriptException(context, exception, "Engine accessSceneProperty(): Invalid key");
}

JSStaticValue ScriptEngineProperties[] = {
    {"gamePaused", ScriptEngineGetGamePaused, 0, ScriptMethodAttributes},
    {"level", ScriptEngineGetLevel, ScriptEngineSetLevel, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptEngineMethods[] = {
    {"log", ScriptEngineLog, ScriptMethodAttributes},
    {"getScript", ScriptEngineGetScript, ScriptMethodAttributes},
    {"loadContainer", ScriptEngineLoadContainer, ScriptMethodAttributes},
    {"localizeString", ScriptEngineLocalizeString, ScriptMethodAttributes},
    {"saveLevel", ScriptEngineSaveLevel, ScriptMethodAttributes},
    {"accessSceneProperty", ScriptEngineAccessSceneProperty, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Engine, ScriptEngineProperties, ScriptEngineMethods);