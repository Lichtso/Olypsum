//
//  GamesMenu.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.12.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

static GUIImage* getThumbnailOfPackage(FilePackage* package) {
    std::string name = "thumbnail";
    package->findFileByNameInSubdir("Textures/", name);
    if(name.size() > 0) {
        FileResourcePtr<Texture> thumbnail = package->getResource<Texture>(name);
        if(thumbnail) {
            GUIImage* image = new GUIImage();
            image->sizeAlignment = GUISizeAlignment::Width;
            image->texture = thumbnail;
            image->texture->uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGBA);
            return image;
        }
    }
    return NULL;
}

void Menu::setSaveGamesMenu() {
    clearAndAddBackground();
    current = saveGames;
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("saveGames");
    label->fontHeight = screenView->height*0.2;
    screenView->addChild(label);
    
    GUIScrollView* scrollView = new GUIScrollView();
    scrollView->width = screenView->width*0.97;
    scrollView->height = screenView->height*0.7;
    screenView->addChild(scrollView);
    
    size_t validSaveGames = 0;
    forEachInDir(supportPath+"Saves/", nullptr, [this, &validSaveGames, &scrollView](const std::string& directoryPath, std::string name) {
        std::string path = supportPath+"Saves/"+name;
        rapidxml::xml_document<xmlUsedCharType> doc;
        std::unique_ptr<char[]> fileData = readXmlFile(doc, supportPath+"Saves/"+name+"Status.xml", false);
        if(!fileData) return false;
        rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("Status");
        FilePackage* package = fileManager.loadPackage(node->first_node("Package")->first_attribute()->value());
        if(!package) return false;
        name.pop_back();
        
        std::string container = node->first_node("Level")->first_attribute()->value();
        GUIButton* button = new GUIButton();
        button->posX = screenView->width*-0.18;
        button->posY = scrollView->height*(0.8-validSaveGames*0.4);
        button->onClick = [name, package, container](GUIButton* button) {
            levelManager.loadGame(package, name, container);
        };
        scrollView->addChild(button);
        GUILabel* label = new GUILabel();
        label->text = name;
        label->posX = screenView->width*-0.25;
        label->posY = screenView->height*0.05;
        label->width = screenView->width*0.3;
        label->fontHeight = screenView->height*0.1;
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        button->addChild(label);
        label = new GUILabel();
        label->text = package->name;
        label->posX = screenView->width*0.1;
        label->posY = -screenView->height*0.05;
        label->width = screenView->width*0.65;
        label->fontHeight = screenView->height*0.1;
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        button->addChild(label);
        if((node = node->first_node("Description")) && (node = node->first_node()) && node->type() == rapidxml::node_cdata) {
            label = new GUILabel();
            label->text = node->value();
            label->posX = screenView->width*0.45;
            label->posY = screenView->height*0.05;
            label->width = screenView->width*0.3;
            label->fontHeight = screenView->height*0.1;
            label->textAlignment = GUILabel::TextAlignment::Right;
            label->sizeAlignment = GUISizeAlignment::Height;
            button->addChild(label);
        }
        GUIImage* image = getThumbnailOfPackage(package);
        if(image) {
            image->height = screenView->height*0.09;
            image->posX = screenView->width*-0.65;
            button->addChild(image);
        }
        
        button = new GUIButton();
        button->posX = screenView->width*0.8;
        button->posY = scrollView->height*(0.8-validSaveGames*0.4);
        button->width = screenView->width*0.14;
        button->sizeAlignment = GUISizeAlignment::Height;
        scrollView->addChild(button);
        label = new GUILabel();
        label->text = fileManager.localizeString("remove");
        label->fontHeight = screenView->height*0.1;
        button->addChild(label);
        button->onClick = [this, name](GUIButton* button) {
            setModalView("remove", fileManager.localizeString("name")+": "+name,
                         [this, name](GUIButton* button) {
                             removeDir(supportPath+"Saves"+SYSTEM_SLASH+name+SYSTEM_SLASH);
                             setSaveGamesMenu();
                         });
        };
        
        validSaveGames ++;
        return false;
    }, nullptr);
    scrollView->contentHeight = validSaveGames*0.4*scrollView->height;
    
    std::function<void(GUIButton*)> onClick[] = {
        [this](GUIButton* button) {
            setMainMenu();
        }, [this](GUIButton* button) {
            setNewGameMenu();
        }
    };
    const char* buttonLabels[] = { "back", "newGame" };
    for(unsigned char i = 0; i < 2; i ++) {
        GUIButton* button = new GUIButton();
        button->posX = screenView->width*(-0.8+i*1.6);
        button->posY = screenView->height*-0.8;
        button->width = screenView->width*0.14;
        button->sizeAlignment = GUISizeAlignment::Height;
        screenView->addChild(button);
        GUILabel* label = new GUILabel();
        label->text = fileManager.localizeString(buttonLabels[i]);
        label->fontHeight = screenView->height*0.1;
        button->addChild(label);
        button->onClick = onClick[i];
    }
    
    screenView->updateContent();
}

