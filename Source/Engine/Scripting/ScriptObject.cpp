//
//  ScriptObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

void ScriptBaseClass::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return args.ScriptException("BaseClass Constructor: Class can't be instantiated");
    
    BaseClass* objectPrt = static_cast<BaseClass*>(v8::Local<v8::External>::Cast(args[0])->Value());
    objectPrt->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
    args.This()->SetInternalField(0, args[0]);
    args.GetReturnValue().Set(args.This());
}

void ScriptBaseClass::GetScriptClass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseClass* objectPtr = getDataOfInstance<BaseClass>(args.This());
    std::string path = objectPtr->scriptFile->name;
    if(objectPtr->scriptFile->filePackage != levelManager.levelPackage)
        path = std::string("../")+objectPtr->scriptFile->filePackage->name+'/'+path;
    args.GetReturnValue().Set(v8::String::New(path.c_str()));
}

void ScriptBaseClass::SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    BaseClass* objectPtr = getDataOfInstance<BaseClass>(args.This());
    FilePackage* filePackage = levelManager.levelPackage;
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, stdStrOfV8(value));
    if(scriptFile)
        objectPtr->scriptFile = scriptFile;
}

void ScriptBaseClass::Delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    getDataOfInstance<BaseClass>(args.This())->removeClean();
}

ScriptBaseClass::ScriptBaseClass(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptClass(name, constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
}

ScriptBaseClass::ScriptBaseClass() :ScriptBaseClass("BaseClass") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("scriptClass"), GetScriptClass, SetScriptClass);
    objectTemplate->Set(v8::String::New("delete"), v8::FunctionTemplate::New(Delete));
}



void ScriptBaseObject::AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        Matrix4* mat = scriptMatrix4.getDataOfInstance(args[0]);
        if(mat->isValid())
            objectPtr->setTransformation(mat->getBTTransform());
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(Matrix4(objectPtr->getTransformation())));
}

void ScriptBaseObject::GetLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("BaseObject getLink(): Too few arguments");
    if(!args[0]->IsUint32())
        return args.ScriptException("BaseObject getLink(): Invalid argument");
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    if(args[0]->IntegerValue() >= objectPtr->links.size())
        return args.ScriptException("BaseObject getLink(): Out of bounds");
    args.GetReturnValue().Set((*std::next(objectPtr->links.begin(), args[0]->IntegerValue()))->scriptInstance);
}

void ScriptBaseObject::GetLinkCount(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    args.GetReturnValue().Set(v8::Integer::New(objectPtr->links.size()));
}

void ScriptBaseObject::GetTransformUpLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<TransformLink*>(*i) && (*i)->b == objectPtr) {
            args.GetReturnValue().Set((*i)->scriptInstance);
            return;
        }
}

void ScriptBaseObject::GetBoneUpLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<BoneLink*>(*i) && (*i)->b == objectPtr) {
            args.GetReturnValue().Set((*i)->scriptInstance);
            return;
        }
}

ScriptBaseObject::ScriptBaseObject() :ScriptBaseClass("BaseObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("transformation"), v8::FunctionTemplate::New(AccessTransformation));
    objectTemplate->Set(v8::String::New("getLink"), v8::FunctionTemplate::New(GetLink));
    objectTemplate->Set(v8::String::New("getLinkCount"), v8::FunctionTemplate::New(GetLinkCount));
    objectTemplate->Set(v8::String::New("getTransformUpLink"), v8::FunctionTemplate::New(GetTransformUpLink));
    objectTemplate->Set(v8::String::New("getBoneUpLink"), v8::FunctionTemplate::New(GetBoneUpLink));
    
    functionTemplate->Inherit(scriptBaseClass.functionTemplate);
}



void ScriptPhysicObject::GetCollisionShape(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    std::string buffer = levelManager.getCollisionShapeName(physicBody->getCollisionShape());
    if(buffer.size() > 0)
        args.GetReturnValue().Set(v8::String::New(buffer.c_str()));
}

void ScriptPhysicObject::SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    btCollisionShape* shape = levelManager.getCollisionShape(stdStrOfV8(value));
    if(shape) {
        PhysicObject* objectPtr = getDataOfInstance<PhysicObject>(args.This());
        objectPtr->getBody()->setCollisionShape(shape);
        objectPtr->updateTouchingObjects();
    }
}

