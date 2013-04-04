//
//  Network.h
//  Olypsum
//
//  Created by Alexander Meißner on 31.03.13.
//
//

#include "Utilities.h"

#ifndef Network_h
#define Network_h

class NetworkManager {
    const int udpPort = 8724, tcpPort = 8725;
    const char* scanIPv4 = "224.0.1.0";
    const char* scanIPv6 = "FF02:0001::";
    NL::Socket udpSocket;
    NL::SocketManager socketManager;
    
    public:
    void init();
    void gameTick();
    void setLocalScan(bool active);
};

extern NetworkManager networkManager;

#endif