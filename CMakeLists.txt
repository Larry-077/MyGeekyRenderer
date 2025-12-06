cmake_minimum_required(VERSION 3.10)
project(MyGeekyRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable ImGui
option(USE_IMGUI "Build with ImGui GUI support" ON)

# Try to find Eigen3
find_package(Eigen3 3.3 QUIET NO_MODULE)

if(NOT Eigen3_FOUND)
    # If not found, try common paths
    find_path(EIGEN3_INCLUDE_DIR Eigen/Core
        PATHS
        /usr/local/include/eigen3
        /usr/include/eigen3
        /opt/homebrew/include/eigen3
        /usr/local/Cellar/eigen/*/include/eigen3
    )
    if(EIGEN3_INCLUDE_DIR)
        message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "Eigen3 not found. Install with: brew install eigen")
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/AABBTree.cpp
    src/AABBTree_ray_intersect.cpp
    src/insert_box_into_box.cpp
    src/insert_triangle_into_box.cpp
    src/per_vertex_normals.cpp
    src/ray_intersect_box.cpp
    src/ray_intersect_triangle.cpp
    src/read_obj.cpp
    src/vertex_triangle_adjacency.cpp
    src/viewing_ray.cpp
    src/triangle_area_normal.cpp
    src/ASCIIRenderer.cpp
)

# Executable
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

# Link Eigen3 (if found as package)
if(TARGET Eigen3::Eigen)
    target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
endif()

# ImGui support
if(USE_IMGUI)
    message(STATUS "Building with ImGui support")
    
    # Find OpenGL
    find_package(OpenGL REQUIRED)
    
    # Find GLFW
    find_package(glfw3 QUIET)
    if(NOT glfw3_FOUND)
        # Try pkg-config
        find_package(PkgConfig)
        if(PkgConfig_FOUND)
            pkg_check_modules(GLFW REQUIRED glfw3)
        else()
            message(FATAL_ERROR "GLFW not found. Install with: brew install glfw")
        endif()
    endif()
    
    # Add ImGui sources (assuming they're in external/imgui)
    set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
    if(EXISTS ${IMGUI_DIR})
        target_sources(${PROJECT_NAME} PRIVATE
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
            ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
            ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        )
        target_include_directories(${PROJECT_NAME} PRIVATE 
            ${IMGUI_DIR} 
            ${IMGUI_DIR}/backends
        )
    else()
        message(WARNING "ImGui directory not found at ${IMGUI_DIR}")
        message(WARNING "Download from: https://github.com/ocornut/imgui")
    endif()
    
    # Link libraries
    target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)
    
    # Define USE_IMGUI
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_IMGUI)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
