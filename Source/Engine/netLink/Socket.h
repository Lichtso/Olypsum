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

#ifndef NL_SOCKET
#define NL_SOCKET

#include "Core.h"
#include "Exception.h"

NL_NAMESPACE

class Socket : public std::streambuf {
    unsigned int readIntermediateSize = 0;
    
    struct AddrinfoDestructor {
        AddrinfoDestructor() { };
        void operator() (struct addrinfo* res) const {
            freeaddrinfo(res);
        };
    };
    std::unique_ptr<struct addrinfo, AddrinfoDestructor> getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress);
    
    void setMulticastGroup(struct sockaddr_storage* addr, bool join);
    unsigned int read(char* buffer, unsigned int size);
    unsigned int write(const char* buffer, unsigned int size);
    int sync();
    int_type underflow();
    int_type overflow(int_type c = traits_type::eof());
    
    void initSocket(bool blocking);
    Socket(int handle, const std::string& hostLocal, unsigned portLocal,
           struct sockaddr_storage* remoteAddr, IPVer ipVer);
    
    public:
    IPVer ipVer = ANY;
    SocketType type = NONE;
    std::string hostLocal, hostRemote;
    int handle = -1;
    unsigned int recvStatus = SOCKET_STATUS_NOT_CONNECTED, //! Or listen queue size if socket is TCP_SERVER
                 portLocal = 0, portRemote = 0;
    
    void initAsTcpClient(const std::string& hostRemote, unsigned portRemote, bool waitUntilConnected = false);
    void initAsTcpServer(const std::string& hostLocal, unsigned portLocal, unsigned listenQueue = 16);
    void initAsUdpPeer(const std::string& hostLocal, unsigned portLocal);
    Socket() { };
    virtual ~Socket();
    
    std::streamsize showmanyc();
    std::streamsize in_avail();
    std::streamsize sgetn(char_type* buffer, std::streamsize size);
    std::streamsize sputn(const char_type* buffer, std::streamsize size);
    
    std::streamsize getReadBufferSize();
    std::streamsize getWriteBufferSize();
    void setReadBufferSize(std::streamsize n);
    void setWriteBufferSize(std::streamsize n);
    
    void setBlockingMode(bool blocking);
    void setMulticastGroup(const std::string& address, bool join);
    Socket* accept();
    void disconnect();
};

NL_NAMESPACE_END

#endif
