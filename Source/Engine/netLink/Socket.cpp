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


#include "socket.h"

NL_NAMESPACE

#ifdef OS_WIN32

static void close(int handle) {
    closesocket(handle);
}
/*
static void freeaddrinfo(PADDRINFOA addrInfo) {
    ::freeaddrinfo(addrInfo);
}

static const char* inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
    if(af == AF_INET) {
        struct sockaddr_in in;
        memset(&in, 0, sizeof(in));
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }else if(af == AF_INET6) {
        struct sockaddr_in6 in;
        memset(&in, 0, sizeof(in));
        in.sin6_family = AF_INET6;
        memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    return NULL;
}
*/
#endif

static int getSocketErrorCode() {
    #ifdef OS_WIN32
        return WSAGetLastError();
    #else
        return errno;
    #endif
}

static void getSocketInfoFromStorage(struct sockaddr_storage* storage, std::string& host, unsigned int& port) {
    char buffer[INET6_ADDRSTRLEN];
    if(reinterpret_cast<struct sockaddr*>(storage)->sa_family == AF_INET) {
        auto sin = reinterpret_cast<struct sockaddr_in*>(storage);
        port = ntohs(sin->sin_port);
        inet_ntop(storage->ss_family, &(sin->sin_addr), buffer, sizeof(buffer));
    }else{
        auto sin = reinterpret_cast<struct sockaddr_in6*>(storage);
        port = ntohs(sin->sin6_port);
        inet_ntop(storage->ss_family, &(sin->sin6_addr), buffer, sizeof(buffer));
    }
    host = buffer;
}

static void getSocketInfoFromHandle(int handle, std::string& host, unsigned int& port) {
    struct sockaddr_storage storage;
    #ifdef OS_WIN32
        int size = sizeof(&storage);
    #else
        socklen_t size = sizeof(&storage);
    #endif
    
    if(getsockname(handle, (struct sockaddr*)&storage, &size) != 0)
        throw Exception(Exception::ERROR_GET_ADDR_INFO, "Socket:: error getting socket info", getSocketErrorCode());
    
    getSocketInfoFromStorage(&storage, host, port);
}

static void checkReadError(const std::string& functionName) {
    #ifdef OS_WIN32
        if(WSAGetLastError() != WSAEWOULDBLOCK)
            throw Exception(Exception::ERROR_READ, std::string("Socket::") + functionName + ": error detected", getSocketErrorCode());
    #else
        if(errno != EAGAIN && errno != EWOULDBLOCK)
            throw Exception(Exception::ERROR_READ, std::string("Socket::") + functionName + ": error detected", getSocketErrorCode());
    #endif
}

std::unique_ptr<struct addrinfo, Socket::AddrinfoDestructor> Socket::getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress) {
    struct addrinfo conf, *res;
    memset(&conf, 0, sizeof(conf));
    conf.ai_flags = AI_V4MAPPED;
    conf.ai_family = AF_UNSPEC;
    
    if(wildcardAddress)
        conf.ai_flags |= AI_PASSIVE;
    
    switch(type) {
        case TCP_CLIENT:
        case TCP_SERVER:
            conf.ai_socktype = SOCK_STREAM;
        break;
        case UDP_PEER:
            conf.ai_socktype = SOCK_DGRAM;
        break;
        default:
            throw Exception(Exception::BAD_PROTOCOL, "Socket::initSocket: bad protocol");
    }
    
    /*
    switch(ipVer) {
        case IP4:
            conf.ai_family = AF_INET;
        break;
        case IP6:
            conf.ai_family = AF_INET6;
        break;
        case ANY:
            conf.ai_family = AF_UNSPEC;
        break;
        default:
            throw Exception(Exception::BAD_IP_VER, "Socket::initSocket: bad ip version parameter");
    }*/
    
    char portStr[10];
    snprintf(portStr, 10, "%u", port);
    int status = getaddrinfo(host, portStr, &conf, &res);
    
    if(status != 0) {
        std::string errorMsg = "Socket::initSocket: Error setting addrInfo: ";
        #ifndef _MSC_VER
        errorMsg += gai_strerror(status);
        #endif
        
        throw Exception(Exception::ERROR_SET_ADDR_INFO, errorMsg, getSocketErrorCode());
    }
    
    return std::unique_ptr<struct addrinfo, Socket::AddrinfoDestructor>(res, AddrinfoDestructor());
}

