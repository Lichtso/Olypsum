//
//  ScriptGUIOutput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSValueRef ScriptGUILabelGetTextAlignment(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    switch(objectPtr->textAlignment) {
        case GUILabel::TextAlignment::Left:
            return ScriptStringLeft.getJSStr(context);
        case GUILabel::TextAlignment::Center:
            return ScriptStringCenter.getJSStr(context);
        case GUILabel::TextAlignment::Right:
            return ScriptStringRight.getJSStr(context);
        default:
            return ScriptException(context, exception, "GUILabel getTextAlignment(): Internal error");
    }
}

static bool ScriptGUILabelSetTextAlignment(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUILabel setTextAlignment(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    if(str == "left")
        objectPtr->textAlignment = GUILabel::TextAlignment::Left;
    else if(str == "center")
        objectPtr->textAlignment = GUILabel::TextAlignment::Center;
    else if(str == "right")
        objectPtr->textAlignment = GUILabel::TextAlignment::Right;
    else {
        ScriptException(context, exception, "GUILabel setTextAlignment(): Invalid value");
        return false;
    }
    return true;
}

static JSValueRef ScriptGUILabelViewGetText(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    ScriptString strText(getDataOfInstance<GUILabel>(instance)->text);
    return strText.getJSStr(context);
}

static bool ScriptGUILabelViewSetText(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUILabel setText(): Expected String");
        return false;
    }
    ScriptString strText(context, value);
    getDataOfInstance<GUILabel>(instance)->text = strText.getStdStr();
    return true;
}

static JSValueRef ScriptGUILabelViewGetFont(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    std::string name;
    FilePackage* filePackage = fileManager.findResource<TextFont>(objectPtr->font, name);
    if(!filePackage)
        return ScriptException(context, exception, "GUILabel getFont(): Internal error");
    ScriptString strFont(fileManager.getPathOfResource(filePackage, name));
    return strFont.getJSStr(context);
}

static bool ScriptGUILabelViewSetFont(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUILabel setFont(): Expected String");
        return false;
    }
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    ScriptString strFont(context, value);
    auto font = fileManager.getResourceByPath<TextFont>(levelManager.levelPackage, strFont.getStdStr());
    if(font) {
        objectPtr->font = font;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUILabelViewGetFontHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUILabel>(instance)->fontHeight);
}

static bool ScriptGUILabelViewSetFontHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUILabel setFontHeight(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    if(isfinite(numberValue)) {
        objectPtr->fontHeight = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUILabelViewAccessColor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUILabel>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) {
        objectPtr->color = getScriptQuaternion(context, argv[0]);
        return argv[0];
    }else
        return newScriptQuaternion(context, objectPtr->color.getQuaternion());
}

JSStaticValue ScriptGUILabelProperties[] = {
    {"sizeAlignment", ScriptGUIGetSizeAlignment<GUILabel>, ScriptGUISetSizeAlignment<GUILabel>, kJSPropertyAttributeDontDelete},
    {"textAlignment", ScriptGUILabelGetTextAlignment, ScriptGUILabelSetTextAlignment, kJSPropertyAttributeDontDelete},
    {"text", ScriptGUILabelViewGetText, ScriptGUILabelViewSetText, kJSPropertyAttributeDontDelete},
    {"font", ScriptGUILabelViewGetFont, ScriptGUILabelViewSetFont, kJSPropertyAttributeDontDelete},
    {"fontHeight", ScriptGUILabelViewGetFontHeight, ScriptGUILabelViewSetFontHeight, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptGUILabelMethods[] = {
    {"color", ScriptGUILabelViewAccessColor, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(GUILabel, ScriptGUILabelProperties, ScriptGUILabelMethods);



static JSValueRef ScriptGUIProgressBarGetValue(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIProgressBar>(instance)->value);
}

static bool ScriptGUIProgressBarSetValue(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUILabel setValue(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIProgressBar>(instance);
    if(isfinite(numberValue)) {
        objectPtr->value = clamp(numberValue, 0.0, 1.0);
        return true;
    }else
        return false;
}

JSStaticValue ScriptGUIProgressBarProperties[] = {
    {"orientation", ScriptGUIGetOrientation<GUIProgressBar>, ScriptGUISetOrientationDual<GUIProgressBar>, kJSPropertyAttributeDontDelete},
    {"value", ScriptGUIProgressBarGetValue, ScriptGUIProgressBarSetValue, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassDefinition(GUIProgressBar, ScriptGUIProgressBarProperties, NULL);



static JSObjectRef ScriptGUIImageConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptGUIView]) || !JSValueIsString(context, argv[1]))
        return ScriptException(context, exception, "GUIImage Constructor: Expected GUIView, String");
    ScriptString strFont(context, argv[1]);
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strFont.getStdStr());
    if(texture) {
        auto objectPtr = new GUIImage();
        objectPtr->texture = texture;
        getDataOfInstance<GUIView>(context, argv[0])->addChild(objectPtr);
        objectPtr->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUIImage], objectPtr);
        JSValueProtect(scriptManager->mainScript->context, objectPtr->scriptInstance);
        return objectPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "GUIImage Constructor: Invalid image");
}

ScriptClassStaticDefinition(GUIImage);

static JSValueRef ScriptGUIImageGetImage(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIImage>(instance);
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(!filePackage)
        return ScriptException(context, exception, "GUIImage getImage(): Internal error");
    ScriptString strFont(fileManager.getPathOfResource(filePackage, name));
    return strFont.getJSStr(context);
}

static bool ScriptGUIImageSetImage(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUIImage setImage(): Expected String");
        return false;
    }
    auto objectPtr = getDataOfInstance<GUIImage>(instance);
    ScriptString strFont(context, value);
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strFont.getStdStr());
    if(texture) {
        objectPtr->texture = texture;
        return true;
    }else
        return false;
}

JSStaticValue ScriptGUIImageProperties[] = {
    {"sizeAlignment", ScriptGUIGetSizeAlignment<GUIImage>, ScriptGUISetSizeAlignment<GUIImage>, kJSPropertyAttributeDontDelete},
    {"image", ScriptGUIImageGetImage, ScriptGUIImageSetImage, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassDefinition(GUIImage, ScriptGUIImageProperties, NULL);