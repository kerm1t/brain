/*
  LT-Stack
  (C) 2025
  author: w.schulz

  Generic object detection
  main.cpp

  https://google.github.io/styleguide/cppguide.html
*/

#include <iostream> 
#define GLEW_STATIC                     // link GLEW static
#include <GL/glew.h>
//#define SDL_MAIN_HANDLED              // SDL's default main function

#include "imgui.h"
#ifdef _WIN32
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#else
#include "src/imgui_impl_sdl2.h"
#include "src/imgui_impl_opengl3.h"
#endif
#include "ImGuiColorTextEdit/TextEditor.h"

#ifdef _WIN32
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#include <SDL2/SDL.h>                   // Linux / MacOS
#endif

#include "LT_app.h"
#include "draw.hpp"

#include <chrono>
float algo_runtime_ms;

#include <mutex>
std::recursive_mutex m_SubscriberMutex; // ecal callback thread
bool b_newframe = false;

#include "init.hpp"
#include "user.hpp"
#include "database.hpp"

/*
  ------------------------------------------------
  main()
  - init eCal (or ...) -> register input callbacks
  - init GFX                    // optional
  - init IO/ user input         // optional
  - init sim                    // optional
  - loop
    - render                    // optional
    - user I/O and insta config
    - if data || user-Input
      - run (Mutex)
      - publish
  - exit ... all of the above
  ------------------------------------------------
*/
int main(int argc, char** argv)
{
  user::init_Cfg();

// Init GFX
  SDL_Window* window = init_SDL();

  SDL_GLContext glContext = SDL_GL_CreateContext(window);

  init_GlEW(); // expects context exists

  db::db_open("..//brain.db3");
  std::string note = db::sql_string("SELECT note from notes WHERE rowid=5;");
  user::editor.SetText(note);

  db::sql_rows("SELECT rowid, topic from notes;");
  user::list_items.reserve(db::rows_val.size());
  for (auto& s : db::rows_val)
    user::list_items.push_back(s.c_str());

//  ImGuiWindowFlags_NoScrollWithMouse();
  // openGL: init GPU structures
  gpu_create_shaders();
  gpu_create_buffers();
  gpu_create_variables();

  gpu_cube();

  axes_create();
  axes_gpu_push_buffers();
  
  grid_create();
  grid_gpu_push_buffers();

  init_GL();

  ImGuiIO& io = init_Imgui(window, glContext); // setup Dear ImGui context
// Init GFX

  user::editor.SetPalette(TextEditor::GetLightPalette());
  ImGui::StyleColorsLight();

  // Loop
  bool close = false;
  do
  {
    render(window);

    user::Imgui_draw(window, io);
    user::Imgui_events();

    bool b_cfg_changed = user::insta_config();

    if (b_newframe || b_cfg_changed) {
      auto t1 = std::chrono::high_resolution_clock::now();
// do something
      auto t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> ms_double = t2 - t1;
      std::cout << "run_frs " << ms_double.count() << "ms\n";
      algo_runtime_ms = ms_double.count();

      b_newframe = false;
    }

    SDL_GL_SwapWindow(window); // dbl buffer

    // user interaction (mouse, keys, ...)
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event); // forward SDL events to ImGUI
      
      user::user_event_SDL(event);
      user::win_event_SDL(event);
      user::Imgui_io(io);

      if (event.type == SDL_QUIT)
      {
        close = true;
      }
    }
  } while (!close);

  gpu_free_buffers(); // free point cloud vbo
  grid_free(); 
  axes_free();
  
  SDL_Quit();
  db::db_close();

  return 0;
}
