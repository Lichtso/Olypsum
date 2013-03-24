/*
    NetLink Sockets: Networking C++ library
    Copyright 2012 Pedro Francisco Pareja Ruiz (PedroPareja@Gmail.com)
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NL_SOCKET_GROUP
#define NL_SOCKET_GROUP

#include "Socket.h"

NL_NAMESPACE

class SocketManager {
    public:
    std::set<Socket*> sockets;
    std::function<bool(SocketManager* group, Socket* serverSocket, Socket* socket)> onAcceptRequest;
    std::function<void(SocketManager* group, Socket* socket, SocketRecvStatus prev)> onStateChanged;
    std::function<void(SocketManager* group, Socket* socket)> onDisconnect;
    std::function<void(SocketManager* group, Socket* socket)> onReceive;
    ~SocketManager();
    void listen(double sec = 0.0);
};

NL_NAMESPACE_END

#endif
