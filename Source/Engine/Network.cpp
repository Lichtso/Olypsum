//
//  Network.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

void NetworkManager::init() {
    socketManager.onReceive = [this](NL::SocketManager* manager, NL::Socket* socket) {
        
    };
    
    udpSocket.initAsUdpPeer("0.0.0.0", udpPort);
    udpSocket.setInputBufferSize(65536);
    udpSocket.setOutputBufferSize(2048);
}

void NetworkManager::gameTick() {
    socketManager.listen();
    profiler.leaveSection("Networking");
}

void NetworkManager::setLocalScan(bool active) {
    udpSocket.setMulticastGroup(scanIPv4, active);
    
}

NetworkManager networkManager;