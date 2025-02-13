cmake_minimum_required(VERSION 3.10)
project(libgo_data)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX OR APPLE)
    add_definitions(-DLINUX -DUNIX)
endif() 

# Include directories
include_directories(include)

# Source files
set(SOURCES_godata
    src/arglist.cpp
    src/bytebuffer.cpp
    src/calccrc.cpp
    src/crc32tab.cpp
    src/dar.cpp
    src/db_blob.cpp
    src/dims.cpp
    src/fastgate.cpp
    src/goeventtree.cpp
    src/golinestats.cpp
    src/gonumeric.cpp
    src/goobject.cpp
    src/goprioritytable.cpp
    src/gosampler.cpp
    src/gostatemachine.cpp
    src/gostatenode.cpp
    src/gostopwatch.cpp
    src/gostring.cpp
    src/gotimer.cpp
    src/gotools_core_version.cpp
    #src/inifield.cpp
    #src/inifiledb.cpp
    #src/inisection.cpp
    src/msgcb.cpp
    src/mutexgate.cpp
    src/now.cpp
    src/pbvparameter.cpp
    src/pointintime.cpp
    src/ptintm.cpp
    src/report.cpp
    src/report_monitor.cpp
    src/report_monitor_file.cpp
    src/report_msg.cpp
    src/si_sleep.cpp
    src/stopwatch.cpp
    src/stricmp.cpp 
    src/thread_core.cpp
    src/threads.cpp
    src/timedevent.cpp
)

# Create a library
add_library(${PROJECT_NAME} STATIC ${SOURCES_godata})

target_compile_definitions(${PROJECT_NAME} PUBLIC LINUX)
target_compile_definitions(${PROJECT_NAME} PUBLIC UNIX)
target_include_directories(${PROJECT_NAME} PUBLIC include)

