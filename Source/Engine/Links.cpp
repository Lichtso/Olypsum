//
//  Links.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//
//

#include "ScriptLinks.h"

BaseLink::BaseLink(LinkInitializer& initializer) {
    init(initializer);
}

BaseLink::BaseLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer = readInitializer(node, levelLoader);
    init(initializer);
}

void BaseLink::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptBaseLink.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
}

void BaseLink::removeClean(BaseObject* object) {
    a->links.erase(this);
    b->links.erase(this);
    delete this;
}

void BaseLink::removeFast(BaseObject* object) {
    /*if(!a || !b)
        delete this;
    else if(a == object)
        a = NULL;
    else
        b = NULL;*/
    
    if(!b)
        delete this;
    else
        b = NULL;
}

LinkInitializer BaseLink::readInitializer(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer;
    node = node->first_node("Objects");
    if(!node) {
        log(error_log, "Tried to construct BaseLink without \"Objects\"-node.");
        return initializer;
    }
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("indexA");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without \"indexA\"-attribute.");
        return initializer;
    }
    sscanf(attribute->value(), "%d", &initializer.index[0]);
    
    attribute = node->first_attribute("indexB");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without \"indexB\"-attribute.");
        return initializer;
    }
    sscanf(attribute->value(), "%d", &initializer.index[1]);
    
    initializer.object[0] = levelLoader->getObjectLinking(initializer.index[0]);
    initializer.object[1] = levelLoader->getObjectLinking(initializer.index[1]);
    return initializer;
}

bool BaseLink::init(LinkInitializer& initializer) {
    auto iteratorA = initializer.object[0]->findLink(initializer.object[1]),
         iteratorB = initializer.object[1]->findLink(initializer.object[0]);
    if(iteratorA != initializer.object[0]->links.end() || iteratorB != initializer.object[1]->links.end()) {
        if(*iteratorA == *iteratorB && dynamic_cast<TransformLink*>(*iteratorA)) {
            delete *iteratorA;
            initializer.object[0]->links.erase(iteratorA);
            initializer.object[1]->links.erase(iteratorB);
        }else{
            log(error_log, "Tried to overwrite link.");
            delete this;
            return false;
        }
    }
    initializer.object[0]->links.insert(this);
    initializer.object[1]->links.insert(this);
    a = initializer.object[0];
    b = initializer.object[1];
    return true;
}

rapidxml::xml_node<xmlUsedCharType>* BaseLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("BaseLink");
    rapidxml::xml_node<xmlUsedCharType>* objectNode = doc.allocate_node(rapidxml::node_element);
    objectNode->name("Objects");
    node->append_node(objectNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("indexA");
    attribute->value(doc.allocate_string(stringOf(linkSaver->index[0]).c_str()));
    objectNode->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("indexB");
    attribute->value(doc.allocate_string(stringOf(linkSaver->index[1]).c_str()));
    objectNode->append_attribute(attribute);
    return node;
}



PhysicLink::PhysicLink(LinkInitializer& initializer, btTypedConstraint* constraintB)
:BaseLink(initializer), constraint(constraintB) {
    constraint->setUserConstraintPtr(this);
    objectManager.physicsWorld->addConstraint(constraint);
}

