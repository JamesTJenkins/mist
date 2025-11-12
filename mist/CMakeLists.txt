cmake_minimum_required (VERSION 3.31.3)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("mist")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if(CLANG)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:-O0;-fstack-protector-all;-g3;-ggdb;-fPIC>"
        "$<$<CONFIG:RELEASE>:-O3;-g0;-fPIC;-DNDEBUG>"
        "-std=gnu++2a;-fmessage-length=0;-Wall;-fms-extensions;-Wno-unused-function"
    )
elseif(GCC)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:-O0;-fstack-protector-all;-g3;-ggdb;-fPIC>"
        "$<$<CONFIG:RELEASE>:-O3;-g0;-fPIC;-DNDEBUG>"
        "-std=gnu++2a;-fmessage-length=0;-Wall;-Wno-unused-function"
    )
elseif(MSVC)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:/Od;/GS;/ZI>"
        "$<$<CONFIG:RELEASE>:/O2;/DEBUG:NONE;-DNDEBUG>"
        "/W3;/wd4505;/wd4100"
    )
endif()

find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

add_subdirectory(vendor/spirv-cross)

add_library(mist STATIC ${SOURCES})

target_include_directories(mist
	PRIVATE "src/"
	PUBLIC "include/"
)

target_link_libraries(mist
    PRIVATE spirv-cross-core spirv-cross-glsl spirv-cross-hlsl spirv-cross-reflect
	PUBLIC glm::glm
	PRIVATE spdlog::spdlog
	PRIVATE Vulkan::Vulkan
	PUBLIC $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main> $<IF:$<TARGET_EXISTS:SDL3::SDL3>,SDL3::SDL3,SDL3::SDL3-static>
    PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
    PRIVATE EnTT::EnTT
)

# Stop warnings from spirv-cross
target_compile_options(spirv-cross PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-w>
    $<$<CXX_COMPILER_ID:Clang>:-w>
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
    PRIVATE $<$<PLATFORM_ID:Windows>:MIST_DLL>
)