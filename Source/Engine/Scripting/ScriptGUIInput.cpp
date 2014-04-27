//
//  ScriptGUIInput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static bool ScriptGUIHandleCallback(const char* name, GUIRect* objectPtr, size_t argc, const JSValueRef* argv) {
    JSContextRef context = scriptManager->mainScript->context;
    ScriptString strFunctionName(name);
    JSValueRef function = JSObjectGetProperty(context, objectPtr->scriptInstance, strFunctionName.str, NULL);
    if(!function) return false;
    JSObjectRef functionObject = JSValueToObject(context, function, NULL);
    if(!functionObject) return false;
    JSValueRef exception = NULL;
    JSObjectCallAsFunction(context, functionObject, objectPtr->scriptInstance, argc, argv, &exception);
    if(exception) {
        ScriptLogException(context, exception);
        return false;
    }else
        return true;
}

static JSObjectRef ScriptGUIButtonConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUIButton Constructor: Expected GUIView");
    auto objectPtr = new GUIButton();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->onClick = [](GUIButton* objectPtr) {
        ScriptGUIHandleCallback("onclick", objectPtr, 0, NULL);
    };
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUIButton], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

ScriptClassStaticDefinition(GUIButton);

static JSValueRef ScriptGUIButtonGetPaddingX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIButton>(instance)->paddingX);
}

static bool ScriptGUIButtonSetPaddingX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIButton setPaddingX(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    if(isfinite(numberValue)) {
        objectPtr->paddingX = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIButtonGetPaddingY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIButton>(instance)->paddingY);
}

static bool ScriptGUIButtonSetPaddingY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIButton setPaddingY(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    if(isfinite(numberValue)) {
        objectPtr->paddingY = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIButtonGetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUIButton>(instance)->enabled);
}

static bool ScriptGUIButtonSetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUIButton setEnabled(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUIButton>(instance)->enabled = JSValueToBoolean(context, value);
    return true;
}

ScriptString ScriptStringReleased("released"), ScriptStringHighlighted("highlighted"), ScriptStringPressed("pressed");

static JSValueRef ScriptGUIButtonGetState(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    switch(objectPtr->state) {
        case GUIButton::State::Released:
            return ScriptStringReleased.getJSStr(context);
        case GUIButton::State::Highlighted:
            return ScriptStringHighlighted.getJSStr(context);
        case GUIButton::State::Pressed:
            return ScriptStringPressed.getJSStr(context);
        default:
            return ScriptException(context, exception, "GUIButton getState(): Internal error");
    }
}

