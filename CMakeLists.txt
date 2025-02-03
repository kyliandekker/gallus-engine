cmake_minimum_required(VERSION 3.10)
project(Gallus)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_GENERATOR_PLATFORM x64)
set(RELEASE "Release")
set(DEBUG "Debug")
set(EDITOR_RELEASE "EditorRelease")
set(EDITOR_DEBUG "EditorDebug")

set(CMAKE_CONFIGURATION_TYPES "${RELEASE};${DEBUG};${EDITOR_RELEASE};${EDITOR_DEBUG}" CACHE STRING "" FORCE)

# Define linker flags for different configurations
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG" CACHE STRING "Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF" CACHE STRING "Release linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_EDITORDEBUG "/DEBUG /EDIT" CACHE STRING "Editor Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_EDITORRELEASE "/OPT:REF /OPT:ICF /EDIT" CACHE STRING "Editor Release linker flags" FORCE)

set(CMAKE_BUILD_TYPE Debug)

# Set the Linker Debug flag for /DEBUG
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

# These are shared on ALL configurations. Rapidjson gives errors if we do not include this and TINYGLTF uses stb_image but we do not need it.
set(PREDEFINITIONS_SHARED "RAPIDJSON_NOMEMBERITERATORCLASS;TINYGLTF_NO_INCLUDE_STB_IMAGE;TINYGLTF_NO_STB_IMAGE;TINYGLTF_NO_STB_IMAGE_WRITE")

# These are specific configuration-based predefinitions.
set(PREDEFINITIONS_DEBUG "_DEBUG;" ${PREDEFINITIONS_SHARED})
set(PREDEFINITIONS_RELEASE "NDEBUG;" ${PREDEFINITIONS_SHARED})

# These are shared on BOTH the Editor configurations.
set(PREDEFINITIONS_EDITOR_SHARED "_EDITOR")

# Editor inherits from their respective configuration and the shared predefinitions.
set(PREDEFINITIONS_EDITOR_DEBUG ${PREDEFINITIONS_EDITOR_SHARED} ${PREDEFINITIONS_DEBUG})
set(PREDEFINITIONS_EDITOR_RELEASE ${PREDEFINITIONS_EDITOR_SHARED} ${PREDEFINITIONS_RELEASE})

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

# Create a source group for IMGUI, name it with () so that it shows up at the top instead of in between the important files.
source_group("(IMGUI)" FILES ${IMGUI})

# Define IMGUI files first
set(DX12
    ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp
)

# Create a source group for IMGUI, name it with () so that it shows up at the top instead of in between the important files.
source_group("(DX12)" FILES ${DX12})

# Define IMGUI files first
set(TINY_GLTF
    ${CMAKE_SOURCE_DIR}/external/tiny_gltf/tiny_gltf.cc
)

# Create a source group for IMGUI, name it with () so that it shows up at the top instead of in between the important files.
source_group("(TINY_GLTF)" FILES ${TINY_GLTF})

# Recursively get all header files in the include directory
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

# Recursively get all source files in the src directory
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Create source group for headers
foreach(header IN LISTS HEADERS)
    get_filename_component(header_dir ${header} DIRECTORY)
    
    # Remove the ${CMAKE_SOURCE_DIR}/include part to get the relative directory structure
    string(REPLACE "${CMAKE_SOURCE_DIR}/include" "" header_dir ${header_dir})
    
    # Check if the header_dir is not empty before applying the REPLACE
    if(NOT header_dir STREQUAL "")
        # Replace forward slashes with backslashes for Visual Studio to recognize as a filter
        string(REPLACE "/" "\\" header_dir ${header_dir})
        # Create a source group for the folder structure inside "Headers"
        source_group("Headers${header_dir}" FILES ${header})
    else()
        # If the header_dir is empty, just put it in the "Headers" root
        source_group("Headers" FILES ${header})
    endif()
endforeach()

# Create source group for sources
foreach(source IN LISTS SOURCES)
    get_filename_component(source_dir ${source} DIRECTORY)
    
    # Remove the ${CMAKE_SOURCE_DIR}/src part to get the relative directory structure
    string(REPLACE "${CMAKE_SOURCE_DIR}/src" "" source_dir ${source_dir})
    
    # Check if the source_dir is not empty before applying the REPLACE
    if(NOT source_dir STREQUAL "")
        # Replace forward slashes with backslashes for Visual Studio to recognize as a filter
        string(REPLACE "/" "\\" source_dir ${source_dir})
        # Create a source group for the folder structure inside "Sources"
        source_group("Sources${source_dir}" FILES ${source})
    else()
        # If the source_dir is empty, just put it in the "Sources" root
        source_group("Sources" FILES ${source})
    endif()
endforeach()

# Include the directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Define the executable as a Win32 application
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${IMGUI} ${DX12} ${TINY_GLTF})

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