void Menu::setNewGameMenu() {
    clearAndAddBackground();
    current = newGame;
    
    fileManager.loadAllPackages();
    
    GUILabel* label = new GUILabel();
    label->posY = screenView->height*0.88;
    label->text = fileManager.localizeString("newGame");
    label->fontHeight = screenView->height*0.2;
    screenView->addChild(label);
    
    GUIScrollView* scrollView = new GUIScrollView();
    scrollView->visible = false;
    scrollView->width = screenView->width*0.97;
    scrollView->height = screenView->height*0.7;
    scrollView->contentHeight = (fileManager.filePackages.size()-1)*0.4*scrollView->height;
    screenView->addChild(scrollView);
    
    GUITextField* textField = new GUITextField();
    label = static_cast<GUILabel*>(textField->children[0]);
    label->text = fileManager.localizeString("newGame");
    textField->posX = 0.0;
    textField->posY = screenView->height*-0.8;
    textField->width = screenView->width*0.4;
    textField->height = screenView->height*0.07;
    textField->onChange = [scrollView](GUITextField* textField) {
        GUILabel* label = static_cast<GUILabel*>(textField->children[0]);
        std::string path = supportPath+"Saves"+SYSTEM_SLASH+label->text+SYSTEM_SLASH;
        scrollView->visible = !checkDir(path) && label->getLength() >= 3;
    };
    screenView->addChild(textField);
    
    size_t i = 0;
    for(auto packageIterator : fileManager.filePackages) {
        FilePackage* package = packageIterator.second;
        if(package->name == "Core") continue;
        
        GUIButton* button = new GUIButton();
        button->posY = scrollView->height*(0.8-(i ++)*0.4);
        button->onClick = [package, textField](GUIButton* button) {
            levelManager.newGame(package, static_cast<GUILabel*>(textField->children[0])->text);
        };
        scrollView->addChild(button);
        label = new GUILabel();
        label->text = package->name;
        label->posX = screenView->width*0.1;
        label->posY = screenView->height*0.05;
        label->width = screenView->width*0.8;
        label->fontHeight = screenView->height*0.1;
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        button->addChild(label);
        label = new GUILabel();
        label->text = package->description;
        label->posX = screenView->width*0.1;
        label->posY = -screenView->height*0.05;
        label->width = screenView->width*0.8;
        label->fontHeight = screenView->height*0.1;
        label->textAlignment = GUILabel::TextAlignment::Left;
        label->sizeAlignment = GUISizeAlignment::Height;
        button->addChild(label);
        GUIImage* image = getThumbnailOfPackage(package);
        if(image) {
            image->height = screenView->height*0.09;
            image->posX = screenView->width*-0.8;
            button->addChild(image);
        }
    }
    
    std::function<void(GUIButton*)> onClick[] = {
        [this](GUIButton* button) {
            openExternURL((std::string("file://")+supportPath+"Packages").c_str());
        }, [this](GUIButton* button) {
            setSaveGamesMenu();
        }
    };
    const char* buttonLabels[] = { "packages", "cancel" };
    for(unsigned char i = 0; i < 2; i ++) {
        GUIButton* button = new GUIButton();
        button->posX = screenView->width*(-0.8+i*1.6);
        button->posY = screenView->height*-0.8;
        button->width = screenView->width*0.14;
        button->sizeAlignment = GUISizeAlignment::Height;
        screenView->addChild(button);
        GUILabel* label = new GUILabel();
        label->text = fileManager.localizeString(buttonLabels[i]);
        label->fontHeight = screenView->height*0.1;
        button->addChild(label);
        button->onClick = onClick[i];
    }
    
    screenView->updateContent();
}