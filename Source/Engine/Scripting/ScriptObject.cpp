//
//  ScriptObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

void ScriptBaseClass::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return ScriptException("BaseClass Constructor: Class can't be instantiated");
    
    BaseClass* objectPrt = static_cast<BaseClass*>(v8::Local<v8::External>::Cast(args[0])->Value());
    objectPrt->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
    args.This()->SetInternalField(0, args[0]);
    ScriptReturn(args.This());
}

void ScriptBaseClass::GetScriptClass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseClass* objectPtr = getDataOfInstance<BaseClass>(args.This());
    std::string path = fileManager.getPathOfResource(objectPtr->scriptFile->filePackage, objectPtr->scriptFile->name);
    ScriptReturn(path.c_str());
}

void ScriptBaseClass::SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    BaseClass* objectPtr = getDataOfInstance<BaseClass>(args.This());
    FilePackage* filePackage = levelManager.levelPackage;
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, stdStrOfV8(value));
    if(scriptFile)
        objectPtr->scriptFile = scriptFile;
}

void ScriptBaseClass::Delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    getDataOfInstance<BaseClass>(args.This())->removeClean();
}

ScriptBaseClass::ScriptBaseClass(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptClass(name, constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = (*functionTemplate)->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
}

ScriptBaseClass::ScriptBaseClass() :ScriptBaseClass("BaseClass") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "scriptClass", GetScriptClass, SetScriptClass);
    ScriptMethod(objectTemplate, "delete", Delete);
}



void ScriptBaseObject::AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        Matrix4* mat = scriptMatrix4->getDataOfInstance(args[0]);
        if(mat->isValid())
            objectPtr->setTransformation(mat->getBTTransform());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(Matrix4(objectPtr->getTransformation())));
}

void ScriptBaseObject::GetTransformUpLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<TransformLink*>(*i) && (*i)->b == objectPtr)
            ScriptReturn((*i)->scriptInstance);
}

void ScriptBaseObject::GetBoneUpLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<BoneLink*>(*i) && (*i)->b == objectPtr)
            ScriptReturn((*i)->scriptInstance);
}

void ScriptBaseObject::IterateLinks(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    
    if(args.Length() != 1 || !args[0]->IsFunction())
        return ScriptException("BaseObject iterateLinks: Invalid argument");
    
    v8::TryCatch tryCatch;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    for(auto link : objectPtr->links) {
        v8::Handle<v8::Value> linkInstance(*(link->scriptInstance));
        function->CallAsFunction(args.This(), 1, &linkInstance);
        if(!scriptManager->tryCatch(&tryCatch))
            break;
    }
}

void ScriptBaseObject::GetLinkCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    ScriptReturn(v8::Integer::New(v8::Isolate::GetCurrent(), objectPtr->links.size()));
}

ScriptBaseObject::ScriptBaseObject() :ScriptBaseClass("BaseObject") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptMethod(objectTemplate, "transformation", AccessTransformation);
    ScriptMethod(objectTemplate, "getTransformUpLink", GetTransformUpLink);
    ScriptMethod(objectTemplate, "getBoneUpLink", GetBoneUpLink);
    ScriptMethod(objectTemplate, "iterateLinks", IterateLinks);
    ScriptAccessor(objectTemplate, "linkCount", GetLinkCount, 0);
    
    ScriptInherit(scriptBaseClass);
}



void ScriptPhysicObject::GetCollisionShape(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    std::string buffer = levelManager.getCollisionShapeName(physicBody->getCollisionShape());
    if(buffer.size() > 0)
        ScriptReturn(buffer.c_str());
}

void ScriptPhysicObject::SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    btCollisionShape* shape = levelManager.getCollisionShape(stdStrOfV8(value));
    if(shape) {
        PhysicObject* objectPtr = getDataOfInstance<PhysicObject>(args.This());
        objectPtr->getBody()->setCollisionShape(shape);
        objectPtr->updateTouchingObjects();
    }
}

