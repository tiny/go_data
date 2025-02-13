# go_data
guarded observable data components

tools originally created for tax software developed in 1992, go-data is a small library that has supported the observable pattern via loose coupling over the decades

# how to add to your project
<code><pre>cmake_minimum_required(VERSION 3.10)
project(test_observables)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX OR APPLE)
    add_definitions(-DLINUX -DUNIX)
endif() 

include(FetchContent)

FetchContent_Declare(
    go_data
    GIT_REPOSITORY  https://github.com/tiny/go_data.git
    GIT_TAG         master
)

FetchContent_GetProperties(go_data)
if (NOT go_data_POPULATED)
    FetchContent_Populate(go_data)
    message(STATUS "libgo_data source dir: ${go_data_SOURCE_DIR}")
    add_subdirectory(${go_data_SOURCE_DIR} ${go_data_BINARY_DIR})
endif() 
FetchContent_MakeAvailable(go_data)

target_include_directories(libgo_data INTERFACE ${go_data_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} 
    src/main_observables.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE libgo_data)
</pre></code>

# how to utilize in your code
<code><pre>#include <gonumeric.h>
#include <cb_lambda.h>

go::GOInteger x, y, z ;

void func_1()
{
    y = x + 2 ;
} // :: func_1

void func_2()
{
    z = y * 3 ;
} // :: func_2

void init()
{
    x.valueCB().install( new go::LambdaPokeCB( func_1 )) ;
    y.valueCB().install( new go::LambdaPokeCB( func_2 )) ;
} // :: init

int main(int argc, char *argv[])
{
    init() ;
    x = 5 ;
    printf("x = %d, y = %d, z = %d\n", (int)x, (int)y, (int)z) ;

    return 0 ;
} // :: main
</pre></code>