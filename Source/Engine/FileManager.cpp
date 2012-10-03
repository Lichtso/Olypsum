//
//  FileManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

FilePackage::FilePackage(std::string nameB) {
    name = nameB;
}

FilePackage::~FilePackage() {
    std::map<std::string, Model*>::iterator modelIterator;
    for(modelIterator = models.begin(); modelIterator != models.end(); modelIterator ++)
        delete modelIterator->second;
    models.clear();
    
    std::map<std::string, Texture*>::iterator textureIterator;
    for(textureIterator = textures.begin(); textureIterator != textures.end(); textureIterator ++)
        delete textureIterator->second;
    textures.clear();
    
    std::map<std::string, SoundTrack*>::iterator soundTrackIterator;
    for(soundTrackIterator = soundTracks.begin(); soundTrackIterator != soundTracks.end(); soundTrackIterator ++)
        delete soundTrackIterator->second;
    soundTracks.clear();
}

bool FilePackage::load() {
    path = resourcesDir+"Packages/"+name+'/';
    DIR* dir = opendir(path.c_str());
    if(dir == NULL) {
        path = gameDataDir+"Packages/"+name+'/';
        DIR* dirB = opendir(path.c_str());
        if(dirB == NULL) return false;
        closedir(dirB);
    }else closedir(dir);
    
    if(name == "Default") localization.strings.clear();
    localization.loadLocalization(path+"Languages/"+localization.selected+".xml");
    return true;
}

std::string FilePackage::getUrlOfFile(const char* groupName, const char* fileName) {
    return path+std::string(groupName)+'/'+std::string(fileName);
}

Texture* FilePackage::getTexture(const char* fileName, GLenum format) {
    Texture* texture;
    std::map<std::string, Texture*>::iterator iterator = textures.find(std::string(fileName));
    if(iterator != textures.end()) {
        texture = iterator->second;
        texture->useCounter ++;
        return texture;
    }
    texture = new Texture();
    std::string url = getUrlOfFile("Textures", fileName);
    if(texture->loadImageInRAM(url.c_str())) {
        if(format == 0)
            return texture;
        else if(format == GL_NORMAL_MAP)
            mainFBO.generateNormalMap(texture, 4.0);
        else
            texture->uploadToVRAM(GL_TEXTURE_2D, format);
        
        texture->unloadFromRAM();
        textures[std::string(fileName)] = texture;
        return texture;
    }
    delete texture;
    return NULL;
}

Model* FilePackage::getModel(const char* fileName) {
    Model* model;
    std::map<std::string, Model*>::iterator iterator = models.find(std::string(fileName));
    if(iterator != models.end()) {
        model = iterator->second;
        model->useCounter ++;
        return model;
    }
    model = new Model();
    std::string url = getUrlOfFile("Models", fileName);
    if(model->loadCollada(this, url.c_str())) {
        models[std::string(fileName)] = model;
        return model;
    }
    delete model;
    return NULL;
}

SoundTrack* FilePackage::getSoundTrack(const char* fileName) {
    SoundTrack* soundTrack;
    std::map<std::string, SoundTrack*>::iterator iterator = soundTracks.find(std::string(fileName));
    if(iterator != soundTracks.end()) {
        soundTrack = iterator->second;
        soundTrack->useCounter ++;
        return false;
    }
    soundTrack = new SoundTrack();
    std::string url = getUrlOfFile("Sounds", fileName);
    if(soundTrack->loadOgg(url.c_str())) {
        soundTracks[std::string(fileName)] = soundTrack;
        return soundTrack;
    }
    delete soundTrack;
    return NULL;
}

bool FilePackage::releaseTexture(Texture* texture) {
    std::map<std::string, Texture*>::iterator iterator;
    for(iterator = textures.begin(); iterator != textures.end(); iterator ++)
        if(iterator->second == texture) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            textures.erase(iterator);
            return true;
        }
    return false;
}

bool FilePackage::releaseModel(Model* model) {
    std::map<std::string, Model*>::iterator iterator;
    for(iterator = models.begin(); iterator != models.end(); iterator ++)
        if(iterator->second == model) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            models.erase(iterator);
            return true;
        }
    return false;
}

bool FilePackage::releaseSoundTrack(SoundTrack* soundTrack) {
    std::map<std::string, SoundTrack*>::iterator iterator;
    for(iterator = soundTracks.begin(); iterator != soundTracks.end(); iterator ++)
        if(iterator->second == soundTrack) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            soundTracks.erase(iterator);
            return true;
        }
    return false;
}



FileManager::~FileManager() {
    clear();
}

void FileManager::clear() {
    for(auto iterator : filePackages)
        delete iterator.second;
    filePackages.clear();
}

static bool readOptionBool(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    return strcmp(attribute->value(), "true") == 0;
}

static unsigned int readOptionUInt(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    unsigned int value;
    sscanf(attribute->value(), "%d", &value);
    return value;
}

