//
//  Network.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
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
	socketManager.listen();
    profiler.leaveSection("Networking");
}

void NetworkManager::enable() {
    if(udpSocket) disable();
    
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
    
    netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(udpSocket.get());
    msgPackSocket << new MsgPack::MapHeader(1);
    msgPackSocket << new MsgPack::String("nickname");
    msgPackSocket << new MsgPack::String(optionsState.nickname);
}

void NetworkManager::disable() {
    if(udpSocket) {
        socketManager.sockets.erase(udpSocket);
        udpSocket.reset();
    }
}

NetworkManager networkManager;