PhysicLink::PhysicLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    rapidxml::xml_node<xmlUsedCharType> *parameterNode;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("type");
    if(!attribute) {
        log(error_log, "Tried to construct PhysicLink without \"type\"-attribute.");
        return;
    }
    
    LinkInitializer initializer = BaseLink::readInitializer(node, levelLoader);
    RigidObject *a = dynamic_cast<RigidObject*>(initializer.object[0]), *b = dynamic_cast<RigidObject*>(initializer.object[1]);
    if(!a || !b) {
        log(error_log, "Tried to construct PhysicLink with objects which aren't RigidObjects.");
        return;
    }
    BaseLink::init(initializer);
    
    if(strcmp(attribute->value(), "point") == 0) {
        parameterNode = node->first_node("Point");
        if(!parameterNode) {
            log(error_log, "Tried to construct Point-PhysicLink without first \"Point\"-node.");
            return;
        }
        XMLValueArray<float> vecData;
        vecData.readString(parameterNode->value(), "%f");
        btVector3 pointA = vecData.getVector3();
        
        parameterNode = parameterNode->next_sibling("Point");
        if(!parameterNode) {
            log(error_log, "Tried to construct Point-PhysicLink without second \"Point\"-node.");
            return;
        }
        vecData.readString(parameterNode->value(), "%f");
        btVector3 pointB = vecData.getVector3();
        
        constraint = new btPoint2PointConstraint(*a->getBody(), *b->getBody(), pointA, pointB);
    }else if(strcmp(attribute->value(), "gear") == 0) {
        rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Axis");
        if(!parameterNode) {
            log(error_log, "Tried to construct Gear-PhysicLink without first \"Axis\"-node.");
            return;
        }
        XMLValueArray<float> vecData;
        vecData.readString(parameterNode->value(), "%f");
        btVector3 axisA = vecData.getVector3();
        
        parameterNode = parameterNode->next_sibling("Axis");
        if(!parameterNode) {
            log(error_log, "Tried to construct Gear-PhysicLink without second \"Axis\"-node.");
            return;
        }
        vecData.readString(parameterNode->value(), "%f");
        btVector3 axisB = vecData.getVector3();
        
        attribute = node->first_attribute("ratio");
        if(!attribute) {
            log(error_log, "Tried to construct Gear-PhysicLink without \"ratio\"-attribute.");
            return;
        }
        float ratio;
        sscanf(attribute->value(), "%f", &ratio);
        
        constraint = new btGearConstraint(*a->getBody(), *b->getBody(), axisA, axisB, ratio);
    }else if(strcmp(attribute->value(), "hinge") == 0 || strcmp(attribute->value(), "slider") == 0) {
        parameterNode = node->first_node("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct Hinge/Slider-PhysicLink without first \"Frame\"-node.");
            return;
        }
        btTransform frameA = readTransformationXML(parameterNode);
        parameterNode = parameterNode->next_sibling("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct Hinge/Slider-PhysicLink without second \"Frame\"-node.");
            return;
        }
        btTransform frameB = readTransformationXML(parameterNode);
        
        btHingeConstraint* hinge = NULL;
        btSliderConstraint* slider = NULL;
        if(strcmp(attribute->value(), "hinge") == 0) {
            /*btTransform transform = btTransform::getIdentity();
            transform.setRotation(btQuaternion(M_PI_2, 0.0, 0.0));
            frameA *= transform;
            frameB *= transform;*/
            this->constraint = hinge = new btHingeConstraint(*a->getBody(), *b->getBody(), frameA, frameB, true);
        }else
            this->constraint = slider = new btSliderConstraint(*a->getBody(), *b->getBody(), frameA, frameB, true);
        
        parameterNode = node->first_node("AngularLimit");
        if(parameterNode) {
            float min, max;
            attribute = parameterNode->first_attribute("min");
            if(!attribute) {
                log(error_log, "Tried to construct Hinge/Slider-PhysicLink-AngularLimit without \"min\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &min);
            attribute = parameterNode->first_attribute("max");
            if(!attribute) {
                log(error_log, "Tried to construct Hinge/Slider-PhysicLink-AngularLimit without \"max\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &max);
            if(hinge)
                hinge->setLimit(min, max);
            else{
                slider->setLowerAngLimit(min);
                slider->setUpperAngLimit(max);
            }
        }
        
        parameterNode = node->first_node("LinearLimit");
        if(parameterNode && slider) {
            float min, max;
            attribute = parameterNode->first_attribute("min");
            if(!attribute) {
                log(error_log, "Tried to construct Slider-PhysicLink-LinearLimit without \"min\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &min);
            attribute = parameterNode->first_attribute("max");
            if(!attribute) {
                log(error_log, "Tried to construct Slider-PhysicLink-LinearLimit without \"max\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &max);
            slider->setLowerLinLimit(min);
            slider->setUpperLinLimit(max);
        }
        
        parameterNode = node->first_node("AngularMotor");
        if(parameterNode) {
            attribute = parameterNode->first_attribute("enabled");
            if(!attribute) {
                log(error_log, "Tried to construct Hinge/Slider-PhysicLink-AngularMotor without \"enabled\"-attribute.");
                return;
            }
            bool enabled = (strcmp(attribute->value(), "true") == 0);
            float velocity, force;
            attribute = parameterNode->first_attribute("velocity");
            if(!attribute) {
                log(error_log, "Tried to construct Hinge/Slider-PhysicLink-AngularMotor without \"velocity\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &velocity);
            attribute = parameterNode->first_attribute("force");
            if(!attribute) {
                log(error_log, "Tried to construct Hinge/Slider-PhysicLink-AngularMotor without \"force\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &force);
            if(hinge)
                hinge->enableAngularMotor(enabled, velocity, force);
            else{
                slider->setPoweredAngMotor(enabled);
                slider->setTargetAngMotorVelocity(velocity);
                slider->setMaxAngMotorForce(force);
            }
        }
        
        parameterNode = node->first_node("LinearMotor");
        if(parameterNode) {
            attribute = parameterNode->first_attribute("enabled");
            if(!attribute) {
                log(error_log, "Tried to construct Slider-PhysicLink-LinearMotor without \"enabled\"-attribute.");
                return;
            }
            bool enabled = (strcmp(attribute->value(), "true") == 0);
            float velocity, force;
            attribute = parameterNode->first_attribute("velocity");
            if(!attribute) {
                log(error_log, "Tried to construct Slider-PhysicLink-LinearMotor without \"velocity\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &velocity);
            attribute = parameterNode->first_attribute("force");
            if(!attribute) {
                log(error_log, "Tried to construct Slider-PhysicLink-LinearMotor without \"force\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &force);
            slider->setPoweredLinMotor(enabled);
            slider->setTargetLinMotorVelocity(velocity);
            slider->setMaxLinMotorForce(force);
        }
    }else if(strcmp(attribute->value(), "dof6") == 0) {
        parameterNode = node->first_node("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct DOF6-PhysicLink without first \"Frame\"-node.");
            return;
        }
        btTransform frameA = readTransformationXML(parameterNode);
        parameterNode = parameterNode->next_sibling("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct DOF6-PhysicLink without second \"Frame\"-node.");
            return;
        }
        btTransform frameB = readTransformationXML(parameterNode);
        
        XMLValueArray<float> vecData;
        btGeneric6DofConstraint* dof6;
        btGeneric6DofSpringConstraint* springDof6 = NULL;
        parameterNode = parameterNode->first_node("Spring");
        if(parameterNode) {
            this->constraint = dof6 = springDof6 = new btGeneric6DofSpringConstraint(*a->getBody(), *b->getBody(), frameA, frameB, true);
            float value;
            unsigned int index;
            while(parameterNode) {
                attribute = parameterNode->first_attribute("index");
                if(!attribute) {
                    log(error_log, "Tried to construct DOF6-PhysicLink-Spring without \"index\"-attribute.");
                    return;
                }
                sscanf(attribute->value(), "%d", &index);
                springDof6->enableSpring(index, true);
                attribute = parameterNode->first_attribute("stiffness");
                if(attribute) {
                    sscanf(attribute->value(), "%f", &value);
                    springDof6->setStiffness(index, value);
                }
                attribute = parameterNode->first_attribute("damping");
                if(attribute) {
                    sscanf(attribute->value(), "%f", &value);
                    springDof6->setDamping(index, value);
                }
                attribute = parameterNode->first_attribute("equilibrium");
                if(attribute) {
                    sscanf(attribute->value(), "%f", &value);
                    springDof6->setEquilibriumPoint(index, value);
                }
                parameterNode = parameterNode->next_sibling("Spring");
            }
        }else
            this->constraint = dof6 = new btGeneric6DofConstraint(*a->getBody(), *b->getBody(), frameA, frameB, true);
        
        parameterNode = node->first_node("AngularLimit");
        if(parameterNode) {
            attribute = parameterNode->first_attribute("min");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-AngularLimit without \"min\"-attribute.");
                return;
            }
            vecData.readString(attribute->value(), "%f");
            dof6->setAngularLowerLimit(vecData.getVector3());
            attribute = parameterNode->first_attribute("max");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-AngularLimit without \"max\"-attribute.");
                return;
            }
            vecData.readString(attribute->value(), "%f");
            dof6->setAngularUpperLimit(vecData.getVector3());
        }
        
        parameterNode = node->first_node("LinearLimit");
        if(parameterNode) {
            attribute = parameterNode->first_attribute("min");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-LinearLimit without \"min\"-attribute.");
                return;
            }
            vecData.readString(attribute->value(), "%f");
            dof6->setLinearLowerLimit(vecData.getVector3());
            attribute = parameterNode->first_attribute("max");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-LinearLimit without \"max\"-attribute.");
                return;
            }
            vecData.readString(attribute->value(), "%f");
            dof6->setLinearUpperLimit(vecData.getVector3());
        }
        
        btTranslationalLimitMotor* linearMotor = dof6->getTranslationalLimitMotor();
        parameterNode = node->first_node("Motor");
        while(parameterNode) {
            bool *enabled;
            unsigned int index;
            float *velocity, *force;
            attribute = parameterNode->first_attribute("index");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-Motor without \"index\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%d", &index);
            
            if(index < 3) {
                enabled = &linearMotor->m_enableMotor[index];
                velocity = &linearMotor->m_targetVelocity[index];
                force = &linearMotor->m_maxMotorForce[index];
            }else{
                btRotationalLimitMotor* angularMotor = dof6->getRotationalLimitMotor(index-3);
                enabled = &angularMotor->m_enableMotor;
                velocity = &angularMotor->m_targetVelocity;
                force = &angularMotor->m_maxMotorForce;
            }
            
            attribute = parameterNode->first_attribute("enabled");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-Motor without \"enabled\"-attribute.");
                return;
            }
            *enabled = (strcmp(attribute->value(), "true") == 0);
            attribute = parameterNode->first_attribute("velocity");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-Motor without \"velocity\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", velocity);
            attribute = parameterNode->first_attribute("force");
            if(!attribute) {
                log(error_log, "Tried to construct DOF6-PhysicLink-Motor without \"force\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", force);
            
            parameterNode = parameterNode->next_sibling("Motor");
        }
    }else if(strcmp(attribute->value(), "coneTwist") == 0) {
        parameterNode = node->first_node("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct ConeTwist-PhysicLink without first \"Frame\"-node.");
            return;
        }
        btTransform frameA = readTransformationXML(parameterNode);
        parameterNode = parameterNode->next_sibling("Frame");
        if(!parameterNode) {
            log(error_log, "Tried to construct ConeTwist-PhysicLink without second \"Frame\"-node.");
            return;
        }
        btTransform frameB = readTransformationXML(parameterNode);
        
        btConeTwistConstraint* coneTwist;
        this->constraint = coneTwist = new btConeTwistConstraint(*a->getBody(), *b->getBody(), frameA, frameB);
        
        parameterNode = node->first_node("AngularLimit");
        if(parameterNode) {
            float value;
            attribute = parameterNode->first_attribute("swingSpanA");
            if(!attribute) {
                log(error_log, "Tried to construct ConeTwist-PhysicLink-AngularLimit without \"swingSpanA\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &value);
            coneTwist->setLimit(5, value);
            attribute = parameterNode->first_attribute("swingSpanB");
            if(!attribute) {
                log(error_log, "Tried to construct ConeTwist-PhysicLink-AngularLimit without \"swingSpanB\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &value);
            coneTwist->setLimit(4, value);
            attribute = parameterNode->first_attribute("twistSpan");
            if(!attribute) {
                log(error_log, "Tried to construct ConeTwist-PhysicLink-AngularLimit without \"twistSpan\"-attribute.");
                return;
            }
            sscanf(attribute->value(), "%f", &value);
            coneTwist->setLimit(3, value);
        }
    }else{
        log(error_log, std::string("Tried to construct PhysicLink with invalid \"type\"-attribute: ")+attribute->value()+'.');
        return;
    }
    
    attribute = node->first_attribute("burstImpulse");
    if(attribute) {
        float value;
        sscanf(attribute->value(), "%f", &value);
        constraint->setBreakingImpulseThreshold(value);
    }
    constraint->enableFeedback(true);
    constraint->setUserConstraintPtr(this);
    objectManager.physicsWorld->addConstraint(constraint);
}

PhysicLink::~PhysicLink() {
    delete constraint;
}

void PhysicLink::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptPhysicLink.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
}

void PhysicLink::gameTick() {
    if(constraint->isEnabled()) return;
    
    if(scriptFile)
        scriptFile->callFunction("onburst", true, { scriptInstance });
    
    removeClean(NULL);
}

void PhysicLink::removeClean(BaseObject* object) {
    objectManager.physicsWorld->removeConstraint(constraint);
    
    btRigidBody* body = static_cast<RigidObject*>(a)->getBody();
    if(body) body->activate();
    body = static_cast<RigidObject*>(b)->getBody();
    if(body) body->activate();
    
    BaseLink::removeClean(object);
}

rapidxml::xml_node<xmlUsedCharType>* PhysicLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    if(&constraint->getRigidBodyA() == static_cast<RigidObject*>(linkSaver->object[1])->getBody())
        linkSaver->swap();
    
    rapidxml::xml_node<xmlUsedCharType> *parameterNode, *node = BaseLink::write(doc, linkSaver);
    node->name("PhysicLink");
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("type");
    node->append_attribute(attribute);
    
    switch(constraint->getConstraintType()) {
        case POINT2POINT_CONSTRAINT_TYPE: {
            attribute->value("point");
            btPoint2PointConstraint* pointConstraint = static_cast<btPoint2PointConstraint*>(constraint);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Point");
            btVector3 point = pointConstraint->getPivotInA();
            parameterNode->value(doc.allocate_string(stringOf(point).c_str()));
            node->append_node(parameterNode);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Point");
            point = pointConstraint->getPivotInB();
            parameterNode->value(doc.allocate_string(stringOf(point).c_str()));
            node->append_node(parameterNode);
        } break;
        case GEAR_CONSTRAINT_TYPE: {
            attribute->value("gear");
            btGearConstraint* gearConstraint = static_cast<btGearConstraint*>(constraint);
            rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
            attribute->name("ratio");
            attribute->value(doc.allocate_string(stringOf(gearConstraint->getRatio()).c_str()));
            node->append_attribute(attribute);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Axis");
            btVector3 axis = gearConstraint->getAxisA();
            parameterNode->value(doc.allocate_string(stringOf(axis).c_str()));
            node->append_node(parameterNode);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Axis");
            axis = gearConstraint->getAxisB();
            parameterNode->value(doc.allocate_string(stringOf(axis).c_str()));
            node->append_node(parameterNode);
        } break;
        case HINGE_CONSTRAINT_TYPE:
        case SLIDER_CONSTRAINT_TYPE: {
            btHingeConstraint* hinge = NULL;
            btSliderConstraint* slider = NULL;
            bool angLimit, angMotor;
            btTransform frameA, frameB;
            float minAngLim, maxAngLim, angMotorVelocity, angMotorForce;
            if(constraint->getConstraintType() == HINGE_CONSTRAINT_TYPE) {
                attribute->value("hinge");
                hinge = static_cast<btHingeConstraint*>(constraint);
                minAngLim = hinge->getLowerLimit();
                maxAngLim = hinge->getUpperLimit();
                angLimit = minAngLim != 1.0 || maxAngLim != -1.0;
                angMotor = hinge->getEnableAngularMotor();
                angMotorVelocity = hinge->getMotorTargetVelosity();
                angMotorForce = hinge->getMaxMotorImpulse();
                /*btTransform transform = btTransform::getIdentity();
                transform.setRotation(btQuaternion(-M_PI_2, 0.0, 0.0));
                frameA = hinge->getFrameOffsetA()*transform;
                frameB = hinge->getFrameOffsetB()*transform;*/
                frameA = slider->getFrameOffsetA();
                frameB = slider->getFrameOffsetB();
            }else{
                attribute->value("slider");
                slider = static_cast<btSliderConstraint*>(constraint);
                minAngLim = slider->getLowerAngLimit();
                maxAngLim = slider->getUpperAngLimit();
                angLimit = minAngLim != 0.0 || maxAngLim != 0.0;
                angMotor = slider->getPoweredAngMotor();
                angMotorVelocity = slider->getTargetAngMotorVelocity();
                angMotorForce = slider->getMaxAngMotorForce();
                frameA = slider->getFrameOffsetA();
                frameB = slider->getFrameOffsetB();
            }
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            parameterNode->append_node(writeTransformationXML(doc, frameA));
            node->append_node(parameterNode);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            parameterNode->append_node(writeTransformationXML(doc, frameB));
            node->append_node(parameterNode);
            if(angLimit) {
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("AngularLimit");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("min");
                attribute->value(doc.allocate_string(stringOf(minAngLim).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("max");
                attribute->value(doc.allocate_string(stringOf(maxAngLim).c_str()));
                parameterNode->append_attribute(attribute);
            }
            if(angMotor || angMotorVelocity != 0.0 || angMotorForce != 0.0) {
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("AngularMotor");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("enabled");
                attribute->value((angMotor) ? "true" : "false");
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("velocity");
                attribute->value(doc.allocate_string(stringOf(angMotorVelocity).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("force");
                attribute->value(doc.allocate_string(stringOf(angMotorForce).c_str()));
                parameterNode->append_attribute(attribute);
            }
            if(slider) {
                if(slider->getLowerLinLimit() != 1.0 || slider->getUpperLinLimit() != -1.0) {
                    parameterNode = doc.allocate_node(rapidxml::node_element);
                    parameterNode->name("LinearLimit");
                    node->append_node(parameterNode);
                    attribute = doc.allocate_attribute();
                    attribute->name("min");
                    attribute->value(doc.allocate_string(stringOf(slider->getLowerLinLimit()).c_str()));
                    parameterNode->append_attribute(attribute);
                    attribute = doc.allocate_attribute();
                    attribute->name("max");
                    attribute->value(doc.allocate_string(stringOf(slider->getUpperLinLimit()).c_str()));
                    parameterNode->append_attribute(attribute);
                }
                if(slider->getPoweredLinMotor() ||
                   slider->getTargetLinMotorVelocity() != 0.0 ||
                   slider->getMaxLinMotorForce() != 0.0) {
                    parameterNode = doc.allocate_node(rapidxml::node_element);
                    parameterNode->name("LinearMotor");
                    node->append_node(parameterNode);
                    attribute = doc.allocate_attribute();
                    attribute->name("enabled");
                    attribute->value((slider->getPoweredLinMotor()) ? "true" : "false");
                    parameterNode->append_attribute(attribute);
                    attribute = doc.allocate_attribute();
                    attribute->name("velocity");
                    attribute->value(doc.allocate_string(stringOf(slider->getTargetLinMotorVelocity()).c_str()));
                    parameterNode->append_attribute(attribute);
                    attribute = doc.allocate_attribute();
                    attribute->name("force");
                    attribute->value(doc.allocate_string(stringOf(slider->getMaxLinMotorForce()).c_str()));
                    parameterNode->append_attribute(attribute);
                }
            }
        } break;
        case D6_CONSTRAINT_TYPE:
        case D6_SPRING_CONSTRAINT_TYPE: {
            attribute->value("dof6");
            btGeneric6DofConstraint* dof6Constraint = static_cast<btGeneric6DofConstraint*>(constraint);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            parameterNode->append_node(writeTransformationXML(doc, dof6Constraint->getFrameOffsetA()));
            node->append_node(parameterNode);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            parameterNode->append_node(writeTransformationXML(doc, dof6Constraint->getFrameOffsetB()));
            node->append_node(parameterNode);
            btVector3 min, max, lowCompare(1.0, 1.0, 1.0), highCompare(-1.0, -1.0, -1.0);
            dof6Constraint->getAngularLowerLimit(min); lowCompare.setW(min.w());
            dof6Constraint->getAngularUpperLimit(max); highCompare.setW(max.w());
            if(min != lowCompare || max != highCompare) {
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("AngularLimit");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("min");
                attribute->value(doc.allocate_string(stringOf(min).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("max");
                attribute->value(doc.allocate_string(stringOf(max).c_str()));
                parameterNode->append_attribute(attribute);
            }
            dof6Constraint->getLinearLowerLimit(min); lowCompare.setW(min.w());
            dof6Constraint->getLinearUpperLimit(max); highCompare.setW(max.w());
            if(min != lowCompare || max != highCompare) {
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("LinearLimit");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("min");
                attribute->value(doc.allocate_string(stringOf(min).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("max");
                attribute->value(doc.allocate_string(stringOf(max).c_str()));
                parameterNode->append_attribute(attribute);
            }
            for(char index = 0; index < 3; index ++) {
                btRotationalLimitMotor* angularMotor = dof6Constraint->getRotationalLimitMotor(index);
                if(!angularMotor->m_enableMotor &&
                   angularMotor->m_targetVelocity == 0.0 &&
                   angularMotor->m_maxMotorForce == 0.0)
                    continue;
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("Motor");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("index");
                attribute->value(doc.allocate_string(stringOf(index+3).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("enabled");
                attribute->value((angularMotor->m_enableMotor) ? "true" : "false");
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("velocity");
                attribute->value(doc.allocate_string(stringOf(angularMotor->m_targetVelocity).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("force");
                attribute->value(doc.allocate_string(stringOf(angularMotor->m_maxMotorForce).c_str()));
                parameterNode->append_attribute(attribute);
            }
            btTranslationalLimitMotor* linearMotor = dof6Constraint->getTranslationalLimitMotor();
            for(char index = 0; index < 3; index ++) {
                if(!linearMotor->m_enableMotor[index] &&
                   linearMotor->m_targetVelocity[index] == 0.0 &&
                   linearMotor->m_maxMotorForce[index] == 0.0)
                    continue;
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("Motor");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("index");
                attribute->value(doc.allocate_string(stringOf(index).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("enabled");
                attribute->value((linearMotor->m_enableMotor[index]) ? "true" : "false");
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("velocity");
                attribute->value(doc.allocate_string(stringOf(linearMotor->m_targetVelocity[index]).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("force");
                attribute->value(doc.allocate_string(stringOf(linearMotor->m_maxMotorForce[index]).c_str()));
                parameterNode->append_attribute(attribute);
            }
            if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE) {
                btGeneric6DofSpringConstraint* dof6SpringConstraint = static_cast<btGeneric6DofSpringConstraint*>(constraint);
                float value;
                for(unsigned char index = 0; index < 6; index ++) {
                    if(!dof6SpringConstraint->getEnableSpring(index)) continue;
                    parameterNode = doc.allocate_node(rapidxml::node_element);
                    parameterNode->name("Spring");
                    node->append_node(parameterNode);
                    attribute = doc.allocate_attribute();
                    attribute->name("index");
                    attribute->value(doc.allocate_string(stringOf(index).c_str()));
                    parameterNode->append_attribute(attribute);
                    value = dof6SpringConstraint->getDamping(index);
                    if(value != 0.0) {
                        attribute = doc.allocate_attribute();
                        attribute->name("damping");
                        attribute->value(doc.allocate_string(stringOf(value).c_str()));
                        parameterNode->append_attribute(attribute);
                    }
                    value = dof6SpringConstraint->getStiffness(index);
                    if(value != 0.0) {
                        attribute = doc.allocate_attribute();
                        attribute->name("stiffness");
                        attribute->value(doc.allocate_string(stringOf(value).c_str()));
                        parameterNode->append_attribute(attribute);
                    }
                    value = dof6SpringConstraint->getEquilibriumPoint(index);
                    if(value != 0.0) {
                        attribute = doc.allocate_attribute();
                        attribute->name("equilibrium");
                        attribute->value(doc.allocate_string(stringOf(value).c_str()));
                        parameterNode->append_attribute(attribute);
                    }
                }
            }
        } break;
        case CONETWIST_CONSTRAINT_TYPE: {
            attribute->value("coneTwist");
            btConeTwistConstraint* coneTwistConstraint = static_cast<btConeTwistConstraint*>(constraint);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            btTransform transform = coneTwistConstraint->getFrameOffsetA();
            parameterNode->append_node(writeTransformationXML(doc, transform));
            node->append_node(parameterNode);
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Frame");
            transform = coneTwistConstraint->getFrameOffsetB();
            parameterNode->append_node(writeTransformationXML(doc, transform));
            node->append_node(parameterNode);
            float swingSpanA = coneTwistConstraint->getSwingSpan1(),
                  swingSpanB = coneTwistConstraint->getSwingSpan2(),
                  twistSpan = coneTwistConstraint->getTwistSpan();
            if(swingSpanA != BT_LARGE_FLOAT || swingSpanB != BT_LARGE_FLOAT || twistSpan != BT_LARGE_FLOAT) {
                parameterNode = doc.allocate_node(rapidxml::node_element);
                parameterNode->name("AngularLimit");
                node->append_node(parameterNode);
                attribute = doc.allocate_attribute();
                attribute->name("swingSpanA");
                attribute->value(doc.allocate_string(stringOf(swingSpanA).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("swingSpanB");
                attribute->value(doc.allocate_string(stringOf(swingSpanB).c_str()));
                parameterNode->append_attribute(attribute);
                attribute = doc.allocate_attribute();
                attribute->name("twistSpan");
                attribute->value(doc.allocate_string(stringOf(twistSpan).c_str()));
                parameterNode->append_attribute(attribute);
            }
        } break;
        default:
            log(error_log, "Tried to save PhysicLink with invalid constraint type.");
            break;
    }
    
    if(constraint->getBreakingImpulseThreshold() != SIMD_INFINITY) {
        attribute = doc.allocate_attribute();
        attribute->name("burstImpulse");
        attribute->value(doc.allocate_string(stringOf(constraint->getBreakingImpulseThreshold()).c_str()));
        node->append_attribute(attribute);
    }
    
    return node;
}



TransformLink::TransformLink(LinkInitializer& initializer) :transform(btTransform::getIdentity()) {
    init(initializer);
}

TransformLink::TransformLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer = BaseLink::readInitializer(node, levelLoader);
    if(init(initializer))
        transform = readTransformationXML(node);
}

void TransformLink::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptTransformLink.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
}

void TransformLink::gameTick() {
    BoneObject* boneObject = dynamic_cast<BoneObject*>(b);
    if(boneObject) {
        b->setTransformation(a->getTransformation()*(boneObject->bone->relativeMat*transform));
        boneObject->gameTick();
    }else
        b->setTransformation(a->getTransformation()*transform);
}

void TransformLink::removeClean(BaseObject* object) {
    a->links.erase(a->links.find(this));
    b->links.erase(b->links.find(this));
    if(object != b)
        b->removeClean();
    delete this;
}

void TransformLink::removeFast(BaseObject* object) {
    if(object != b && dynamic_cast<BoneObject*>(b)) {
        b->links.erase(b->links.find(this));
        b->removeFast();
        delete this;
    }else
        BaseLink::removeFast(object);
}

rapidxml::xml_node<xmlUsedCharType>* TransformLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseLink::write(doc, linkSaver);
    node->name("TransformLink");
    
    node->append_node(writeTransformationXML(doc, transform));
    return node;
}