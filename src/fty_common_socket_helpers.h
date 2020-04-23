/*  =========================================================================
    fty_common_socket_helpers - Helper functions for communication

    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

#ifndef FTY_COMMON_SOCKET_HELPERS_H_INCLUDED
#define FTY_COMMON_SOCKET_HELPERS_H_INCLUDED

#include <string>
#include <vector>

namespace fty
{
    using Payload = std::vector<std::string>;
        
    //functions
    Payload recvFrames(int socket);
    void sendFrames(int socket, const Payload & payload);
    
} //namespace fty

#endif