void FileManager::loadOptions() {
    createDir(gameDataDir+"Saves/");
    createDir(gameDataDir+"Packages/");
    localization.selected = "English";
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    unsigned int fileSize;
    char* fileData = readXmlFile(doc, gameDataDir+"Options.xml", fileSize, false);
    if(fileData) {
        if(strcmp(doc.first_node("version")->first_attribute("value")->value(), VERSION) != 0) {
            delete [] fileData;
            saveOptions();
            return;
        }
        localization.selected = doc.first_node("language")->first_attribute("value")->value();
        rapidxml::xml_node<xmlUsedCharType>* graphics = doc.first_node("graphics");
        screenBlurFactor = (readOptionBool(graphics->first_node("screenBlurEnabled"))) ? 0.0 : -1.0;
        edgeSmoothEnabled = readOptionBool(graphics->first_node("edgeSmoothEnabled"));
        fullScreenEnabled = readOptionBool(graphics->first_node("fullScreenEnabled"));
        cubemapsEnabled = readOptionBool(graphics->first_node("cubemapsEnabled"));
        depthOfFieldQuality = readOptionUInt(graphics->first_node("depthOfFieldQuality"));
        bumpMappingQuality = readOptionUInt(graphics->first_node("bumpMappingQuality"));
        shadowQuality = readOptionUInt(graphics->first_node("shadowQuality"));
        ssaoQuality = readOptionUInt(graphics->first_node("ssaoQuality"));
        blendingQuality = readOptionUInt(graphics->first_node("blendingQuality"));
        particleCalcTarget = readOptionUInt(graphics->first_node("particleCalcTarget"));
        delete [] fileData;
    }else saveOptions();
    
    loadPackage("Default");
}

void FileManager::saveOptions() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    addXMLNode(doc, &doc, "version", VERSION);
    addXMLNode(doc, &doc, "language", localization.selected.c_str());
    rapidxml::xml_node<xmlUsedCharType>* graphics = doc.allocate_node(rapidxml::node_element);
    graphics->name("graphics");
    doc.append_node(graphics);
    
    char str[24];
    addXMLNode(doc, graphics, "screenBlurEnabled", (screenBlurFactor > -1.0) ? "true" : "false");
    addXMLNode(doc, graphics, "edgeSmoothEnabled", (edgeSmoothEnabled) ? "true" : "false");
    addXMLNode(doc, graphics, "fullScreenEnabled", (fullScreenEnabled) ? "true" : "false");
    addXMLNode(doc, graphics, "cubemapsEnabled", (cubemapsEnabled) ? "true" : "false");
    sprintf(&str[0], "%d", depthOfFieldQuality);
    addXMLNode(doc, graphics, "depthOfFieldQuality", &str[0]);
    sprintf(&str[4], "%d", bumpMappingQuality);
    addXMLNode(doc, graphics, "bumpMappingQuality", &str[4]);
    sprintf(&str[8], "%d", shadowQuality);
    addXMLNode(doc, graphics, "shadowQuality", &str[8]);
    sprintf(&str[12], "%d", ssaoQuality);
    addXMLNode(doc, graphics, "ssaoQuality", &str[12]);
    sprintf(&str[16], "%d", blendingQuality);
    addXMLNode(doc, graphics, "blendingQuality", &str[16]);
    sprintf(&str[20], "%d", particleCalcTarget);
    addXMLNode(doc, graphics, "particleCalcTarget", &str[20]);
    
    writeXmlFile(doc, gameDataDir+"Options.xml");
}

bool FileManager::loadPackage(const char* name) {
    FilePackage* package = new FilePackage(name);
    if(!package->load()) {
        delete package;
        return false;
    }
    filePackages[name] = package;
    return true;
}

FilePackage* FileManager::getPackage(const char* nameStr) {
    std::string name;
    if(nameStr) name = nameStr;
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) {
        FilePackage* package = new FilePackage(nameStr);
        if(!package->load()) {
            delete package;
            return NULL;
        }
        filePackages[name] = package;
        return package;
    }
    return iterator->second;
}

void FileManager::unloadPackage(const char* nameStr) {
    std::string name;
    if(nameStr) name = nameStr;
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) return;
    delete iterator->second;
    filePackages.erase(iterator);
}

void FileManager::releaseTexture(Texture* texture) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseTexture(texture)) return;
    char buffer[64];
    sprintf(buffer, "Texture allready released: %p\n", texture);
    log(error_log, buffer);
}

void FileManager::releaseModel(Model* model) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseModel(model)) return;
    char buffer[64];
    sprintf(buffer, "Model allready released: %p\n", model);
    log(error_log, buffer);
}

void FileManager::releaseSoundTrack(SoundTrack* soundTrack) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseSoundTrack(soundTrack)) return;
    char buffer[64];
    sprintf(buffer, "SoundTrack allready released: %p\n", soundTrack);
    log(error_log, buffer);
}

FileManager fileManager;