# ONLY SUPPORTS WINDOWS AND CLANG CURRENTLY

cmake_minimum_required(VERSION 3.5)

project(mist C CXX)
set(CMAKE_BUILD_TYPE Release)

add_subdirectory(mist/vendor/SDL2)
add_subdirectory(mist/vendor/glm)

file(GLOB SOURCES "mist/src/*.cpp" "mist/src/*.hpp" "mist/src/*.h")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fstack-protector-all -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -g0 -DNDEBUG")

if (WINDOWS)
set(CMAKE_CXX_FLAGS "-pthread -std=gnu++2a -fmessage-length=0 -D_WIN32 -Wall -Wextra -pedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-ignored-attributes -Wno-language-extension-token")
endif()

add_executable(mist ${SOURCES})
# Might turn engine into static lib to then use in seperate editor and game builds
# will keep as exe for now while testing
# add_library(mist STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC mist/src 
    PUBLIC mist/vendor/SDL2 
    PUBLIC mist/vendor/glm
)

target_link_directories(${PROJECT_NAME} PRIVATE mist/src)
target_link_libraries(${PROJECT_NAME} mist/src)