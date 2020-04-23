/*  =========================================================================
    fty_common_socket_sync_client - Simple client for synchronous request using unix socket

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

#ifndef FTY_COMMON_SOCKET_SYNC_CLIENT_H_INCLUDED
#define FTY_COMMON_SOCKET_SYNC_CLIENT_H_INCLUDED

#include "fty_common_client.h"

#include <string>
#include <vector>
#include <functional>


namespace fty
{
    // This class is thread safe.
    
    class SocketSyncClient
        : public SyncClient //Implement interface for synchronous client
    {    
    public:
        explicit SocketSyncClient(const std::string & path);
        
        //methods
        std::vector<std::string> syncRequestWithReply(const std::vector<std::string> & payload) override;
        
    private:
        //attributs
        std::string m_path;        
    };
    
} //namespace fty

//  @interface
//  Self test of this class
void
    fty_common_socket_sync_client_test (bool verbose);
//  @end

#endif
