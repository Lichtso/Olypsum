//
//  LevelManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#include "AppMain.h"

LevelManager::LevelManager() {
    
}

void LevelManager::showErrorModal(const std::string& error) {
    GUIFramedView* modalView = new GUIFramedView();
    modalView->width = currentScreenView->width*0.4;
    modalView->height = currentScreenView->height*0.4;
    GUILabel* label = new GUILabel();
    label->color = Color4(1, 0, 0);
    label->posY = modalView->height*0.65;
    label->text = localization.localizeString("error");
    label->fontHeight = currentScreenView->height*0.14;
    modalView->addChild(label);
    label = new GUILabel();
    label->width = modalView->width*0.9;
    label->text = error;
    label->fontHeight = currentScreenView->height*0.09;
    modalView->addChild(label);
    GUIButton* button = new GUIButton();
    button->posY = modalView->height*-0.7;
    button->width = currentScreenView->width*0.14;
    button->sizeAlignment = GUISizeAlignment_Height;
    modalView->addChild(button);
    label = new GUILabel();
    label->text = localization.localizeString("ok");
    label->fontHeight = currentScreenView->height*0.1;
    button->addChild(label);
    button->onClick = [](GUIButton* button) {
        currentScreenView->setModalView(NULL);
    };
    currentScreenView->setModalView(modalView);
}

btCollisionShape* LevelManager::getCollisionShape(const std::string& name) {
    btCollisionShape* shape = levelManager.sharedCollisionShapes[name];
    if(shape) return shape;
    log(error_log, std::string("Couldn't find collision shape with id ")+name+'.');
    return NULL;
}

std::string LevelManager::getCollisionShapeName(btCollisionShape* shape) {
    for(auto iterator : sharedCollisionShapes)
        if(iterator.second == shape)
            return iterator.first;
    return "";
}

void LevelManager::clear() {
    if(scriptManager)
        scriptManager->callFunctionOfScript(scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName),
                                            "onleave", false, { });
    levelId = "";
    saveGameName = "";
    levelPackage = NULL;
    gameStatus = noGame;
    for(auto iterator: sharedCollisionShapes)
        delete iterator.second;
    sharedCollisionShapes.clear();
    objectManager.clear();
    fileManager.clear();
    fileManager.getPackage("Default");
    menu.setMenu(Menu::Name::main);
}

bool LevelManager::loadLevel(const std::string& nextLevelId) {
    LevelLoader levelLoader;
    if(levelLoader.loadLevel(nextLevelId))
        return true;
    if(!currentScreenView->modalView)
        showErrorModal(localization.localizeString("packageError_Corrupted"));
    return false;
}

bool LevelManager::saveLevel(const std::string& localData, const std::string& globalData) {
    LevelSaver levelSaver;
    return levelSaver.saveLevel(localData, globalData);
}

bool LevelManager::loadGame(const std::string& name) {
    std::string path = gameDataDir+"Saves/"+name+'/';
    if(!checkDir(path)) {
        showErrorModal(localization.localizeString("packageError_NotFound"));
        return false;
    }
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", false);
    if(!fileData) {
        showErrorModal(localization.localizeString("packageError_FilesMissing"));
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.first_node("Status");
    if(strcmp(statusNode->first_node("Version")->first_attribute("value")->value(), VERSION) != 0) {
        showErrorModal(localization.localizeString("packageError_Version"));
        return false;
    }
    saveGameName = name;
    levelPackage = fileManager.getPackage(statusNode->first_node("Package")->first_attribute("value")->value());
    loadLevel(statusNode->first_node("Level")->first_attribute("value")->value());
    return true;
}

bool LevelManager::newGame(const std::string& packageName, const std::string& name) {
    if(!createDir(gameDataDir+"Saves/"+name+'/')) {
        showErrorModal(localization.localizeString("packageError_Exists"));
        return false;
    }
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.allocate_node(rapidxml::node_element);
    statusNode->name("Status");
    doc.append_node(statusNode);
    addXMLNode(doc, statusNode, "Version", VERSION);
    addXMLNode(doc, statusNode, "Package", packageName.c_str());
    addXMLNode(doc, statusNode, "Level", "start");
    writeXmlFile(doc, gameDataDir+"Saves/"+name+"/Status.xml", true);
    return loadGame(name);
}

bool LevelManager::removeGame(const std::string& name) {
    if(!removeDir(gameDataDir+"Saves/"+name+'/')) {
        showErrorModal(localization.localizeString("packageError_NotFound"));
        return false;
    }
    return true;
}

LevelManager levelManager;