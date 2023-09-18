# ONLY SUPPORTS WINDOWS AND CLANG CURRENTLY

cmake_minimum_required(VERSION 3.5)

project(mist C CXX)

set(BUILD_SHARED_LIBS ON)

add_subdirectory(mist/vendor/SDL2)
add_subdirectory(mist/vendor/glm)
add_subdirectory(mist/vendor/Vulkan-Headers)

file(GLOB_RECURSE SOURCES "mist/src/*.cpp")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fstack-protector-all -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -g0 -DNDEBUG")

if (WINDOWS)
    set(CMAKE_CXX_FLAGS "-pthread -std=gnu++2a -fmessage-length=0 -D_WIN32 -Wall -Wextra -pedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-ignored-attributes -Wno-language-extension-token")
endif()

add_executable(mist ${SOURCES})
# Might turn engine into static lib to then use in seperate editor and game builds
# will keep as exe for now while testing
# add_library(mist STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    SDL2::SDL2
    glm::glm
    Vulkan::Headers
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)