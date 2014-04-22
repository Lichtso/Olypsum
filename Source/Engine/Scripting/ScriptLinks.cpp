//
//  ScriptLinks.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptBaseLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptBaseObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptBaseObject]))
        return ScriptException(context, exception, "BaseLink Constructor: Expected BaseObject, BaseObject");
    LinkInitializer initializer;
    initializer.object[0] = getDataOfInstance<BaseObject>(context, argv[0]);
    initializer.object[1] = getDataOfInstance<BaseObject>(context, argv[1]);
    auto linkPtr = new BaseLink();
    if(linkPtr->init(initializer)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptBaseLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "BaseLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(BaseLink);

static JSValueRef ScriptBaseLinkGetObjectA(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return getDataOfInstance<BaseLink>(instance)->a->scriptInstance;
}

static JSValueRef ScriptBaseLinkGetObjectB(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return getDataOfInstance<BaseLink>(instance)->b->scriptInstance;
}

JSStaticValue ScriptBaseLinkProperties[] = {
    {"objectA", ScriptBaseLinkGetObjectA, 0, ScriptMethodAttributes},
    {"objectB", ScriptBaseLinkGetObjectB, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

ScriptClassDefinition(BaseLink, ScriptBaseLinkProperties, NULL);



static JSObjectRef ScriptTransformLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptBaseObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptBaseObject]))
        return ScriptException(context, exception, "TransformLink Constructor: Expected BaseObject, BaseObject, Matrix4, ...");
    LinkInitializer initializer;
    initializer.object[0] = getDataOfInstance<BaseObject>(context, argv[0]);
    initializer.object[1] = getDataOfInstance<BaseObject>(context, argv[1]);
    auto linkPtr = new TransformLink();
    std::vector<btTransform> transformations;
    for(unsigned int i = 2; i < argc; i ++) {
        if(!JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]))
            return ScriptException(context, exception, "TransformLink Constructor: Expected BaseObject, BaseObject, Matrix4, ...");
        transformations.push_back(getDataOfInstance<Matrix4>(context, argv[i])->getBTTransform());
    }
    if(linkPtr->init(initializer, transformations)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptBaseLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "TransformLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(TransformLink);

static JSValueRef ScriptTransformLinkAccessTransformation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto linkPtr = getDataOfInstance<TransformLink>(instance);
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "TransformLink transformation(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index >= linkPtr->transformations.size())
        return ScriptException(context, exception, "TransformLink transformation(): Out of bounds");
    btTransform& transform = linkPtr->transformations[index];
    if(argc == 2 && JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptMatrix4])) {
        Matrix4* mat = getDataOfInstance<Matrix4>(context, argv[1]);
        if(mat->isValid())
            transform = mat->getBTTransform();
        return argv[1];
    }else
        return newScriptMatrix4(context, Matrix4(transform));
}

JSStaticFunction ScriptTransformLinkMethods[] = {
    {"transformation", ScriptTransformLinkAccessTransformation, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(TransformLink, NULL, ScriptTransformLinkMethods);



static JSObjectRef ScriptBoneLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptBaseObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptBaseObject]) ||
       !JSValueIsString(context, argv[2]))
        return ScriptException(context, exception, "BoneLink Constructor: Expected BaseObject, BaseObject, String");
    LinkInitializer initializer;
    initializer.object[0] = getDataOfInstance<BaseObject>(context, argv[0]);
    initializer.object[1] = getDataOfInstance<BaseObject>(context, argv[1]);
    auto linkPtr = new BoneLink();
    ScriptString strBone(context, argv[2]);
    if(linkPtr->init(initializer, linkPtr->getBoneByName(strBone.getStdStr()))) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptBaseLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "BoneLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(BoneLink);

static JSValueRef ScriptBoneLinkGetBone(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto linkPtr = getDataOfInstance<BoneLink>(instance);
    ScriptString strBone(linkPtr->bone->name);
    return strBone.getJSStr(context);
}

static bool ScriptBoneLinkSetBone(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "boneLink setBone(): Expected String");
        return false;
    }
    auto linkPtr = getDataOfInstance<BoneLink>(instance);
    ScriptString strBone(context, value);
    Bone* bone = linkPtr->getBoneByName(strBone.getStdStr());
    if(bone) {
        linkPtr->bone = bone;
        return false;
    }else
        return true;
}

static JSValueRef ScriptBoneLinkIterateBoneChildren(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1)
        return ScriptException(context, exception, "BoneLink iterateBoneChildren: Expected Function");
    JSObjectRef callback = JSValueToObject(context, argv[0], NULL);
    if(!callback || !JSObjectIsFunction(context, callback))
        return ScriptException(context, exception, "BoneLink iterateBoneChildren: Expected Function");
    exception = NULL;
    auto linkPtr = getDataOfInstance<BoneLink>(instance);
    for(auto bone : linkPtr->bone->children) {
        ScriptString strBone(bone->name);
        JSValueRef args[] = { strBone.getJSStr(context) };
        JSObjectCallAsFunction(context, callback, callback, 1, args, exception);
        if(exception)
            return NULL;
    }
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptBoneLinkGetRelativeMat(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto linkPtr = getDataOfInstance<BoneLink>(instance);
    return newScriptMatrix4(context, Matrix4(linkPtr->bone->relativeMat));
}

static JSValueRef ScriptBoneLinkGetAbsoluteMat(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto linkPtr = getDataOfInstance<BoneLink>(instance);
    return newScriptMatrix4(context, Matrix4(linkPtr->bone->absoluteInv));
}

JSStaticValue ScriptBoneLinkProperties[] = {
    {"bone", ScriptBoneLinkGetBone, ScriptBoneLinkSetBone, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptBoneLinkMethods[] = {
    {"iterateBoneChildren", ScriptBoneLinkIterateBoneChildren, ScriptMethodAttributes},
    {"getRelativeMat", ScriptBoneLinkGetRelativeMat, ScriptMethodAttributes},
    {"getAbsoluteMat", ScriptBoneLinkGetAbsoluteMat, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(BoneLink, ScriptBoneLinkProperties, ScriptBoneLinkMethods);