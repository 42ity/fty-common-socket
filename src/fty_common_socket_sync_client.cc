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

/*
@header
    fty_common_socket_sync_client - Simple client for synchronous request using unix socket
@discuss
@end
*/

#include <stdexcept>

#include "fty_common_socket_sync_client.h"
#include "fty_common_socket_helpers.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace fty
{
    SocketSyncClient::SocketSyncClient(const std::string & path)
    :   m_path(path)
    {
    }
    
       
    std::vector<std::string> SocketSyncClient::syncRequestWithReply(const std::vector<std::string> & payload)
    {
        int data_socket = -1;
        
        try
        {
            struct sockaddr_un addr;
            int ret;

            /* Create local socket. */
            data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
            if (data_socket == -1)
            {
                throw std::runtime_error("Impossible to connect to server");
            }

            memset(&addr, 0, sizeof(struct sockaddr_un));

            /* Connect socket to socket address */

            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, m_path.c_str(), sizeof(addr.sun_path) - 1);

            ret = connect (data_socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
            if (ret == -1)
            {
                throw std::runtime_error("Impossible to connect to server");
            }

            sendFrames(data_socket, payload);

            std::vector<std::string> data = recvFrames(data_socket);
            
            close(data_socket);

            return data;
        }
        catch(std::exception &)
        {
            if(data_socket != -1)
            {
                close(data_socket);
            }
            
            throw;
        }
        
     }
        
} //namespace fty

//  --------------------------------------------------------------------------
//  Self test of this class
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
fty_common_socket_sync_client_test (bool verbose)
{
    printf (" * fty_common_socket_sync_client: ");

    //  @selftest
    //  Simple create/destroy test
    fty::SocketSyncClient(std::string(SELFTEST_DIR_RW"/test.socket"));
    //  @end
    printf ("OK\n");
}