void ScriptPhysicObject::GetCollisionShapeInfo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btCollisionShape* shape = getDataOfInstance<PhysicObject>(args.This())->getBody()->getCollisionShape();
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("type"), v8::String::New(shape->getName()));
    switch(shape->getShapeType()) {
        case CYLINDER_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("size"), scriptVector3.newInstance(static_cast<btCylinderShape*>(shape)->getHalfExtentsWithMargin()));
            if(dynamic_cast<btCylinderShapeX*>(shape))
                result->Set(v8::String::New("direction"), v8::String::New("x"));
            else if(dynamic_cast<btCylinderShapeZ*>(shape))
            result->Set(v8::String::New("direction"), v8::String::New("z"));
            else
                result->Set(v8::String::New("direction"), v8::String::New("y"));
        } break;
        case BOX_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("size"), scriptVector3.newInstance(static_cast<btBoxShape*>(shape)->getHalfExtentsWithMargin()));
        } break;
        case SPHERE_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("radius"), v8::Number::New(static_cast<btSphereShape*>(shape)->getRadius()));
        } break;
        case CAPSULE_SHAPE_PROXYTYPE: {
            btCapsuleShape* capsuleShape = static_cast<btCapsuleShape*>(shape);
            result->Set(v8::String::New("radius"), v8::Number::New(capsuleShape->getRadius()));
            result->Set(v8::String::New("length"), v8::Number::New(capsuleShape->getHalfHeight()));
            if(dynamic_cast<btCapsuleShapeX*>(shape))
                result->Set(v8::String::New("direction"), v8::String::New("x"));
            else if(dynamic_cast<btCapsuleShapeZ*>(shape))
                result->Set(v8::String::New("direction"), v8::String::New("z"));
            else
                result->Set(v8::String::New("direction"), v8::String::New("y"));
        } break;
        case CONE_SHAPE_PROXYTYPE: {
            btConeShape* coneShape = static_cast<btConeShape*>(shape);
            result->Set(v8::String::New("radius"), v8::Number::New(coneShape->getRadius()));
            result->Set(v8::String::New("length"), v8::Number::New(coneShape->getHeight()));
            if(dynamic_cast<btConeShapeX*>(shape))
                result->Set(v8::String::New("direction"), v8::String::New("x"));
            else if(dynamic_cast<btConeShapeZ*>(shape))
                result->Set(v8::String::New("direction"), v8::String::New("z"));
            else
                result->Set(v8::String::New("direction"), v8::String::New("y"));
        } break;
        case MULTI_SPHERE_SHAPE_PROXYTYPE: {
            btMultiSphereShape* multiSphereShape = static_cast<btMultiSphereShape*>(shape);
            v8::Handle<v8::Array> positions = v8::Array::New(multiSphereShape->getSphereCount());
            v8::Handle<v8::Array> radii = v8::Array::New(multiSphereShape->getSphereCount());
            for(unsigned int i = 0; i < multiSphereShape->getSphereCount(); i ++) {
                positions->Set(i, scriptVector3.newInstance(multiSphereShape->getSpherePosition(i)));
                radii->Set(i, v8::Number::New(multiSphereShape->getSphereRadius(i)));
            }
            result->Set(v8::String::New("positions"), positions);
            result->Set(v8::String::New("radii"), radii);
        } break;
        case COMPOUND_SHAPE_PROXYTYPE: {
            btCompoundShape* compoundShape = static_cast<btCompoundShape*>(shape);
            
            v8::Handle<v8::Array> transformations = v8::Array::New(compoundShape->getNumChildShapes());
            v8::Handle<v8::Array> children = v8::Array::New(compoundShape->getNumChildShapes());
            for(unsigned int i = 0; i < compoundShape->getNumChildShapes(); i ++) {
                transformations->Set(i, scriptMatrix4.newInstance(compoundShape->getChildTransform(i)));
                std::string buffer = levelManager.getCollisionShapeName(compoundShape->getChildShape(i));
                if(buffer.size())
                    children->Set(i, v8::String::New(buffer.c_str()));
            }
            result->Set(v8::String::New("transformations"), transformations);
            result->Set(v8::String::New("children"), children);
        } break;
        case CONVEX_HULL_SHAPE_PROXYTYPE: {
            btConvexHullShape* convexHullShape = static_cast<btConvexHullShape*>(shape);
            v8::Handle<v8::Array> points = v8::Array::New(convexHullShape->getNumPoints());
            for(unsigned int i = 0; i < convexHullShape->getNumPoints(); i ++)
                points->Set(i, scriptVector3.newInstance(convexHullShape->getUnscaledPoints()[i]));
            result->Set(v8::String::New("points"), points);
        } break;
        case STATIC_PLANE_PROXYTYPE: {
            btStaticPlaneShape* staticPlaneShape = static_cast<btStaticPlaneShape*>(shape);
            result->Set(v8::String::New("normal"), scriptVector3.newInstance(staticPlaneShape->getPlaneNormal()));
            result->Set(v8::String::New("distance"), v8::Number::New(staticPlaneShape->getPlaneConstant()));
        } break;
        case TERRAIN_SHAPE_PROXYTYPE: {
            TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
            btVector3 size = static_cast<btHeightfieldTerrainShape*>(shape)->getLocalScaling() *
                             btVector3(objectPtr->width*0.5, 0.5, objectPtr->length*0.5);
            result->Set(v8::String::New("size"), scriptVector3.newInstance(size));
        } break;
    }
    args.GetReturnValue().Set(result);
}

void ScriptPhysicObject::GetAngularFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    args.GetReturnValue().Set(physicBody->getRollingFriction());
}

void ScriptPhysicObject::SetAngularFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setRollingFriction(value->NumberValue());
}

void ScriptPhysicObject::GetLinearFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    args.GetReturnValue().Set(physicBody->getFriction());
}

void ScriptPhysicObject::SetLinearFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setFriction(value->NumberValue());
}

void ScriptPhysicObject::GetRestitution(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    args.GetReturnValue().Set(physicBody->getRestitution());
}

void ScriptPhysicObject::SetRestitution(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(args.This())->getBody();
    physicBody->setRestitution(value->NumberValue());
}

ScriptPhysicObject::ScriptPhysicObject() :ScriptPhysicObject("PhysicObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"), GetCollisionShape, SetCollisionShape);
    objectTemplate->Set(v8::String::New("collisionShapeInfo"), v8::FunctionTemplate::New(GetCollisionShapeInfo));
    
    functionTemplate->Inherit(scriptBaseObject.functionTemplate);
}