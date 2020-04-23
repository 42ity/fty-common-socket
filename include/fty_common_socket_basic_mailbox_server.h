/*  =========================================================================
    fty_common_socket_basic_mailbox_server - Basic synchronous mailbox server using unix socket

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

#ifndef FTY_COMMON_SOCKET_BASIC_MAILBOX_SERVER_H_INCLUDED
#define FTY_COMMON_SOCKET_BASIC_MAILBOX_SERVER_H_INCLUDED

#include "fty_common_sync_server.h"

#include <string>
#include <vector>
#include <functional>

namespace fty
{
   
    /**
     * \brief Handler for basic mailbox server using object
     *        implementing fty::SyncServer interface.
     * 
     * The server receive request using mailbox with the following protocol
     * 
     * \see fty_common_socket_sync_client.h
     */
    
    class SocketBasicServer
    {    
    public:
        explicit SocketBasicServer( fty::SyncServer & server,
                                    const std::string & path,
                                    size_t maxClient = 30);
        
        ~SocketBasicServer();
        
        void init();
        void run();
        void requestStop();
        
    private:
        //attributs
        fty::SyncServer & m_server;
        std::string m_path;
        size_t m_maxClient;
        int m_serverSocket;
        bool m_stopRequested = false;
        bool m_running = false;
        
        int m_pipe[2];
    };
    
} //namespace fty

//  @interface

//  Self test of this class
void
    fty_common_socket_basic_mailbox_server_test (bool verbose);

#endif
