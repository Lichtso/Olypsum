//
//  ScriptVisualObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptVisualObject.h"

void ScriptMatterObject::GetIntegrity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    MatterObject* objectPtr = getDataOfInstance<MatterObject>(args.This());
    args.GetReturnValue().Set(objectPtr->integrity);
}

void ScriptMatterObject::SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    MatterObject* objectPtr = getDataOfInstance<MatterObject>(args.This());
    if(objectPtr->integrity <= 0.0) return;
    objectPtr->integrity = fmax(0.0, value->NumberValue());
}

void ScriptMatterObject::AttachDecal(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 3 || !args[0]->IsNumber() || !scriptMatrix4.isCorrectInstance(args[1]) || !args[2]->IsString())
        return args.ScriptException("MatterObject attachDecal: Invalid argument");
    
    Decal* decal = new Decal();
    decal->life = args[0]->NumberValue();
    decal->transformation = scriptMatrix4.getDataOfInstance(args[1])->getBTTransform();
    decal->diffuse = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[2]));
    
    if(args.Length() > 3 && args[3]->IsString())
        decal->heightMap = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[3]));
    
    MatterObject* objectPtr = getDataOfInstance<MatterObject>(args.This());
    objectPtr->decals.insert(decal);
}

ScriptMatterObject::ScriptMatterObject() :ScriptPhysicObject("MatterObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("integrity"), GetIntegrity, SetIntegrity);
    objectTemplate->Set(v8::String::New("attachDecal"), v8::FunctionTemplate::New(AttachDecal));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



void ScriptRigidObject::GetModel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Model>(objectPtr->model, name);
    if(filePackage)
        args.GetReturnValue().Set(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptRigidObject::SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    auto model = fileManager.getResourceByPath<Model>(levelManager.levelPackage, stdStrOfV8(value));
    if(model) objectPtr->setModel(NULL, model);
}

void ScriptRigidObject::GetMass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    args.GetReturnValue().Set((body->getInvMass() == 0.0) ? 0.0 : 1.0/body->getInvMass());
}

void ScriptRigidObject::SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() < 0.0 || value->NumberValue() == NAN) return;
    btVector3 inertia;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->getCollisionShape()->calculateLocalInertia(value->NumberValue(), inertia);
    body->setMassProps(value->NumberValue(), inertia);
    body->activate();
}

void ScriptRigidObject::GetKinematic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    args.GetReturnValue().Set(objectPtr->getKinematic());
}

void ScriptRigidObject::SetKinematic(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    objectPtr->setKinematic(value->BooleanValue());
}

void ScriptRigidObject::AccessAngularVelocity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec)) {
            body->setAngularVelocity(vec);
            body->activate();
        }
        args.GetReturnValue().Set(args[0]);
        return;
    }
    args.GetReturnValue().Set(scriptVector3.newInstance(body->getAngularVelocity()));
}

void ScriptRigidObject::AccessLinearVelocity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec)) {
            body->setLinearVelocity(vec);
            body->activate();
        }
        args.GetReturnValue().Set(args[0]);
        return;
    }
    args.GetReturnValue().Set(scriptVector3.newInstance(body->getLinearVelocity()));
}

void ScriptRigidObject::AccessAngularFactor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec))
            body->setAngularFactor(vec);
        args.GetReturnValue().Set(args[0]);
        return;
    }
    args.GetReturnValue().Set(scriptVector3.newInstance(body->getAngularFactor()));
}

void ScriptRigidObject::AccessLinearFactor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
        if(isValidVector(vec))
            body->setLinearFactor(vec);
        args.GetReturnValue().Set(args[0]);
        return;
    }
    args.GetReturnValue().Set(scriptVector3.newInstance(body->getLinearFactor()));
}

void ScriptRigidObject::GetAngularDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    args.GetReturnValue().Set(body->getAngularDamping());
}

void ScriptRigidObject::SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setAngularDamping(value->NumberValue());
}

void ScriptRigidObject::GetLinearDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    args.GetReturnValue().Set(body->getLinearDamping());
}

void ScriptRigidObject::SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() == NAN) return;
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setLinearDamping(value->NumberValue());
}

void ScriptRigidObject::AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        Matrix4* mat = scriptMatrix4.getDataOfInstance(args[0]);
        if(mat->isValid()) {
            objectPtr->setTransformation(mat->getBTTransform());
            objectPtr->setKinematic(true);
        }
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(Matrix4(objectPtr->getTransformation())));
}