static bool ScriptGUIButtonSetState(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUIButton setState(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    if(str == ScriptStringReleased.getStdStr())
        objectPtr->state = GUIButton::State::Released;
    else if(str == ScriptStringHighlighted.getStdStr())
        objectPtr->state = GUIButton::State::Highlighted;
    else if(str == ScriptStringPressed.getStdStr())
        objectPtr->state = GUIButton::State::Pressed;
    else{
        ScriptException(context, exception, "GUIButton setState(): Invalid value");
        return false;
    }
    return true;
}

static JSValueRef ScriptGUIButtonGetType(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    switch(objectPtr->type) {
        case GUIButton::Type::Normal:
            return ScriptStringNormal.getJSStr(context);
        case GUIButton::Type::Delete:
            return ScriptStringDelete.getJSStr(context);
        case GUIButton::Type::Add:
            return ScriptStringAdd.getJSStr(context);
        case GUIButton::Type::Edit:
            return ScriptStringEdit.getJSStr(context);
        case GUIButton::Type::Lockable:
            return ScriptStringLockable.getJSStr(context);
        default:
            return ScriptException(context, exception, "GUIButton getType(): Internal error");
    }
}

static bool ScriptGUIButtonSetType(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUIButton setType(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<GUIButton>(instance);
    if(str == ScriptStringNormal.getStdStr())
        objectPtr->type = GUIButton::Type::Normal;
    else if(str == ScriptStringDelete.getStdStr())
        objectPtr->type = GUIButton::Type::Delete;
    else if(str == ScriptStringAdd.getStdStr())
        objectPtr->type = GUIButton::Type::Add;
    else if(str == ScriptStringEdit.getStdStr())
        objectPtr->type = GUIButton::Type::Edit;
    else if(str == ScriptStringLockable.getStdStr())
        objectPtr->type = GUIButton::Type::Lockable;
    else{
        ScriptException(context, exception, "GUIButton setType(): Invalid value");
        return false;
    }
    return true;
}

JSStaticValue ScriptGUIButtonProperties[] = {
    {"sizeAlignment", ScriptGUIGetSizeAlignment<GUIButton>, ScriptGUISetSizeAlignment<GUIButton>, kJSPropertyAttributeDontDelete},
    {"paddingX", ScriptGUIButtonGetPaddingX, ScriptGUIButtonSetPaddingX, kJSPropertyAttributeDontDelete},
    {"paddingY", ScriptGUIButtonGetPaddingY, ScriptGUIButtonSetPaddingY, kJSPropertyAttributeDontDelete},
    {"enabled", ScriptGUIButtonGetEnabled, ScriptGUIButtonSetEnabled, kJSPropertyAttributeDontDelete},
    {"state", ScriptGUIButtonGetState, ScriptGUIButtonSetState, kJSPropertyAttributeDontDelete},
    {"type", ScriptGUIButtonGetType, ScriptGUIButtonSetType, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassDefinition(GUIButton, ScriptGUIButtonProperties, NULL);



static JSObjectRef ScriptGUICheckBoxConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUICheckBox Constructor: Expected GUIView");
    auto objectPtr = new GUICheckBox();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->onClick = [](GUICheckBox* objectPtr) {
        ScriptGUIHandleCallback("onclick", objectPtr, 0, NULL);
    };
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUICheckBox], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

ScriptClassStaticDefinition(GUICheckBox);

ScriptClassDefinition(GUICheckBox, NULL, NULL);



static JSObjectRef ScriptGUITabsConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUITabs Constructor: Expected GUIView");
    auto objectPtr = new GUITabs();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->onChange = [](GUITabs* objectPtr) {
        ScriptGUIHandleCallback("onchange", objectPtr, 0, NULL);
    };
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUITabs], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

static JSValueRef ScriptGUITabsGetSelected(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUITabs>(instance)->selected);
}

static bool ScriptGUITabsSetSelected(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUITabs setSelected(): Expected Number");
        return false;
    }
    int numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUITabs>(instance);
    if(numberValue < objectPtr->children.size()) {
        objectPtr->selected = std::max(-1, numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUITabsGetDeactivatable(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUITabs>(instance)->deactivatable);
}

static bool ScriptGUITabsSetDeactivatable(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUITabs setDeactivatable(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUITabs>(instance)->deactivatable = JSValueToBoolean(context, value);
    return true;
}

JSStaticValue ScriptGUITabsProperties[] = {
    {"sizeAlignment", ScriptGUIGetSizeAlignment<GUITabs>, ScriptGUISetSizeAlignment<GUITabs>, kJSPropertyAttributeDontDelete},
    {"orientation",  ScriptGUIGetOrientation<GUITabs>,  ScriptGUISetOrientation<GUITabs>, kJSPropertyAttributeDontDelete},
    {"selected", ScriptGUITabsGetSelected, ScriptGUITabsSetSelected, kJSPropertyAttributeDontDelete},
    {"deactivatable", ScriptGUITabsGetDeactivatable, ScriptGUITabsSetDeactivatable, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassStaticDefinition(GUITabs);

ScriptClassDefinition(GUITabs, ScriptGUITabsProperties, NULL);



static JSObjectRef ScriptGUISliderConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUISlider Constructor: Expected GUIView");
    auto objectPtr = new GUISlider();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->onChange = [](GUISlider* objectPtr, bool changing) {
        JSValueRef argv[] = { JSValueMakeBoolean(scriptManager->mainScript->context, changing) };
        ScriptGUIHandleCallback("onchange", objectPtr, 1, argv);
    };
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUISlider], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

static JSValueRef ScriptGUISliderGetValue(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUISlider>(instance)->value);
}

static bool ScriptGUISliderSetValue(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUISlider setValue(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUISlider>(instance);
    if(isfinite(numberValue)) {
        objectPtr->value = clamp(numberValue, 0.0, 1.0);
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUISliderGetSteps(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUISlider>(instance)->steps);
}

static bool ScriptGUISliderSetSteps(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUISlider setSteps(): Expected Number");
        return false;
    }
    getDataOfInstance<GUISlider>(instance)->steps = JSValueToNumber(context, value, NULL);
    return true;
}

static JSValueRef ScriptGUISliderGetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUISlider>(instance)->enabled);
}

static bool ScriptGUISliderSetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUISlider setEnabled(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUISlider>(instance)->enabled = JSValueToBoolean(context, value);
    return true;
}

JSStaticValue ScriptGUISliderProperties[] = {
    {"orientation",  ScriptGUIGetOrientation<GUISlider>,  ScriptGUISetOrientationDual<GUISlider>, kJSPropertyAttributeDontDelete},
    {"value", ScriptGUISliderGetValue, ScriptGUISliderSetValue, kJSPropertyAttributeDontDelete},
    {"steps", ScriptGUISliderGetSteps, ScriptGUISliderSetSteps, kJSPropertyAttributeDontDelete},
    {"enabled", ScriptGUISliderGetEnabled, ScriptGUISliderSetEnabled, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassStaticDefinition(GUISlider);

ScriptClassDefinition(GUISlider, ScriptGUISliderProperties, NULL);



static JSObjectRef ScriptGUITextFieldConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]))
        return ScriptException(context, exception, "GUITextField Constructor: Expected GUIView");
    auto objectPtr = new GUITextField();
    getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
    objectPtr->onChange = [](GUITextField* objectPtr) {
        ScriptGUIHandleCallback("onchange", objectPtr, 0, NULL);
    };
    objectPtr->onFocus = [](GUITextField* objectPtr) {
        ScriptGUIHandleCallback("onfocus", objectPtr, 0, NULL);
    };
    auto label = static_cast<GUILabel*>(objectPtr->children[0]);
    label->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUILabel], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, label->scriptInstance);
    objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUITextField], objectPtr);
    JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
    return objectPtr->scriptInstance;
}

