/*  =========================================================================
    fty-common-socket - Provides common unix socket tools for agents

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
#pragma once

//  Opaque class structures to allow forward references
//  These classes are stable or legacy and built in all releases
typedef struct _fty_common_socket_sync_client_t fty_common_socket_sync_client_t;
#define FTY_COMMON_SOCKET_SYNC_CLIENT_T_DEFINED
typedef struct _fty_common_socket_basic_mailbox_server_t fty_common_socket_basic_mailbox_server_t;
#define FTY_COMMON_SOCKET_BASIC_MAILBOX_SERVER_T_DEFINED


//  Public classes, each with its own header file
#include "fty_common_socket_basic_mailbox_server.h"
#include "fty_common_socket_sync_client.h"
