//
//  ScriptDisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//
//

#include "ScriptDisplayObject.h"

v8::Handle<v8::Value> ScriptModelObject::GetIntegrity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->integrity));
}

void ScriptModelObject::SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    if(objectPtr->integrity <= 0.0) return;
    objectPtr->integrity = fmax(0.0, value->NumberValue());
}

v8::Handle<v8::Value> ScriptModelObject::GetModel(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Model>(objectPtr->model, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getResourcePath(filePackage, name).c_str()));
}

void ScriptModelObject::SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    auto model = fileManager.initResource<Model>(stdStrOfV8(value));
    if(model) objectPtr->setModel(model);
}

v8::Handle<v8::Value> ScriptModelObject::AccessTextureAnimation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || !args[0]->IsInt32())
        return v8::ThrowException(v8::String::New("ModelObject getTextureAnimation: Invalid argument"));
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(args.This());
    if(args[0]->Uint32Value() > objectPtr->textureAnimation.size())
        return v8::ThrowException(v8::String::New("ModelObject getTextureAnimation: Out of bounds"));
    if(args.Length() == 2 && args[1]->IsNumber()) {
        objectPtr->textureAnimation[args[0]->Uint32Value()] = args[1]->NumberValue();
        return args[1];
    }else
        return handleScope.Close(v8::Number::New(objectPtr->textureAnimation[args[0]->Uint32Value()]));
}

ScriptModelObject::ScriptModelObject(const char* name) :ScriptPhysicObject(name) {
    
}

ScriptModelObject::ScriptModelObject() :ScriptModelObject("ModelObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("integrity"), GetIntegrity, SetIntegrity);
    objectTemplate->SetAccessor(v8::String::New("model"), GetModel, SetModel);
    objectTemplate->Set(v8::String::New("textureAnimation"), v8::FunctionTemplate::New(AccessTextureAnimation));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
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
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): To less arguments"));
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
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): To less arguments"));
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
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): To less arguments"));
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

ScriptRigidObject::ScriptRigidObject() :ScriptModelObject("RigidObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
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
    
    functionTemplate->Inherit(scriptModelObject.functionTemplate);
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

ScriptTerrainObject::ScriptTerrainObject() :ScriptPhysicObject("TerrainObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"), NULL, NULL);
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth);
    objectTemplate->SetAccessor(v8::String::New("length"), GetLength);
    objectTemplate->SetAccessor(v8::String::New("bitDepth"), GetBitDepth, SetBitDepth);
    objectTemplate->Set(v8::String::New("updateModel"), v8::FunctionTemplate::New(UpdateModel));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



ScriptModelObject scriptModelObject;
ScriptRigidObject scriptRigidObject;
ScriptTerrainObject scriptTerrainObject;