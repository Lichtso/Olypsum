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
    
    socketManager.onReceive = [this](netLink::SocketManager* manager, netLink::Socket* socket) {
        try {
            socket->advanceInputBuffer();
            MsgPack::Deserializer deserializer(socket);
            deserializer.deserialize([this](std::unique_ptr<MsgPack::Element> parsed) {
				std::ostringstream stream;
				stream << *parsed;
				log(network_log, stream.str());
                return false;
            });
        }catch(netLink::Exception exc) {
            log(network_log, "Exception "+stringOf(exc.code));
        }
    };
}

void NetworkManager::gameTick() {
	try {
		socketManager.listen();
	}catch(netLink::Exception exc) {
		log(network_log, "Exception "+stringOf(exc.code));
	}
    profiler.leaveSection("Networking");
}

void NetworkManager::enable() {
    if(udpSocket) disable();
    
    udpSocket.reset(new netLink::Socket());
    udpSocket->setInputBufferSize(10000);
    udpSocket->setOutputBufferSize(10000);
    socketManager.sockets.insert(udpSocket);
    
    if(optionsState.ipVersion == "ipV4")
        udpSocket->initAsUdpPeer("0.0.0.0", udpPort);
    else if(optionsState.ipVersion == "ipV6")
        udpSocket->initAsUdpPeer("::0", udpPort);
    else
        udpSocket->initAsUdpPeer("*", udpPort);
    
    udpSocket->hostRemote = (udpSocket->getIPVersion() == netLink::IPv4) ? scanIPv4 : scanIPv6;
    udpSocket->portRemote = udpPort;
    udpSocket->setMulticastGroup(udpSocket->hostRemote, true);
    
    try {
        MsgPack::Serializer serializer(udpSocket.get());
        serializer << new MsgPack::MapHeader(1);
        serializer << "nickname";
        serializer << optionsState.nickname;
        serializer.serialize();
        udpSocket->pubsync();
    }catch(netLink::Exception exc) {
        log(network_log, "Exception "+stringOf(exc.code));
    }
}

void NetworkManager::disable() {
    if(udpSocket) {
        socketManager.sockets.erase(udpSocket);
        udpSocket.reset();
    }
}

NetworkManager networkManager;