void ScriptRigidObject::ApplyImpulseAtPoint(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return args.ScriptException("RigidObject applyImpulseAtPoint(): Too few arguments");
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]))
        return args.ScriptException("RigidObject applyImpulseAtPoint(): Invalid argument");
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vecA = scriptVector3.getDataOfInstance(args[0]),
              vecB = scriptVector3.getDataOfInstance(args[1]);
    if(isValidVector(vecA) && isValidVector(vecB)) {
        body->applyImpulse(vecA, vecB);
        body->activate();
    }
}

void ScriptRigidObject::ApplyAngularImpulse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("RigidObject applyAngularImpulse(): Too few arguments");
    if(!scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("RigidObject applyAngularImpulse(): Invalid argument");
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
    if(isValidVector(vec)) {
        body->applyTorqueImpulse(vec);
        body->activate();
    }
}

void ScriptRigidObject::ApplyLinearImpulse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("RigidObject applyLinearImpulse(): Too few arguments");
    if(!scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("RigidObject applyLinearImpulse(): Invalid argument");
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    btVector3 vec = scriptVector3.getDataOfInstance(args[0]);
    if(isValidVector(vec)) {
        body->applyCentralImpulse(vec);
        body->activate();
    }
}

void ScriptRigidObject::GetBoneByName(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("RigidObject getBoneByName(): Too few arguments");
    if(!args[0]->IsString())
        return args.ScriptException("RigidObject getBoneByName(): Invalid argument");
    
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    BoneLink* linkPrt = objectPtr->findBoneLinkOfName(cStrOfV8(args[0]));
    if(linkPrt)
        args.GetReturnValue().Set(linkPrt->b->scriptInstance);
}

void ScriptRigidObject::AccessTextureAnimationTime(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || !args[0]->IsInt32())
        return args.ScriptException("RigidObject getTextureAnimationTime: Invalid argument");
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    if(args[0]->Uint32Value() >= objectPtr->textureAnimationTime.size())
        return args.ScriptException("RigidObject getTextureAnimationTime: Out of bounds");
    if(args.Length() == 2 && args[1]->IsNumber()) {
        objectPtr->textureAnimationTime[args[0]->Uint32Value()] = args[1]->NumberValue();
        args.GetReturnValue().Set(args[1]);
        return;
    }else
        args.GetReturnValue().Set(objectPtr->textureAnimationTime[args[0]->Uint32Value()]);
}

ScriptRigidObject::ScriptRigidObject() :ScriptMatterObject("RigidObject") {
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
    objectTemplate->Set(v8::String::New("getBoneByName"), v8::FunctionTemplate::New(GetBoneByName));
    objectTemplate->Set(v8::String::New("textureAnimationTime"), v8::FunctionTemplate::New(AccessTextureAnimationTime));
    
    functionTemplate->Inherit(scriptMatterObject.functionTemplate);
}



void ScriptTerrainObject::GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    args.GetReturnValue().Set(objectPtr->width);
}

void ScriptTerrainObject::GetLength(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    args.GetReturnValue().Set(objectPtr->length);
}

void ScriptTerrainObject::GetBitDepth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    args.GetReturnValue().Set(objectPtr->bitDepth << 2);
}

void ScriptTerrainObject::SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->Uint32Value() % 4 != 0 || value->Uint32Value() == 0 || value->Uint32Value() > 8) return;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    objectPtr->bitDepth = value->Uint32Value() >> 2;
}

void ScriptTerrainObject::UpdateModel(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    objectPtr->updateModel();
    objectPtr->updateTouchingObjects();
}

ScriptTerrainObject::ScriptTerrainObject() :ScriptMatterObject("TerrainObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"),
                                static_cast<v8::AccessorGetterCallback>(NULL),
                                static_cast<v8::AccessorSetterCallback>(NULL));
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth);
    objectTemplate->SetAccessor(v8::String::New("length"), GetLength);
    objectTemplate->SetAccessor(v8::String::New("bitDepth"), GetBitDepth, SetBitDepth);
    objectTemplate->Set(v8::String::New("updateModel"), v8::FunctionTemplate::New(UpdateModel));
    
    functionTemplate->Inherit(scriptMatterObject.functionTemplate);
}