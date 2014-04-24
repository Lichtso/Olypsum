//
//  Network.h
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef Network_h
#define Network_h

#include "Utilities.h"

class NetworkManager {
    const int udpPort = 8724, tcpPort = 8725;
    // http://en.wikipedia.org/wiki/Multicast_address
    const char* scanIPv4 = "224.0.0.100";
    const char* scanIPv6 = "FF02:0001::";
    std::shared_ptr<netLink::Socket> udpSocket;
    netLink::SocketManager socketManager;
    
    public:
    void init();
    void gameTick();
    bool enable();
    void disable();
};

extern NetworkManager networkManager;

#endif