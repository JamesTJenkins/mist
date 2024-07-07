cmake_minimum_required(VERSION 3.5)
project(editor C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -DNDEBUG")
set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -Wall -fms-extensions -Wno-unused-function -municode")
#set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -Wall -Wextra -pedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-ignored-attributes -Wno-language-extension-token")

set(IMGUI ${CMAKE_SOURCE_DIR}/mist/vendor/imgui)

add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${IMGUI}/imgui.cpp
	${IMGUI}/imgui_draw.cpp
	${IMGUI}/imgui_tables.cpp
	${IMGUI}/imgui_widgets.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/mist/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC mist
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)