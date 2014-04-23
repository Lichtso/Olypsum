//
//  Network.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "Menu/Menu.h"

void NetworkManager::init() {
#ifdef WIN32
    netLink::init();
#endif
    
    socketManager.onReceiveMsgPack = [this](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
        std::ostringstream stream;
        stream << *element;
        log(network_log, stream.str());
    };
}

void NetworkManager::gameTick() {
    try {
        socketManager.listen();
    }catch(netLink::Exception exep) {
        log(network_log, std::string("Listen Exception ")+stringOf(exep.code));
    }
    profiler.leaveSection("Networking");
}

bool NetworkManager::enable() {
    if(udpSocket) disable();
    
    try {
        udpSocket = socketManager.newMsgPackSocket();
        
        if(optionsState.ipVersion == "ipV4")
            udpSocket->initAsUdpPeer("0.0.0.0", udpPort);
        else if(optionsState.ipVersion == "ipV6")
            udpSocket->initAsUdpPeer("::0", udpPort);
        else
            udpSocket->initAsUdpPeer("*", udpPort);
        
        udpSocket->hostRemote = (udpSocket->getIPVersion() == netLink::IPv4) ? scanIPv4 : scanIPv6;
        udpSocket->portRemote = udpPort;
        udpSocket->setMulticastGroup(udpSocket->hostRemote, true);
        
        std::vector<std::unique_ptr<MsgPack::Element>> packet;
        packet.push_back(std::unique_ptr<MsgPack::Element>(new MsgPack::String("nickname")));
        packet.push_back(std::unique_ptr<MsgPack::Element>(new MsgPack::String(optionsState.nickname)));
        
        netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(udpSocket.get());
        msgPackSocket << new MsgPack::Map(packet);
        
        return true;
    }catch(netLink::Exception exep) {
        log(network_log, std::string("Enable Exception ")+stringOf(exep.code));
        disable();
        return false;
    }
}

void NetworkManager::disable() {
    if(udpSocket) {
        socketManager.sockets.erase(udpSocket);
        udpSocket.reset();
    }
}

NetworkManager networkManager;