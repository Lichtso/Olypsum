//
//  ScriptDisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptDisplayObject.h"

v8::Handle<v8::Value> ScriptGraphicObject::GetIntegrity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GraphicObject* objectPtr = getDataOfInstance<GraphicObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->integrity));
}

void ScriptGraphicObject::SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    GraphicObject* objectPtr = getDataOfInstance<GraphicObject>(info.This());
    if(objectPtr->integrity <= 0.0) return;
    objectPtr->integrity = fmax(0.0, value->NumberValue());
}

v8::Handle<v8::Value> ScriptGraphicObject::AttachDecal(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 3 || !args[0]->IsNumber() || !scriptMatrix4.isCorrectInstance(args[1]) || !args[2]->IsString())
        return v8::ThrowException(v8::String::New("GraphicObject attachDecal: Invalid argument"));
    
    Decal* decal = new Decal();
    decal->life = args[0]->NumberValue();
    decal->transformation = scriptMatrix4.getDataOfInstance(args[1])->getBTTransform();
    decal->diffuse = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[2]));
    
    if(args.Length() > 3 && args[3]->IsString())
        decal->heightMap = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[3]));
    
    GraphicObject* objectPtr = getDataOfInstance<GraphicObject>(args.This());
    objectPtr->decals.insert(decal);
    return v8::Undefined();
}

ScriptGraphicObject::ScriptGraphicObject() :ScriptPhysicObject("GraphicObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("integrity"), GetIntegrity, SetIntegrity);
    objectTemplate->Set(v8::String::New("attachDecal"), v8::FunctionTemplate::New(AttachDecal));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptRigidObject::GetModel(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Model>(objectPtr->model, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptRigidObject::SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(info.This());
    auto model = fileManager.getResourceByPath<Model>(levelManager.levelPackage, stdStrOfV8(value));
    if(model) objectPtr->setModel(NULL, model);
}

v8::Handle<v8::Value> ScriptRigidObject::GetMass(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New((body->getInvMass() == 0.0) ? 0.0 : 1.0/body->getInvMass()));
}

void ScriptRigidObject::SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() < 0.0 || value->NumberValue() == NAN) return;
    btVector3 inertia;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->getCollisionShape()->calculateLocalInertia(value->NumberValue(), inertia);
    body->setMassProps(value->NumberValue(), inertia);
    body->activate();
}

v8::Handle<v8::Value> ScriptRigidObject::GetKinematic(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(info.This());
    return handleScope.Close(v8::Boolean::New(objectPtr->getKinematic()));
}

void ScriptRigidObject::SetKinematic(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(info.This());
    objectPtr->setKinematic(value->BooleanValue());
}

v8::Handle<v8::Value> ScriptRigidObject::AccessAngularVelocity(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec)) {
            body->setAngularVelocity(vec);
            body->activate();
        }
        return args[0];
    }
    return handleScope.Close(scriptVector3.newInstance(body->getAngularVelocity()));
}

v8::Handle<v8::Value> ScriptRigidObject::AccessLinearVelocity(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec)) {
            body->setLinearVelocity(vec);
            body->activate();
        }
        return args[0];
    }
    return handleScope.Close(scriptVector3.newInstance(body->getLinearVelocity()));
}

v8::Handle<v8::Value> ScriptRigidObject::AccessAngularFactor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec))
            body->setAngularFactor(vec);
        return args[0];
    }
    return handleScope.Close(scriptVector3.newInstance(body->getAngularFactor()));
}

v8::Handle<v8::Value> ScriptRigidObject::AccessLinearFactor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec))
            body->setLinearFactor(vec);
        return args[0];
    }
    return handleScope.Close(scriptVector3.newInstance(body->getLinearFactor()));
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New(body->getAngularDamping()));
}

void ScriptRigidObject::SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setAngularDamping(value->NumberValue());
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New(body->getLinearDamping()));
}

void ScriptRigidObject::SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setLinearDamping(value->NumberValue());
}

v8::Handle<v8::Value> ScriptRigidObject::AccessTransformation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        Matrix4* mat = scriptMatrix4.getDataOfInstance(args[0]);
        if(mat->isValid()) {
            objectPtr->setTransformation(mat->getBTTransform());
            objectPtr->setKinematic(true);
        }
        return args[0];
    }else
        return handleScope.Close(scriptMatrix4.newInstance(Matrix4(objectPtr->getTransformation())));
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyImpulseAtPoint(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): Too few arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]))
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vecA = scriptVector3.getDataOfInstance(args[0]),
              vecB = scriptVector3.getDataOfInstance(args[1]);
    if(isValidVector(vecA) && isValidVector(vecB)) {
        body->applyImpulse(vecA, vecB);
        body->activate();
    }
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyAngularImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): Too few arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
    if(isValidVector(vec)) {
        body->applyTorqueImpulse(vec);
        body->activate();
    }
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyLinearImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): Too few arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
    if(isValidVector(vec)) {
        body->applyCentralImpulse(vec);
        body->activate();
    }
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::FindBoneByPath(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject findBoneByPath(): Too few arguments"));
    if(!args[0]->IsArray())
        return v8::ThrowException(v8::String::New("RigidObject findBoneByPath(): Invalid argument"));
    
    v8::Handle<v8::Array> path = v8::Handle<v8::Array>::Cast(args[0]);
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    BoneObject* boneObject = objectPtr->getRootBone();
    if(!boneObject) return v8::Undefined();
    
    for(unsigned int i = 0; i < path->Length(); i ++) {
        bool notFound = true;
        std::string boneName = stdStrOfV8(path->Get(i));
        for(auto iterator : boneObject->links)
            if(dynamic_cast<TransformLink*>(iterator) &&
               dynamic_cast<BoneObject*>(iterator->b) &&
               static_cast<BoneObject*>(iterator->b)->bone->name == boneName) {
                boneObject = static_cast<BoneObject*>(iterator->b);
                notFound = false;
                break;
            }
        
        if(notFound)
            return v8::Undefined();
    }
    
    return handleScope.Close(boneObject->scriptInstance);
}