void ScriptPhysicObject::GetCollisionShapeInfo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btCollisionShape* shape = getDataOfInstance<PhysicObject>(args.This())->getBody()->getCollisionShape();
    v8::Handle<v8::Object> result = v8::Object::New(v8::Isolate::GetCurrent());
    result->Set(ScriptString("type"), ScriptString(shape->getName()));
    switch(shape->getShapeType()) {
        case CYLINDER_SHAPE_PROXYTYPE: {
            result->Set(ScriptString("size"), scriptVector3->newInstance(static_cast<btCylinderShape*>(shape)->getHalfExtentsWithMargin()));
            if(dynamic_cast<btCylinderShapeX*>(shape))
                result->Set(ScriptString("direction"), ScriptString("x"));
            else if(dynamic_cast<btCylinderShapeZ*>(shape))
            result->Set(ScriptString("direction"), ScriptString("z"));
            else
                result->Set(ScriptString("direction"), ScriptString("y"));
        } break;
        case BOX_SHAPE_PROXYTYPE: {
            result->Set(ScriptString("size"), scriptVector3->newInstance(static_cast<btBoxShape*>(shape)->getHalfExtentsWithMargin()));
        } break;
        case SPHERE_SHAPE_PROXYTYPE: {
            result->Set(ScriptString("radius"), v8::Number::New(v8::Isolate::GetCurrent(), static_cast<btSphereShape*>(shape)->getRadius()));
        } break;
        case CAPSULE_SHAPE_PROXYTYPE: {
            btCapsuleShape* capsuleShape = static_cast<btCapsuleShape*>(shape);
            result->Set(ScriptString("radius"), v8::Number::New(v8::Isolate::GetCurrent(), capsuleShape->getRadius()));
            result->Set(ScriptString("length"), v8::Number::New(v8::Isolate::GetCurrent(), capsuleShape->getHalfHeight()));
            if(dynamic_cast<btCapsuleShapeX*>(shape))
                result->Set(ScriptString("direction"), ScriptString("x"));
            else if(dynamic_cast<btCapsuleShapeZ*>(shape))
                result->Set(ScriptString("direction"), ScriptString("z"));
            else
                result->Set(ScriptString("direction"), ScriptString("y"));
        } break;
        case CONE_SHAPE_PROXYTYPE: {
            btConeShape* coneShape = static_cast<btConeShape*>(shape);
            result->Set(ScriptString("radius"), v8::Number::New(v8::Isolate::GetCurrent(), coneShape->getRadius()));
            result->Set(ScriptString("length"), v8::Number::New(v8::Isolate::GetCurrent(), coneShape->getHeight()));
            if(dynamic_cast<btConeShapeX*>(shape))
                result->Set(ScriptString("direction"), ScriptString("x"));
            else if(dynamic_cast<btConeShapeZ*>(shape))
                result->Set(ScriptString("direction"), ScriptString("z"));
            else
                result->Set(ScriptString("direction"), ScriptString("y"));
        } break;
        case MULTI_SPHERE_SHAPE_PROXYTYPE: {
            btMultiSphereShape* multiSphereShape = static_cast<btMultiSphereShape*>(shape);
            v8::Handle<v8::Array> positions = v8::Array::New(v8::Isolate::GetCurrent(), multiSphereShape->getSphereCount());
            v8::Handle<v8::Array> radii = v8::Array::New(v8::Isolate::GetCurrent(), multiSphereShape->getSphereCount());
            for(unsigned int i = 0; i < multiSphereShape->getSphereCount(); i ++) {
                positions->Set(i, scriptVector3->newInstance(multiSphereShape->getSpherePosition(i)));
                radii->Set(i, v8::Number::New(v8::Isolate::GetCurrent(), multiSphereShape->getSphereRadius(i)));
            }
            result->Set(ScriptString("positions"), positions);
            result->Set(ScriptString("radii"), radii);
        } break;
        case COMPOUND_SHAPE_PROXYTYPE: {
            btCompoundShape* compoundShape = static_cast<btCompoundShape*>(shape);
            
            v8::Handle<v8::Array> transformations = v8::Array::New(v8::Isolate::GetCurrent(), compoundShape->getNumChildShapes());
            v8::Handle<v8::Array> children = v8::Array::New(v8::Isolate::GetCurrent(), compoundShape->getNumChildShapes());
            for(unsigned int i = 0; i < compoundShape->getNumChildShapes(); i ++) {
                transformations->Set(i, scriptMatrix4->newInstance(compoundShape->getChildTransform(i)));
                std::string buffer = levelManager.getCollisionShapeName(compoundShape->getChildShape(i));
                if(buffer.size())
                    children->Set(i, ScriptString(buffer.c_str()));
            }
            result->Set(ScriptString("transformations"), transformations);
            result->Set(ScriptString("children"), children);
        } break;
        case CONVEX_HULL_SHAPE_PROXYTYPE: {
            btConvexHullShape* convexHullShape = static_cast<btConvexHullShape*>(shape);
            v8::Handle<v8::Array> points = v8::Array::New(v8::Isolate::GetCurrent(), convexHullShape->getNumPoints());
            for(unsigned int i = 0; i < convexHullShape->getNumPoints(); i ++)
                points->Set(i, scriptVector3->newInstance(convexHullShape->getUnscaledPoints()[i]));
            result->Set(ScriptString("points"), points);
        } break;
        case STATIC_PLANE_PROXYTYPE: {
            btStaticPlaneShape* staticPlaneShape = static_cast<btStaticPlaneShape*>(shape);
            result->Set(ScriptString("normal"), scriptVector3->newInstance(staticPlaneShape->getPlaneNormal()));
            result->Set(ScriptString("distance"), v8::Number::New(v8::Isolate::GetCurrent(), staticPlaneShape->getPlaneConstant()));
        } break;
        case TERRAIN_SHAPE_PROXYTYPE: {
            TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
            btVector3 size = static_cast<btHeightfieldTerrainShape*>(shape)->getLocalScaling() *
                             btVector3(objectPtr->width*0.5, 0.5, objectPtr->length*0.5);
            result->Set(ScriptString("size"), scriptVector3->newInstance(size));
        } break;
    }
    ScriptReturn(result);
}

void ScriptPhysicObject::GetAngularFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    ScriptReturn(physicBody->getRollingFriction());
}

void ScriptPhysicObject::SetAngularFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setRollingFriction(value->NumberValue());
}

void ScriptPhysicObject::GetLinearFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    ScriptReturn(physicBody->getFriction());
}

void ScriptPhysicObject::SetLinearFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setFriction(value->NumberValue());
}

void ScriptPhysicObject::GetRestitution(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    ScriptReturn(physicBody->getRestitution());
}

void ScriptPhysicObject::SetRestitution(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setRestitution(value->NumberValue());
}

ScriptPhysicObject::ScriptPhysicObject() :ScriptPhysicObject("PhysicObject") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "collisionShape", GetCollisionShape, SetCollisionShape);
    ScriptMethod(objectTemplate, "collisionShapeInfo", GetCollisionShapeInfo);
    
    ScriptInherit(scriptBaseObject);
}