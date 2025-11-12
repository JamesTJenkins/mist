cmake_minimum_required (VERSION 3.31.3)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("editor")

add_subdirectory(mist)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE SOURCES "editor/src/*.cpp")

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

find_package(SDL3 REQUIRED)

add_executable(editor ${SOURCES})

target_include_directories(editor
	PRIVATE "editor/src/"
)

target_link_libraries(editor
    PRIVATE mist
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
    PRIVATE $<$<PLATFORM_ID:Windows>:MIST_DLL>
)

set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/editor/assets")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/assets")

file(REMOVE_RECURSE ${ASSETS_DEST_DIR}) # Need to clear the assets directory before copying over otherwise asset changes wont show

add_custom_command(
    OUTPUT ${ASSETS_DEST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_SRC_DIR} ${ASSETS_DEST_DIR}
    COMMENT "Copying assets over to build dir"
    DEPENDS ${ASSETS_SRC_DIR}
)

add_custom_target(CopyAssets ALL
    DEPENDS ${ASSETS_DEST_DIR}
)

add_dependencies(editor
    CopyAssets
)