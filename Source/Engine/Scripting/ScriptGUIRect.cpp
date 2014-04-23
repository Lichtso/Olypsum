//
//  ScriptGUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

template<typename T, ScriptClassName name> static JSObjectRef ScriptGUIConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUI Constructor: Expected GUIView");
    auto objectPtr = new T();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[name], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

#define ScriptGUIStaticDefinition(name) \
static bool Script##name##InstanceofCallback(JSContextRef context, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {\
    return JSValueIsObjectOfClass(context, possibleInstance, ScriptClasses[Script##name]);\
}\
JSClassDefinition Script##name##Static = {\
    0, kJSClassAttributeNone, #name,\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,\
    ScriptGUIConstructor<name, Script##name>, Script##name##InstanceofCallback, NULL\
};

ScriptGUIStaticDefinition(GUIView);
ScriptGUIStaticDefinition(GUIFramedView);
ScriptGUIStaticDefinition(GUIScrollView);
ScriptGUIStaticDefinition(GUIImage);
ScriptGUIStaticDefinition(GUILabel);
ScriptGUIStaticDefinition(GUIProgressBar);



static JSObjectRef ScriptGUIRectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "GUIRect Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(GUIRect);

static JSValueRef ScriptGUIRectGetX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIRect>(instance)->posX);
}

static bool ScriptGUIRectSetX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIRect setX(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIRect>(instance);
    if(isfinite(numberValue)) {
        objectPtr->posX = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIRectGetY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIRect>(instance)->posY);
}

static bool ScriptGUIRectSetY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIRect setY(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIRect>(instance);
    if(isfinite(numberValue)) {
        objectPtr->posY = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIRectGetWidth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIRect>(instance)->width);
}

static bool ScriptGUIRectSetWidth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIRect setWidth(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIRect>(instance);
    if(isfinite(numberValue)) {
        objectPtr->width = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIRectGetHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIRect>(instance)->height);
}

static bool ScriptGUIRectSetHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIRect setHeight(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIRect>(instance);
    if(isfinite(numberValue)) {
        objectPtr->height = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIRectGetVisible(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUIRect>(instance)->visible);
}

static bool ScriptGUIRectSetVisible(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUIRect setVisible(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUIRect>(instance)->visible = JSValueToBoolean(context, value);
    return true;
}

static JSValueRef ScriptGUIRectGetFocused(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUIRect>(instance)->isFocused());
}

static bool ScriptGUIRectSetFocused(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUIRect setFocused(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUIRect>(instance)->setFocused(JSValueToBoolean(context, value));
    return true;
}

static JSValueRef ScriptGUIRectGetParent(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return getDataOfInstance<GUIRect>(instance)->parent->scriptInstance;
}

static bool ScriptGUIRectSetParent(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsObjectOfClass(context, value, ScriptClasses[ScriptGUIView])) {
        ScriptException(context, exception, "GUIRect setParent(): Expected GUIView");
        return false;
    }
    getDataOfInstance<GUIView>(context, value)->addChild(getDataOfInstance<GUIRect>(instance));
    return true;
}

static JSValueRef ScriptGUIRectDelete(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIRect>(instance);
    objectPtr->parent->deleteChild(objectPtr->parent->getIndexOfChild(objectPtr));
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptGUIRectUpdateContent(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    getDataOfInstance<GUIRect>(instance)->updateContent();
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptGUIRectProperties[] = {
    {"x", ScriptGUIRectGetX, ScriptGUIRectSetX, kJSPropertyAttributeDontDelete},
    {"y", ScriptGUIRectGetY, ScriptGUIRectSetY, kJSPropertyAttributeDontDelete},
    {"width", ScriptGUIRectGetWidth, ScriptGUIRectSetWidth, kJSPropertyAttributeDontDelete},
    {"height", ScriptGUIRectGetHeight, ScriptGUIRectSetHeight, kJSPropertyAttributeDontDelete},
    {"visible", ScriptGUIRectGetVisible, ScriptGUIRectSetVisible, kJSPropertyAttributeDontDelete},
    {"focused", ScriptGUIRectGetFocused, ScriptGUIRectSetFocused, kJSPropertyAttributeDontDelete},
    {"parent", ScriptGUIRectGetParent, ScriptGUIRectSetParent, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptGUIRectMethods[] = {
    {"delete", ScriptGUIRectDelete, ScriptMethodAttributes},
    {"updateContent", ScriptGUIRectUpdateContent, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(GUIRect, ScriptGUIRectProperties, ScriptGUIRectMethods);