# Install script for directory: /Users/prompt/Downloads/libssh-0.11.0/include/libssh

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libssh" TYPE FILE FILES
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/callbacks.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/libssh.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/ssh2.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/legacy.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/libsshpp.hpp"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/sftp.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/server.h"
    "/Users/prompt/Downloads/libssh-0.11.0/include/libssh/sftpserver.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libssh" TYPE FILE FILES "/Users/prompt/Downloads/libssh-0.11.0/build/include/libssh/libssh_version.h")
endif()

