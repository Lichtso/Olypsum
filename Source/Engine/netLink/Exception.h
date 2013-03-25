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

#ifndef __NL_EXCEPTION
#define __NL_EXCEPTION

#include "Core.h"

NL_NAMESPACE

class Exception {
    public:
    enum CODE {
        BAD_PROTOCOL,
        BAD_IP_VER,
        ERROR_INIT,
        ERROR_SET_ADDR_INFO,
        ERROR_GET_ADDR_INFO,
        ERROR_SET_SOCK_OPT,
        ERROR_CAN_NOT_LISTEN,
        ERROR_CONNECT_SOCKET,
        ERROR_SEND,
        ERROR_READ,
        ERROR_IOCTL,
        ERROR_SELECT,
        ERROR_ALLOC,
        EXPECTED_TCP_CLIENT,
        EXPECTED_TCP_SERVER,
        EXPECTED_UDP_PEER,
        EXPECTED_NON_SERVER,
        EXPECTED_HOST_TO
    } code;
    std::string msg;
    int nativeErrorCode;
    Exception(CODE _code, const std::string& _msg, int _nativeErrorCode = 0): code(_code), msg(_msg), nativeErrorCode(_nativeErrorCode) {}
};

NL_NAMESPACE_END

#endif
