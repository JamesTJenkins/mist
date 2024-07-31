cmake_minimum_required(VERSION 3.5)
project(mist LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fstack-protector-all -g3 -ggdb -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -fPIC -DNDEBUG")
set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -Wall -fms-extensions -Wno-unused-function")
# For more aggressive compilation warning/error checking
#set(CMAKE_CXX_FLAGS "-std=gnu++2a -fmessage-length=0 -Wall -Wextra -pedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-ignored-attributes -Wno-language-extension-token")

find_package(glm CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)

set(IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

#find_package(SPIRV-Tools CONFIG REQUIRED)
# The following libraries are static and depend on SPIRV-Tools-static.
#find_package(SPIRV-Tools-link CONFIG REQUIRED)
#find_package(SPIRV-Tools-lint CONFIG REQUIRED)
#find_package(SPIRV-Tools-opt CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)

set(VULKAN "C:/VulkanSDK/1.3.290.0")    #$ENV{VULKAN_SDK} for some reason is deleting slashes
#find_package(spirv_cross_core REQUIRED)
#find_package(spirv_cross_glsl REQUIRED)

add_library(mist SHARED 
    ${SOURCES}
    ${IMGUI}/imgui.cpp
	${IMGUI}/imgui_draw.cpp
	${IMGUI}/imgui_tables.cpp
	${IMGUI}/imgui_widgets.cpp
	${IMGUI}/backends/imgui_impl_sdl2.cpp
	${IMGUI}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PUBLIC ${IMGUI}/
    PUBLIC ${IMGUI}/backends/
    PRIVATE ${VULKAN}/Include/spirv_cross/
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE glm::glm
    PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    PRIVATE Vulkan::Vulkan
    PRIVATE spdlog::spdlog_header_only
    #PRIVATE ${VULKAN}/Lib/spirv-cross-core.lib
    PRIVATE ${VULKAN}/Lib/spirv-cross-glsl.lib
    #PRIVATE spirv-cross-glsl
    #PRIVATE SPIRV-Tools
    #PRIVATE SPIRV-Tools-opt
    #PRIVATE SPIRV-Tools-link
    #PRIVATE SPIRV-Tools-lint
    PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
    PRIVATE $<$<PLATFORM_ID:Windows>:MIST_DLL>
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)