v8::Handle<v8::Value> ScriptRigidObject::AccessTextureAnimationTime(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || !args[0]->IsInt32())
        return v8::ThrowException(v8::String::New("RigidObject getTextureAnimationTime: Invalid argument"));
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    if(args[0]->Uint32Value() >= objectPtr->textureAnimationTime.size())
        return v8::ThrowException(v8::String::New("RigidObject getTextureAnimationTime: Out of bounds"));
    if(args.Length() == 2 && args[1]->IsNumber()) {
        objectPtr->textureAnimationTime[args[0]->Uint32Value()] = args[1]->NumberValue();
        return args[1];
    }else
        return handleScope.Close(v8::Number::New(objectPtr->textureAnimationTime[args[0]->Uint32Value()]));
}

ScriptRigidObject::ScriptRigidObject() :ScriptGraphicObject("RigidObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("model"), GetModel, SetModel);
    objectTemplate->SetAccessor(v8::String::New("mass"), GetMass, SetMass);
    objectTemplate->SetAccessor(v8::String::New("kinematic"), GetKinematic, SetKinematic);
    objectTemplate->Set(v8::String::New("angularVelocity"), v8::FunctionTemplate::New(AccessAngularVelocity));
    objectTemplate->Set(v8::String::New("linearVelocity"), v8::FunctionTemplate::New(AccessLinearVelocity));
    objectTemplate->Set(v8::String::New("angularFactor"), v8::FunctionTemplate::New(AccessAngularFactor));
    objectTemplate->Set(v8::String::New("linearFactor"), v8::FunctionTemplate::New(AccessLinearFactor));
    objectTemplate->SetAccessor(v8::String::New("angularDamping"), GetAngularDamping, SetAngularDamping);
    objectTemplate->SetAccessor(v8::String::New("linearDamping"), GetLinearDamping, SetLinearDamping);
    objectTemplate->Set(v8::String::New("transformation"), v8::FunctionTemplate::New(AccessTransformation));
    objectTemplate->Set(v8::String::New("applyImpulseAtPoint"), v8::FunctionTemplate::New(ApplyImpulseAtPoint));
    objectTemplate->Set(v8::String::New("applyAngularImpulse"), v8::FunctionTemplate::New(ApplyAngularImpulse));
    objectTemplate->Set(v8::String::New("applyLinearImpulse"), v8::FunctionTemplate::New(ApplyLinearImpulse));
    objectTemplate->Set(v8::String::New("findBoneByPath"), v8::FunctionTemplate::New(FindBoneByPath));
    objectTemplate->Set(v8::String::New("textureAnimationTime"), v8::FunctionTemplate::New(AccessTextureAnimationTime));
    
    functionTemplate->Inherit(scriptGraphicObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptTerrainObject::GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->width));
}

v8::Handle<v8::Value> ScriptTerrainObject::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->length));
}

v8::Handle<v8::Value> ScriptTerrainObject::GetBitDepth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->bitDepth << 2));
}

void ScriptTerrainObject::SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->Uint32Value() % 4 != 0 || value->Uint32Value() == 0 || value->Uint32Value() > 8) return;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    objectPtr->bitDepth = value->Uint32Value() >> 2;
}

v8::Handle<v8::Value> ScriptTerrainObject::UpdateModel(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    objectPtr->updateModel();
    objectPtr->updateTouchingObjects();
    return v8::Undefined();
}

ScriptTerrainObject::ScriptTerrainObject() :ScriptGraphicObject("TerrainObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"),
                                static_cast<v8::AccessorGetter>(NULL), static_cast<v8::AccessorSetter>(NULL));
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth);
    objectTemplate->SetAccessor(v8::String::New("length"), GetLength);
    objectTemplate->SetAccessor(v8::String::New("bitDepth"), GetBitDepth, SetBitDepth);
    objectTemplate->Set(v8::String::New("updateModel"), v8::FunctionTemplate::New(UpdateModel));
    
    functionTemplate->Inherit(scriptGraphicObject.functionTemplate);
}



ScriptGraphicObject scriptGraphicObject;
ScriptRigidObject scriptRigidObject;
ScriptTerrainObject scriptTerrainObject;