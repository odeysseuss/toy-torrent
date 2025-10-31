cmake_minimum_required(VERSION 3.18)
project(torrent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CFLAGS
    -Wall
    -Wextra
    -pedantic
)

set(INCDIR
    include
)

set(TORRENTSRC
    src/main.cpp
    src/tcp.cpp
    src/bencode.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${TORRENTSRC})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INCDIR})
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC ${CFLAGS})

# For nvim tmux runner
add_custom_target(run
    COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    DEPENDS ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