void Socket::setMulticastGroup(struct sockaddr_storage* addr, bool join) {
    if(ipVer == IP4) {
        auto sin = reinterpret_cast<struct sockaddr_in*>(addr)->sin_addr;
        if((ntohl(sin.s_addr) & 0xF0000000) == 0xE0000000) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr = sin;
            mreq.imr_interface.s_addr = INADDR_ANY;
            
            if(setsockopt(handle, IPPROTO_IP, (join) ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) == -1)
                throw Exception(Exception::ERROR_SET_SOCK_OPT, "Socket::setMulticastGroup: Error establishing socket options", getSocketErrorCode());
        }
    }else{
        auto sin = reinterpret_cast<struct sockaddr_in6*>(addr)->sin6_addr;
        if(sin.__u6_addr.__u6_addr8[0] == 0xFF) {
            struct ipv6_mreq mreq;
            mreq.ipv6mr_multiaddr = sin;
            mreq.ipv6mr_interface = 0;
            
            if(setsockopt(handle, IPPROTO_IPV6, (join) ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, &mreq, sizeof(ipv6_mreq)) == -1)
                throw Exception(Exception::ERROR_SET_SOCK_OPT, "Socket::setMulticastGroup: Error establishing socket options", getSocketErrorCode());
        }
    }
}

unsigned int Socket::read(char* buffer, unsigned int size) {
    switch(type) {
        case UDP_PEER: {
            struct sockaddr_storage remoteAddr;
            socklen_t addrSize = sizeof(remoteAddr);
            int status = recvfrom(handle, buffer, size, 0, reinterpret_cast<struct sockaddr*>(&remoteAddr), &addrSize);
            
            if(status == -1) {
                portRemote = 0;
                hostRemote = "";
                checkReadError("readFrom");
            }else
                getSocketInfoFromStorage(&remoteAddr, hostRemote, portRemote);
            
            return status;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            int status = recv(handle, buffer, size, 0);
            if(status == -1) checkReadError("read");
            return status;
        }
        case NONE:
        case TCP_SERVER:
            throw Exception(Exception::EXPECTED_NON_SERVER, "Socket::send: Expected client or peer socket");
    }
}

unsigned int Socket::write(const char* buffer, unsigned int size) {
    switch(type) {
        case UDP_PEER: {
            auto res = getSocketInfoFor(hostRemote.c_str(), portRemote, false);
            
            size_t sentBytes = 0;
            while(sentBytes < size) {
                int status = ::sendto(handle, (const char*)buffer + sentBytes, size - sentBytes, 0, res->ai_addr, res->ai_addrlen);
                
                if(status == -1)
                    throw Exception(Exception::ERROR_SEND, "Error sending data", getSocketErrorCode());
                
                sentBytes += status;
            }
            return sentBytes;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            size_t sentBytes = 0;
            while(sentBytes < size) {
                int status = ::send(handle, (const char*)buffer + sentBytes, size - sentBytes, 0);
                
                if(status == -1)
                    throw Exception(Exception::ERROR_SEND, "Error sending data", getSocketErrorCode());
                
                sentBytes += status;
            }
            return sentBytes;
        }
        case NONE:
        case TCP_SERVER:
            throw Exception(Exception::EXPECTED_NON_SERVER, "Socket::send: Expected client or peer socket");
    }
}

int Socket::sync() {
    if(!getWriteBufferSize()) //No write intermediate buffer
        return -1;
    if(pptr() == pbase()) //Allready in sync
        return 0;
    try {
        write(pbase(), pptr()-pbase());
        setp(pbase(), epptr());
        return 0;
    }catch (Exception err) {
        return -1;
    }
}

Socket::int_type Socket::underflow() {
    if(!readIntermediateSize) //No read intermediate buffer
        return traits_type::eof();
    try {
        std::streamsize readBytes = read(eback(), std::min(showmanyc(), (std::streamsize)readIntermediateSize));
        setg(eback(), eback(), eback()+readBytes);
        return (readBytes) ? *eback() : traits_type::eof();
    }catch (Exception err) {
        return traits_type::eof();
    }
}

Socket::int_type Socket::overflow(int_type c) {
    if(sync() == -1)
        return traits_type::eof();
    *pbase() = c;
    pbump(1);
    return c;
}



