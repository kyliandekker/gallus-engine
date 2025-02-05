cmake_minimum_required(VERSION 3.10)
project(Gallus)

add_subdirectory(tools)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_GENERATOR_PLATFORM x64)
set(RELEASE "Release")
set(DEBUG "Debug")
set(EDITOR_RELEASE "EditorRelease")
set(EDITOR_DEBUG "EditorDebug")
set(EDITOR_DEBUG_HYBRID "EditorDebugHybrid")

set(CMAKE_CONFIGURATION_TYPES "${RELEASE};${DEBUG};${EDITOR_RELEASE};${EDITOR_DEBUG};${EDITOR_DEBUG_HYBRID}" CACHE STRING "" FORCE)

# Define linker flags for different configurations
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG" CACHE STRING "Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF" CACHE STRING "Release linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_EDITORDEBUG "/DEBUG /EDIT" CACHE STRING "Editor Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_EDITORDEBUG2 "/DEBUG /EDIT" CACHE STRING "Editor Debug Hybrid linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_EDITORRELEASE "/OPT:REF /OPT:ICF /EDIT" CACHE STRING "Editor Release linker flags" FORCE)

set(CMAKE_BUILD_TYPE Debug)

# Set the Linker Debug flag for /DEBUG
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

# These are shared on ALL configurations. Rapidjson gives errors if we do not include this and TINYGLTF uses stb_image but we do not need it.
set(PREDEFINITIONS_SHARED "RAPIDJSON_NOMEMBERITERATORCLASS;TINYGLTF_NO_INCLUDE_STB_IMAGE;TINYGLTF_NO_STB_IMAGE;TINYGLTF_NO_STB_IMAGE_WRITE;_RESOURCE_ATLAS")

# These are specific configuration-based predefinitions.
set(PREDEFINITIONS_DEBUG "_DEBUG;" ${PREDEFINITIONS_SHARED})
set(PREDEFINITIONS_RELEASE "NDEBUG;" ${PREDEFINITIONS_SHARED})

# These are shared on ALL the Editor configurations.
set(PREDEFINITIONS_EDITOR_SHARED "_EDITOR")

# Editor inherits from their respective configuration and the shared predefinitions.
set(PREDEFINITIONS_EDITOR_DEBUG ${PREDEFINITIONS_EDITOR_SHARED} ${PREDEFINITIONS_DEBUG} "_RENDER_TEX")
set(PREDEFINITIONS_EDITOR_RELEASE ${PREDEFINITIONS_EDITOR_SHARED} ${PREDEFINITIONS_RELEASE} "_RENDER_TEX")

set(PREDEFINITIONS_EDITOR_DEBUG_HYBRID ${PREDEFINITIONS_EDITOR_SHARED} ${PREDEFINITIONS_DEBUG})

# Add the libraries
set(LIBRARIES 
    Shcore.lib
    dxgi.lib
    d3d12.lib
    d3dcompiler.lib
    dxguid.lib
    Shlwapi.lib
)

# Define IMGUI files first
set(IMGUI
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_dx12.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_helpers.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/implot.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/implot_items.cpp
)

# Define IMGUI files first
set(DX12
    ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp
)

# Define IMGUI files first
set(TINY_GLTF
    ${CMAKE_SOURCE_DIR}/external/tiny_gltf/tiny_gltf.cc
)

# Recursively get all header files in the include directory
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/engine/include/*.h)

# Recursively get all source files in the src directory
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/engine/src/*.cpp)

# Recursively get all header files in the include directory for the editor
file(GLOB_RECURSE HEADERS_EDITOR ${CMAKE_SOURCE_DIR}/editor/include/*.h)

# Recursively get all source files in the src directory for the editor
file(GLOB_RECURSE SOURCES_EDITOR ${CMAKE_SOURCE_DIR}/editor/src/*.cpp)

# Define the executable as a Win32 application
add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES} ${DX12} ${TINY_GLTF})

# Include the directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/editor/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external)

target_sources(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:${EDITOR_RELEASE}>:${IMGUI} ${HEADERS_EDITOR} ${SOURCES_EDITOR}>
    $<$<CONFIG:${EDITOR_DEBUG}>:${IMGUI} ${HEADERS_EDITOR} ${SOURCES_EDITOR}>
    $<$<CONFIG:${EDITOR_DEBUG_HYBRID}>:${IMGUI} ${HEADERS_EDITOR} ${SOURCES_EDITOR}>
)

# Set properties for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20 # Use C++ 20.
)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

# Define preprocessor definitions for different configurations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_RELEASE}>"
    "$<$<CONFIG:${EDITOR_DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${EDITOR_DEBUG_HYBRID}>:${PREDEFINITIONS_EDITOR_DEBUG_HYBRID}>"
    "$<$<CONFIG:${EDITOR_RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)

# Set output directories
# Make sure DLL and EXE targets go to the same directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Output directory for static lib (.LIB)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for shared lib (.DLL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for executables (.EXE)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:${DEBUG}>:/MTd>"
        "$<$<CONFIG:${RELEASE}>:/MT>"
        "$<$<CONFIG:${EDITOR_DEBUG}>:/MTd>"
        "$<$<CONFIG:${EDITOR_DEBUG_HYBRID}>:/MTd>"
        "$<$<CONFIG:${EDITOR_RELEASE}>:/MT>"
    )
endif()

if(MSVC)
    # Set Debug Information Format (/ZI) for Debug configuration
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_OPTIONS "/ZI")

    # Set the Linker Debug flag for /DEBUG (for Debug configurations)
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_OPTIONS "/DEBUG")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying assets from ${CMAKE_SOURCE_DIR}/assets to $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)