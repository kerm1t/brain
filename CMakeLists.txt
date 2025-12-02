cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON) # eCAL/proto

# "../" --> output directory relative same to running in IDE
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

#if (MSVC)
  set(CMAKE_CXX_STANDARD 17)
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

set(SDL2_PATH ./lib/SDL-Glew_pkg/SDL2-2.32.6/)
set(SDL2_DIR ./lib/SDL-Glew_pkg/SDL2-2.32.6/cmake) # for find_package
find_package(SDL2 REQUIRED)

set(GLEW_DIR ./lib/SDL-Glew_pkg/glew-2.1.0)
set(glew "glew-2.1.0")
set(IMGUI_FOLDER ./lib/imgui)
set(TEXTEDIT_FOLDER ./lib/ImGuiColorTextEdit)

include_directories(. ./src src/math src/database
${GLEW_DIR}/include
${SDL2_INCLUDE_DIRS}
${IMGUI_FOLDER}
${IMGUI_FOLDER}/src
${IMGUI_FOLDER}/debuggers
${TEXTEDIT_FOLDER}
)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/${GLEW_DIR}/lib/release/x64
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
${TEXTEDIT_FOLDER}/TextEditor.cpp
${TEXTEDIT_FOLDER}/TextEditor.h

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
src/math/linmath.h
src/database.hpp
src/database/sqlite3.c
)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
)

if (UNIX)
target_link_libraries(${PROJECT_NAME} GL SDL2 GLEW ${CMAKE_DL_LIBS})
endif()

if (MSVC)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_PATH}/lib/x64/SDL2.dll"
         $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()