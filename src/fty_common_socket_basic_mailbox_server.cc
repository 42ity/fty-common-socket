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

/*
@header
    fty_common_socket_basic_mailbox_server - Basic synchronous mailbox server using unix socket
@discuss
@end
*/
#include "fty_common_socket_basic_mailbox_server.h"
#include "fty_common_socket_helpers.h"
#include <errno.h>
#include <iostream>
#include <pwd.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

//  Structure of our class
namespace fty {

SocketBasicServer::SocketBasicServer(fty::SyncServer& server, const std::string& path, size_t maxClient)
    : m_server(server)
    , m_path(path)
    , m_maxClient(maxClient)
{
    m_serverSocket = -1;
    m_pipe[0]      = -1;
    m_pipe[1]      = -1;

    /*
     * In case the program exited inadvertently on the last run,
     * remove the socket.
     */

    unlink(m_path.c_str());

    // Create unix socket.

    m_serverSocket = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC);
    if (m_serverSocket == -1) {
        throw std::runtime_error("Impossible to create unix socket");
    }

    struct sockaddr_un name;
    memset(&name, 0, sizeof(name));

    // Bind socket to socket name.

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, m_path.c_str(), sizeof(name.sun_path) - 1);

    int ret = bind(m_serverSocket, reinterpret_cast<const struct sockaddr*>(&name), sizeof(struct sockaddr_un));
    if (ret == -1) {
        throw std::runtime_error("Impossible to bind the Unix socket");
    }

    // Change read/write privileges of the socket
    ret = chmod(m_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    if (ret == -1) {
        throw std::runtime_error(
            "Impossible to change the rights of the Unix socket " + m_path + ": " + std::string(strerror(errno)));
    }

    // Prepare for accepting connections
    ret = listen(m_serverSocket, int(m_maxClient));
    if (ret == -1) {
        throw std::runtime_error("Impossible to listen on the Unix socket");
    }

    if (pipe(m_pipe) < 0) {
        throw std::runtime_error("Impossible to create the pipe");
    }
}

SocketBasicServer::~SocketBasicServer()
{
    if (m_serverSocket != -1) { close(m_serverSocket); m_serverSocket = -1; }
    if (m_pipe[0] != -1) { close(m_pipe[0]); m_pipe[0] = -1; }
    if (m_pipe[1] != -1) { close(m_pipe[1]); m_pipe[1] = -1; }

    /* Unlink the socket. */
    unlink(m_path.c_str());
}

void SocketBasicServer::run()
{
    m_running = true;

    // Clear the reference set of socket
    fd_set socketsSet;
    FD_ZERO(&socketsSet);

    // Add the server socket
    FD_SET(m_serverSocket, &socketsSet);
    int lastSocket = m_serverSocket;

    // Add the pipe
    FD_SET(m_pipe[0], &socketsSet);
    if (m_pipe[0] > lastSocket) {
        // Keep track of the maximum
        lastSocket = m_pipe[0];
    }

    // infinite loop for handling connection
    while (1) {
        fd_set tmpSockets = socketsSet;

        // Detect activity on the sockets
        if (select(lastSocket + 1, &tmpSockets, nullptr, nullptr, nullptr) == -1) {
            if (m_stopRequested) {
                break;
            }
            else { // error case
                continue;
            }
        }

        if (m_stopRequested) {
            break;
        }

        // Run through the existing connections looking for data to be read
        for (int socket = 0; socket <= lastSocket; socket++) {

            if (!FD_ISSET(socket, &tmpSockets)) {
                // Nothing change on the socket
                continue;
            }

            if (socket == m_serverSocket) {
                // A client is asking a new connection
                struct sockaddr_storage clientaddr;
                socklen_t addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));

                int newSocket = accept(m_serverSocket, reinterpret_cast<struct sockaddr*>(&clientaddr), &addrlen);
                if (newSocket != -1) {
                    // save the socket
                    FD_SET(newSocket, &socketsSet);

                    if (newSocket > lastSocket) {
                        // Keep track of the maximum
                        lastSocket = newSocket;
                    }

                } else {
                    // PRINT_DEBUG("Server accept() error");
                }
            } else if (socket == m_pipe[0]) {
                char c;
                if (read(m_pipe[0], &c, 1) != 1) {
                    // error
                }
            } else {
                try {
                    // We received request

                    // get credential info
                    struct ucred cred;
                    socklen_t lenCredStruct = sizeof(cred);

                    if (getsockopt(socket, SOL_SOCKET, SO_PEERCRED, &cred, &lenCredStruct) == -1) {
                        throw std::runtime_error("Impossible to get sender");
                    }

                    struct passwd* pws = getpwuid(cred.uid);

                    std::string sender(pws->pw_name);

                    // printf("=== New connection from %s with PID %i, with UID %i and GID %i\n",pws->pw_name,cred.pid,
                    // cred.uid, cred.gid);

                    // Get frames
                    std::vector<std::string> payload = recvFrames(socket);

                    // Execute the request
                    Payload results = m_server.handleRequest(sender, payload);

                    // send the result if it's not empty
                    if (!results.empty()) {
                        sendFrames(socket, results);
                    }

                    // check if we need to leave
                    if (m_stopRequested) {
                        break;
                    }

                } catch (...) {
                    if (m_stopRequested) {
                        break;
                    }

                    // close the connection in case of error
                    close(socket);

                    // Remove from reference set
                    FD_CLR(socket, &socketsSet);

                    if (socket == lastSocket) {
                        lastSocket--;
                    }
                }
            }
        }
    }

    // End of the handler.Close the sockets except the server one.
    for (int socket = lastSocket; socket >= 0; socket--) {
        fd_set tmpSockets = socketsSet;

        if (!FD_ISSET(socket, &tmpSockets)) {
            // This socket is not in the set
            continue;
        }

        // Don't close the server socket or pipe
        if ((socket != m_serverSocket) && (socket != m_pipe[0])) {
            close(socket);

            // Remove from reference set
            FD_CLR(socket, &socketsSet);

            if (socket == lastSocket) {
                lastSocket--;
            }
        }
    }

    m_running       = false;
    m_stopRequested = false;
}

void SocketBasicServer::requestStop()
{
    if (m_running) {
        m_stopRequested = true;

        if (write(m_pipe[1], "s", 1) != -1) {
            // error
        }
    }
}

} // namespace fty
