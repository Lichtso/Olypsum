//
//  ScriptIO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "AppMain.h"

static JSValueRef ScriptMouseGetX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, menu.mouseX);
}

static bool ScriptMouseSetX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "Mouse setX(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= -menu.screenView->width && numberValue <= menu.screenView->width) {
        menu.mouseX = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptMouseGetY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, menu.mouseY);
}

static bool ScriptMouseSetY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "Mouse setY(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= -menu.screenView->height && numberValue <= menu.screenView->height) {
        menu.mouseY = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptMouseGetIsFixed(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, menu.mouseFixed);
}

static bool ScriptMouseSetIsFixed(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "Mouse setIsFixed(): Expected Boolean");
        return false;
    }
    menu.mouseFixed = JSValueToBoolean(context, value);
    return true;
}

JSStaticValue ScriptMouseProperties[] = {
    {"x", ScriptMouseGetX, ScriptMouseSetX, kJSPropertyAttributeDontDelete},
    {"y", ScriptMouseGetY, ScriptMouseSetY, kJSPropertyAttributeDontDelete},
    {"isFixed", ScriptMouseGetIsFixed, ScriptMouseSetIsFixed, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptMouseMethods[] = {
    {0, 0, 0}
};

ScriptClassDefinition(Mouse, ScriptMouseProperties, ScriptMouseMethods);



static JSValueRef ScriptKeyboardGetKeyCount(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, keyStateSize);
}

static JSValueRef ScriptKeyboardIsKeyPressed(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Keyboard isKeyPressed(): Expected Number");
    unsigned int key = JSValueToNumber(context, argv[0], NULL);
    if(key >= keyStateSize)
        return ScriptException(context, exception, "Keyboard isKeyPressed(): Out of bounds");
    return JSValueMakeBoolean(context, keyState[key]);
}

JSStaticValue ScriptKeyboardProperties[] = {
    {"keyCount", ScriptKeyboardGetKeyCount, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptKeyboardMethods[] = {
    {"isKeyPressed", ScriptKeyboardIsKeyPressed, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Keyboard, ScriptKeyboardProperties, ScriptKeyboardMethods);