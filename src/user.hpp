/*
  LT-Stack
  (C) 2025
  author: w.schulz

  FRS - Freespace and Road Boundaries => make generic
  json, user input, insta algo-rerun, event handling
*/

#ifndef USER_HPP
#define USER_HPP

#include "database.hpp"

namespace user {
  static int mx, my; // mouse-pos
  static int dmx, dmy; // mouse-delta

  std::vector<const char*> list_items;


//  bool b_cfg_changed{ false };
  namespace insta {
    // keep a copy of all config props.
    // compare to GUI, if anything changed -> insta rerun of algorithm
    bool b_draw_frs = true;
    bool b_draw_unknown = false;
    bool b_draw_polyline = true;
    int publish_ground{ true };           // atm (2024:-) GEN needs Point cloud to be fully transferred

    int self_occlusion_az_from{ 0 };      // Ego vehicle occlusion of sensor FoV
    int self_occlusion_az_to{ 0 };        // Ego vehicle occlusion of sensor FoV
    float self_occlusion_distance{ 0.0 }; // Ego vehicle occlusion of sensor FoV
    int omit_seg_start{ 0 };              // Omit these segments starting with ...
    int omit_seg_stop{ 0 };               // Omit these segments stopping with ...
    float road_bounds{ 2.75 };            // Noise points intensity threshold. The reflected points have relatively small intensity than others.

    // overrule the dynamic freespace by a static box
    bool b_frs_in_a_box{ 0 };
    float box_x1{ 0.0 }; // 2do: bbox struct
    float box_y1{ 0.0 };
    float box_x2{ 0.0 };
    float box_y2{ 0.0 };

    bool b_minbeams;
    bool b_erode;
  }

  TextEditor editor; // ImGUI text editor

  void init_Cfg() {
    std::cout << "\n" << "Loading json topic + cfg..." << std::endl;
    bool jsonStatus = false;
    // Read ecal topics from json --> NOT YET USED
    std::string json_path = "./conf/ecal/ecalTopics.json"; // fro MSVC: "../conf", in app: "./conf"
    std::string ecalJsonArgumentHeader = "--topics=";
  }

  bool insta_config() {
    bool b_rerun = false;
    return b_rerun;
  }

  void process_event(SDL_Event& event) { // this is SDL --> move it to ImGUI
    if (event.type == SDL_MOUSEWHEEL)
    {
      if (event.wheel.y > 0) // scroll up
      {
        zoom += 1.0f;
      }
      else if (event.wheel.y < 0) // scroll down
      {
        zoom -= 1.0f;
      }
    }
    dmx = mx - event.motion.x;
    dmy = my - event.motion.y;
    // https://stackoverflow.com/questions/27536941/sdl-mouse-input
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_LMASK))//event.button.button == SDL_BUTTON_LEFT)
    {
      cam.rot[0] += dmx * 3.14159f / 180.0f;
      cam.rot[1] += dmy * 3.14159f / 180.0f;
    }
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_RMASK))
    {
      cam.trans[0] += dmx;
      cam.trans[2] -= dmy;
    }
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_MMASK))
    {
      cam.rot[0] += dmy * 3.14159f / 180.0f;
      cam.rot[2] += dmx * 3.14159f / 180.0f;
    }

    if (event.motion.x > 0) // check, if mouse moves out of windows (this yields high event.x)
      mx = event.motion.x;  // or use event.motion.xrel, yrel
    if (event.motion.y > 0) my = event.motion.y;

    // https://www.libsdl.org/release/SDL-1.2.15/docs/html/guideinputkeyboard.html
    if (event.type == SDL_KEYUP)
    {
      if (event.key.keysym.scancode == SDL_SCANCODE_1) // color coding = 0
      {
        colorcoding = 0;
        enable_colorbuffer(colorcoding);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_2) // color coding = 1
      {
        colorcoding = 1;
        enable_colorbuffer(colorcoding);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_3) // color coding = 2
      {
        colorcoding = 2;
        enable_colorbuffer(colorcoding);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_P) // press key "P"
      {
      }
    }


  }
  void user_event_SDL(SDL_Event& event) { // this is SDL --> move it to ImGUI
    if (event.type == SDL_MOUSEWHEEL)
    {
      if (event.wheel.y > 0) // scroll up
      {
        zoom += 1.0f;
      }
      else if (event.wheel.y < 0) // scroll down
      {
        zoom -= 1.0f;
      }
    }
  }
  void win_event_SDL(SDL_Event& event) {
    if (event.type == SDL_DROPFILE) {      // In case if dropped file
      char* dropped_filedir = event.drop.file;
      std::cout << "main.cpp: loading " << dropped_filedir << std::endl;
      SDL_free(dropped_filedir);    // Free dropped_filedir memory
    }

    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        resize(event.window.data1, event.window.data2);
      }
    }
  }

  void Imgui_io(ImGuiIO& io) {
///    if (io.KeysDown[65] == true) // A
///    {
///      std::cout << "A pressed" << std::endl;
///    }

    // a) control camera
    if (io.MouseClicked && (io.MouseDown[0] == true) && !io.WantCaptureMouse) // not over ImGUI element, https://github.com/ocornut/imgui/issues/52
    {
      dmx = io.MouseDelta.x;
      dmy = io.MouseDelta.y;
      cam.rot[0] += dmy * 3.14159f / 180.0f;
      cam.rot[1] += dmx * 3.14159f / 180.0f;
    }
    if (io.MouseClicked && (io.MouseDown[1] == true) && !io.WantCaptureMouse) //https://github.com/ocornut/imgui/issues/52
    {
      dmx = io.MouseDelta.x;
      dmy = io.MouseDelta.y;
      cam.trans[0] += dmx;
      cam.trans[1] -= dmy;
    }

    if (io.MouseClicked && (io.MouseDown[2] == true) && !io.WantCaptureMouse) // https://github.com/ocornut/imgui/issues/52
    {
      dmx = io.MouseDelta.x;
      dmy = io.MouseDelta.y;
      cam.rot[0] += dmy * 3.14159f / 180.0f;
      cam.rot[2] += dmx * 3.14159f / 180.0f;
    }
  } // Imgui_io


  void Imgui_draw(SDL_Window* window, ImGuiIO& io) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("found");
//      const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
      static int item_current = 1;
//      ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
      ImGui::ListBox("topics", &item_current, list_items.data(), static_cast<int>(list_items.size()), 30);
      ImGui::End();


      ImGui::Begin(COMP_NAME);                               // Create an ImGUI window called <COMP> and append into it.
      if (ImGui::Button("Update DB")) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
//        db::write_sql("Note from BrAIn at run time");
        db::sql_update(editor.GetText());
      }
      editor.Render("TextEditor");
      ImGui::End();
    }
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

} // namespace user

#endif // USER_HPP