void Socket::initSocket(bool blockingConnect) {
    #ifdef OS_WIN32
    char yes = 1;
    #else
    int yes = 1;
    #endif
    
    std::unique_ptr<struct addrinfo, Socket::AddrinfoDestructor> res;
    
    if(type == TCP_CLIENT) {
        res = getSocketInfoFor(hostRemote.c_str(), portRemote, false);
    }else{
        const char* host;
        if(!hostLocal.compare("") || !hostLocal.compare("*"))
            host = NULL;
        else
            host = hostLocal.c_str();
        res = getSocketInfoFor(host, portLocal, true);
    }
    
    struct addrinfo* nextAddr = res.get();
    while(nextAddr) {
        handle = socket(nextAddr->ai_family, nextAddr->ai_socktype, nextAddr->ai_protocol);
        if(handle == -1) {
            nextAddr = nextAddr->ai_next;
            continue;
        }
        
        setBlockingMode(blockingConnect);
        if(setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(decltype(yes))) == -1)
            throw Exception(Exception::ERROR_SET_SOCK_OPT, "Socket::initSocket: Error establishing socket options");
        
        switch(nextAddr->ai_family) {
            case AF_INET:
                ipVer = IP4;
            break;
            case AF_INET6:
                ipVer = IP6;
            break;
        }
        
        switch(type) {
            case NONE:
            case TCP_SERVERS_CLIENT:
                throw Exception(Exception::ERROR_CONNECT_SOCKET, "Socket::initSocket: unknown socket type");
            case TCP_CLIENT:
                if(connect(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1 && (blockingConnect || getSocketErrorCode() != EINPROGRESS)) {
                    close(handle);
                    handle = -1;
                }else if(blockingConnect)
                    recvStatus = SOCKET_STATUS_OPEN;
            break;
            case TCP_SERVER: {
                if(bind(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1) {
                    close(handle);
                    handle = -1;
                }
                
                if(listen(handle, recvStatus) == -1)
                    throw Exception(Exception::ERROR_CAN_NOT_LISTEN, "Socket::initSocket: could not start listening", getSocketErrorCode());
            } break;
            case UDP_PEER: {
                if(bind(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1) {
                    close(handle);
                    handle = -1;
                }
                
                setMulticastGroup(reinterpret_cast<sockaddr_storage*>(nextAddr->ai_addr), true);
                
                recvStatus = SOCKET_STATUS_OPEN;
            } break;
        }
        if(handle == -1) {
            nextAddr = nextAddr->ai_next;
            continue;
        }
        
        if(blockingConnect)
            setBlockingMode(false);
        break;
    }
    
    if(handle == -1)
        throw Exception(Exception::ERROR_CONNECT_SOCKET, "Socket::initSocket: error in socket connection/bind", getSocketErrorCode());

    getSocketInfoFromHandle(handle, hostLocal, portLocal);
}

Socket::Socket(int _handle, const std::string& _hostLocal, unsigned _portLocal,
               struct sockaddr_storage* remoteAddr, IPVer _ipVer)
                :handle(_handle), hostLocal(_hostLocal), portLocal(_portLocal),
                ipVer(_ipVer), type(TCP_SERVERS_CLIENT), recvStatus(SOCKET_STATUS_OPEN) {
    getSocketInfoFromStorage(remoteAddr, hostRemote, portRemote);
    setBlockingMode(false);
}

void Socket::initAsTcpClient(const std::string& _hostRemote, unsigned _portRemote, bool waitUntilConnected) {
    type = TCP_CLIENT;
    hostRemote = _hostRemote;
    portRemote = _portRemote;
    initSocket(waitUntilConnected);
}

void Socket::initAsTcpServer(const std::string& _hostLocal, unsigned _portLocal, unsigned _listenQueue) {
    type = TCP_SERVER;
    hostLocal = _hostLocal;
    portLocal = _portLocal;
    recvStatus = _listenQueue;
    initSocket(false);
}

void Socket::initAsUdpPeer(const std::string& _hostLocal, unsigned _portLocal) {
    type = UDP_PEER;
    hostLocal = _hostLocal;
    portLocal = _portLocal;
    initSocket(false);
}

Socket::~Socket() {
    if(type != TCP_SERVER) {
        setReadBufferSize(0);
        setWriteBufferSize(0);
    }
    disconnect();
}



std::streamsize Socket::showmanyc() {
    #ifdef OS_WIN32
    u_long result = -1;
    int status = ioctlsocket(handle, FIONREAD, &result);
    #else
    int result = -1;
    int status = ioctl(handle, FIONREAD, &result);
    #endif
    
    if(status)
        throw Exception(Exception::ERROR_IOCTL, "Socket::nextReadSize: error ioctl", getSocketErrorCode());
    
    return result;
}

std::streamsize Socket::in_avail() {
    std::streamsize bytesInSystemCache = showmanyc();
    return (bytesInSystemCache < 0) ? -1 : (bytesInSystemCache+egptr()-gptr());
}

std::streamsize Socket::sgetn(Socket::char_type* buffer, std::streamsize size) {
    if(readIntermediateSize) //Read intermediate buffer
        return xsgetn(buffer, size);
    
    try {
        return read(buffer, std::min(size, showmanyc()));
    }catch (Exception err) {
        return 0;
    }
}

std::streamsize Socket::sputn(const Socket::char_type* buffer, std::streamsize size) {
    if(getWriteBufferSize()) //Write intermediate buffer
        return xsputn(buffer, size);
    
    try {
        return write(buffer, size);
    }catch (Exception err) {
        return 0;
    }
}

std::streamsize Socket::getReadBufferSize() {
    return readIntermediateSize;
}

std::streamsize Socket::getWriteBufferSize() {
    return epptr()-pbase();
}

void Socket::setReadBufferSize(std::streamsize n) {
    if(type == TCP_SERVER)
        throw Exception(Exception::EXPECTED_NON_SERVER, "Socket::send: Expected client or peer socket");
    
    if(eback()) delete [] eback();
    if(n == 0) return;
    char* readBuffer = new char[n];
    setg(readBuffer, readBuffer, readBuffer);
    readIntermediateSize = n;
}

void Socket::setWriteBufferSize(std::streamsize n) {
    if(type == TCP_SERVER)
        throw Exception(Exception::EXPECTED_NON_SERVER, "Socket::send: Expected client or peer socket");
    
    if(pbase()) delete [] pbase();
    if(n == 0) return;
    char* writeBuffer = new char[n];
    setp(writeBuffer, writeBuffer+n);
}

void Socket::setBlockingMode(bool blocking) {
    int result = -1;
    
    #ifdef OS_WIN32
    u_long non_blocking = !blocking;
    result = ioctlsocket(handle, FIONBIO, &non_blocking);
    if(result != 0)
        throw Exception(Exception::ERROR_IOCTL, "Socket::blocking: ioctl error", getSocketErrorCode());
    #else
    int flags = fcntl(handle, F_GETFL);
    if(blocking)
        result = fcntl(handle, F_SETFL, flags & ~O_NONBLOCK);
    else
        result = fcntl(handle, F_SETFL, flags | O_NONBLOCK);
    if(result == -1)
        throw Exception(Exception::ERROR_IOCTL, "Socket::blocking: ioctl error", getSocketErrorCode());
    #endif
}

void Socket::setMulticastGroup(const std::string& address, bool join) {
    if(type != UDP_PEER)
        throw Exception(Exception::EXPECTED_UDP_PEER, "Socket::setMulticastGroup: Expected UDP_PEER socket");
    
    struct sockaddr_storage addr;
    if(inet_pton((ipVer == IP4) ? AF_INET : AF_INET6, address.c_str(), &addr) == -1)
        throw Exception(Exception::ERROR_GET_ADDR_INFO, "Socket:: error getting socket info", getSocketErrorCode());
    
    setMulticastGroup(&addr, join);
}

Socket* Socket::accept() {
    if(type != TCP_SERVER)
        throw Exception(Exception::EXPECTED_TCP_SERVER, "Socket::accept: only TCP_SERVER can accept connections");
    
    struct sockaddr_storage remoteAddr;
    #ifdef OS_WIN32
    int addrSize = sizeof(remoteAddr);
    #else
    unsigned int addrSize = sizeof(remoteAddr);
    #endif
    
    int newHandler = ::accept(handle, reinterpret_cast<struct sockaddr *>(&remoteAddr), &addrSize);
    if(newHandler == -1) return NULL;
    
    return new Socket(newHandler, hostLocal, portLocal, &remoteAddr, ipVer);
}

void Socket::disconnect() {
    if(handle == -1) return;
    close(handle);
    handle = -1;
}

NL_NAMESPACE_END