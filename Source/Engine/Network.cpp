//
//  Network.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

void NetworkManager::init() {
    udpSocket = new netLink::Socket();
    udpSocket->setInputBufferSize(128);
    udpSocket->setOutputBufferSize(128);
    udpSocket->initAsUdpPeer("::0", udpPort);
    socketManager.sockets.insert(udpSocket);
    socketManager.onReceive = [this](netLink::SocketManager* manager, netLink::Socket* socket) {
        socket->advanceToNextPacket();
        netLink::MsgPack::Stream stream(udpSocket);
        printf("UDP received MAP: %lu\n", stream.readMap());
        std::string str;
        stream >> str;
        printf("UDP received STR: %s\n", str.c_str());
        stream >> str;
        printf("UDP received STR: %s\n", str.c_str());
    };
}

void NetworkManager::gameTick() {
    socketManager.listen();
    profiler.leaveSection("Networking");
}

void NetworkManager::setLocalScan(bool active) {
    udpSocket->setMulticastGroup(scanIPv6, active);
    
    if(!active) return;
    
    char buffer[128];
    FILE* f = popen("hostname", "r");
    fgets(buffer, 128, f);
    pclose(f);
    size_t len = strlen(buffer)-1;
    buffer[len] = 0;
    udpSocket->hostRemote = scanIPv6;
    udpSocket->portRemote = udpPort;
    netLink::MsgPack::Stream stream(udpSocket);
    stream.writeMap(1);
    stream << "name";
    stream << buffer;
    udpSocket->pubsync();
}

NetworkManager networkManager;