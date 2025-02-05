cmake_minimum_required(VERSION 3.10)
project(FontEmbedder)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_GENERATOR_PLATFORM x64)
set(RELEASE "Release")
set(DEBUG "Debug")

set(CMAKE_CONFIGURATION_TYPES "${RELEASE};${DEBUG}" CACHE STRING "" FORCE)

# Define linker flags for different configurations
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG" CACHE STRING "Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF" CACHE STRING "Release linker flags" FORCE)

set(CMAKE_BUILD_TYPE Debug)

# Set the Linker Debug flag for /DEBUG
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

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
	
	# all win32 stuff
    ${CMAKE_SOURCE_DIR}/engine/src/graphics/win32/Window.cpp
	
	# logger
    ${CMAKE_SOURCE_DIR}/engine/src/core/logger/Logger.cpp
	
	# File Utils
    ${CMAKE_SOURCE_DIR}/engine/src/core/FileUtils.cpp
	
	# systems
    ${CMAKE_SOURCE_DIR}/engine/src/core/System.cpp
    ${CMAKE_SOURCE_DIR}/engine/src/core/FileUtils.cpp # TODO REMOVE
    ${CMAKE_SOURCE_DIR}/engine/src/core/ReserveDataStream.cpp # TODO REMOVE
    ${CMAKE_SOURCE_DIR}/engine/src/core/DataStream.cpp # TODO REMOVE
    ${CMAKE_SOURCE_DIR}/engine/src/core/Data.cpp # TODO REMOVE
	
	# all DX12 stuff
    ${CMAKE_SOURCE_DIR}/engine/src/graphics/dx12/CommandQueue.cpp
    ${CMAKE_SOURCE_DIR}/engine/src/graphics/dx12/CommandList.cpp
    ${CMAKE_SOURCE_DIR}/engine/src/graphics/dx12/DX12System.cpp
    ${CMAKE_SOURCE_DIR}/engine/src/graphics/dx12/HeapAllocation.cpp
)

# Recursively get all header files in the include directory
file(GLOB_RECURSE HEADERS_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Recursively get all source files in the src directory
file(GLOB_RECURSE SOURCES_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Include the directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/engine/include)
include_directories(${CMAKE_SOURCE_DIR}/editor/include)

# Define the executable as a Win32 application
add_executable(${PROJECT_NAME} WIN32 ${HEADERS_TOOLS} ${SOURCES_TOOLS} ${IMGUI})

# Set properties for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20 # Use C++ 20.
)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

# Set output directories
# Make sure DLL and EXE targets go to the same directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Output directory for static lib (.LIB)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for shared lib (.DLL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for executables (.EXE)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:${DEBUG}>:/MTd>"
        "$<$<CONFIG:${RELEASE}>:/MT>"
    )
endif()

if(MSVC)
    # Set Debug Information Format (/ZI) for Debug configuration
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_OPTIONS "/ZI")

    # Set the Linker Debug flag for /DEBUG (for Debug configurations)
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_OPTIONS "/DEBUG")
endif()