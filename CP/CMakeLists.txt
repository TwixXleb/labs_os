set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
    set(ADDITIONAL_LIBS Threads::Threads rt)
else()
    set(ADDITIONAL_LIBS Threads::Threads)
endif()

find_package(Threads REQUIRED)

set(SERVER_SOURCES src/server.cpp)
set(CLIENT_SOURCES src/client.cpp)

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE Threads::Threads rt)

add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE Threads::Threads rt)