static JSValueRef ScriptGUITextFieldGetCursorX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUITextField>(instance)->getCursorX());
}

static bool ScriptGUITextFieldSetCursorX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUITextField setCursorX(): Expected Number");
        return false;
    }
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    getDataOfInstance<GUITextField>(instance)->setCursorX(numberValue);
    return true;
}

static JSValueRef ScriptGUITextFieldGetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<GUITextField>(instance)->enabled);
}

static bool ScriptGUITextFieldSetEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "GUITextField setEnabled(): Expected Boolean");
        return false;
    }
    getDataOfInstance<GUITextField>(instance)->enabled = JSValueToBoolean(context, value);
    return true;
}

static JSValueRef ScriptGUITextFieldGetText(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUITextField>(instance);
    ScriptString strText(static_cast<GUILabel*>(objectPtr->children[0])->text);
    return strText.getJSStr(context);
}

static bool ScriptGUITextFieldSetText(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUITextField setText(): Expected String");
        return false;
    }
    ScriptString strText(context, value);
    auto objectPtr = getDataOfInstance<GUITextField>(instance);
    static_cast<GUILabel*>(objectPtr->children[0])->text = strText.getStdStr();
    return true;
}

JSStaticValue ScriptGUITextFieldProperties[] = {
    {"cursorX", ScriptGUITextFieldGetCursorX, ScriptGUITextFieldSetCursorX, kJSPropertyAttributeDontDelete},
    {"enabled", ScriptGUITextFieldGetEnabled, ScriptGUITextFieldSetEnabled, kJSPropertyAttributeDontDelete},
    {"text", ScriptGUITextFieldGetText, ScriptGUITextFieldSetText, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassStaticDefinition(GUITextField);

ScriptClassDefinition(GUITextField, ScriptGUITextFieldProperties, NULL);