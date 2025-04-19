cmake_minimum_required(VERSION 3.20.0)
project(minesweeper VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)

include_directories(./include)

# 指定 SFML 版本
find_package(SFML 3.0.0 REQUIRED COMPONENTS Graphics Window System Network)

set(LIB_LINKS SFML::Graphics SFML::Window SFML::System SFML::Network)

file(GLOB_RECURSE SRC_FILES ./src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_LINKS})

install(TARGETS ${PROJECT_NAME})
