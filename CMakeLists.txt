# ONLY SUPPORTS WINDOWS

cmake_minimum_required(VERSION 3.5)
project(mist C CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fstack-protector-all -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -g0 -DNDEBUG")

if (WIN32)
    set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -D_WIN32 -Wall")
    # For more aggressive compilation warning/error checking
    #set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -D_WIN32 -Wall -Wextra -pedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-ignored-attributes -Wno-language-extension-token")
endif()

find_package(glm CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
# Might turn engine into static lib to then use in seperate editor and game builds
# will keep as exe for now while testing
# add_library(mist STATIC ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    PRIVATE Vulkan::Vulkan
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)