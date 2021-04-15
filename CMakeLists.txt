cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

########################################################################################################################

project(fty_common_socket
    VERSION 1.0.0
    DESCRIPTION "Provides common unix socket tools for agents"
)

########################################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
########################################################################################################################

etn_target(shared ${PROJECT_NAME}
    PUBLIC_INCLUDE_DIR include
    PUBLIC
        fty_common_socket_basic_mailbox_server.h
        fty_common_socket.h
        fty_common_socket_sync_client.h
    SOURCES
        src/fty_common_socket_basic_mailbox_server.cc
        src/fty_common_socket_helpers.cc
        src/fty_common_socket_helpers.h
        src/fty_common_socket_sync_client.cc
    USES
        cxxtools
        fty_common
        fty_common_logging
)

########################################################################################################################

etn_test_target(${PROJECT_NAME}
    SOURCES
        test/fty_common_socket_basic_mailbox_server.cc
        test/fty_common_socket_sync_client.cc
        test/main.cpp
    SUBDIR
        test
)

########################################################################################################################

