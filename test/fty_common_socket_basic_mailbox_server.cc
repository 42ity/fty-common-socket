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

#include "fty_common_socket_basic_mailbox_server.h"
#include "fty_common_socket_sync_client.h"
#include "fty_common_unit_tests.h"
#include <catch2/catch.hpp>
#include <thread>

TEST_CASE("Basic mailbox server")
{
    printf(" * fty_common_socket_basic_mailbox_server: ");

    // normal case
    {
        fty::EchoServer server;

        fty::SocketBasicServer agent(server, "test.socket");


        std::thread serverThread(&fty::SocketBasicServer::run, &agent);

        // create a client
        {
            fty::SocketSyncClient syncClient("test.socket");

            fty::Payload expectedPayload = {"This", "is", "a", "test"};

            fty::Payload receivedPayload = syncClient.syncRequestWithReply(expectedPayload);

            CHECK(expectedPayload == receivedPayload);
        }

        agent.requestStop();

        serverThread.join();
    }

    // check destroy
    {
        fty::EchoServer server;

        fty::SocketBasicServer agent(server, "test.socket");
    }

    // check destroy
    {
        fty::EchoServer server;

        fty::SocketBasicServer agent(server, "test.socket");

        agent.requestStop();
    }
}
