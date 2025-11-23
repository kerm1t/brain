cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON) # eCAL/proto

# "../" --> output directory relative same to running in IDE
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

#if (MSVC)
  set(CMAKE_CXX_STANDARD 11)
#endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
endif()

project(brain)

if(MSVC)
  # CMake >= 3.15 will erroneously define PROTOBUF_USE_DLLS otherwise
  set (Protobuf_USE_STATIC_LIBS TRUE) 
endif()

set (SDL2_DIR ./lib/SDL2-2.26.4/cmake)
find_package(SDL2 REQUIRED)

set(glew "glew-2.1.0")
set(IMGUI_FOLDER imgui)

include_directories(. ./src src/math
./lib/${glew}/include
${SDL2_INCLUDE_DIRS}
${IMGUI_FOLDER}
${IMGUI_FOLDER}/src
${IMGUI_FOLDER}/debuggers
ImGuiColorTextEdit
)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/./lib/${glew}/lib/release/x64
  ${CMAKE_CURRENT_SOURCE_DIR}/./lib)

# CMAKE_PROJECT_NAME -> top level project
# PROJECT_NAME       -> most recent project
# add WIN32 to remove console window ->
add_executable (${PROJECT_NAME} #WIN32
${IMGUI_FOLDER}/imgui.cpp
${IMGUI_FOLDER}/imgui_demo.cpp
${IMGUI_FOLDER}/imgui_draw.cpp
${IMGUI_FOLDER}/imgui_tables.cpp
${IMGUI_FOLDER}/imgui_widgets.cpp
${IMGUI_FOLDER}/backends/imgui_impl_opengl3.cpp
${IMGUI_FOLDER}/backends/imgui_impl_sdl2.cpp
# 2do: merge these with libLT/(Fox/Point)Cloudhandler.*
#./hndl_pointcloud.cpp
#./hndl_pointcloud.h
ImGuiColorTextEdit/TextEditor.cpp
ImGuiColorTextEdit/TextEditor.h

src/main.cpp
src/LT_app.h
src/init.hpp
src/app_algowrp.hpp
src/app_ecal.hpp
src/user.hpp
src/draw.hpp
src/draw_primitives.hpp
src/shader/start.fs
src/shader/start.vs
# rbnn
src/cluster.hpp
src/kdtree.h
# dbscan
#src/dbscan.cpp
src/dbscan.hpp
src/nanoflann.hpp
src/math/linmath.h
src/database.hpp
)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
)

if (UNIX)
target_link_libraries(${PROJECT_NAME} GL SDL2 GLEW ${CMAKE_DL_LIBS})
endif()
