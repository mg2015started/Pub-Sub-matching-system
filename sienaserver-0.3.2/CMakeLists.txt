cmake_minimum_required(VERSION 3.8)
project(sienaserver_0_3_2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        build/sienaserverconf.h
        include/EventServer.h
        include/PThreads.h
        include/PThreads.icc
        include/set_util.h
        tests/client.cc
        tests/compare.cc
        tests/covers.cc
        tests/pingsiena.cc
        tests/sender.cc
        EventServer.cc
        EventServer.h
        EventServer_old.cc
        main.cc
        PThreads.cc
        PThreads.h
        PThreads.icc
        set_util.h)

add_executable(sienaserver_0_3_2 ${SOURCE_FILES})