/*
  User interaction handling
  partially generic module

  (C) 2025
  author: w.schulz

  json, user input, insta algo-rerun, event handling
*/

#ifndef USER_HPP
#define USER_HPP

#include "database.hpp"

namespace user {
  ImGuiIO io;

  static int mx, my; // mouse-pos
  static int dmx, dmy; // mouse-delta

  std::vector<const char*> list_items;
///  static int item_current = 1;
  char prevbuf[256];
// s. database.hpp -->  struct note { ... };
  static int item_selected = 1;

  enum e_STATE { STATE_NONE, STATE_NEW, STATE_EDIT };
  e_STATE curr_state = STATE_EDIT;

  db::s_Note s_note;

  ///  bool mouse_clicked = false;

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
    if (event.type == SDL_MOUSEWHEEL && !io.WantCaptureMouse)
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
    if (event.type == SDL_MOUSEWHEEL && !io.WantCaptureMouse)
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
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_LMASK) && !io.WantCaptureMouse)//event.button.button == SDL_BUTTON_LEFT)
    {
      cam.rot[0] += dmx * 3.14159f / 180.0f;
      cam.rot[1] += dmy * 3.14159f / 180.0f;
    }
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_RMASK) && !io.WantCaptureMouse)
    {
      cam.trans[0] += dmx;
      cam.trans[2] -= dmy;
    }
    if ((event.type == SDL_MOUSEMOTION) && (event.motion.state & SDL_BUTTON_MMASK) && !io.WantCaptureMouse)
    {
      cam.rot[0] += dmy * 3.14159f / 180.0f;
      cam.rot[2] += dmx * 3.14159f / 180.0f;
    }

    if (event.motion.x > 0) // check, if mouse moves out of windows (this yields high event.x)
      mx = event.motion.x;  // or use event.motion.xrel, yrel
    if (event.motion.y > 0) my = event.motion.y;
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

/*  void Imgui_io() {
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
  */
  // (C) by chatgpt
  bool ListBoxSelectable(
    const char* label,
    int* current_item,
//    const std::vector<std::string>& items,
    const std::vector<const char*>& items,
    int height_in_items = 8
  )
  {
    bool changed = false;

    float item_height = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 size = ImVec2(0, item_height * height_in_items);

    ImGui::BeginGroup();
    ImGui::TextUnformatted(label);

    // A unique ID helps avoid collisions if used multiple times
    ImGui::PushID(label);

    if (ImGui::BeginChild("##listbox", size, true))
    {
      for (int i = 0; i < items.size(); i++)
      {
        bool is_selected = (i == *current_item);

        if (ImGui::Selectable(items[i], is_selected))
        {
          *current_item = i;
          changed = true;
        }
      }
    }

    ImGui::EndChild();
    ImGui::PopID();
    ImGui::EndGroup();

    return changed;
  }

  void update_detail() {
    std::string sid = std::to_string(db::rows_id[item_selected]);
    std::string topic = db::sql_string("SELECT topic from notes WHERE rowid=" + sid + ";"); // Ugly, but works
    strcpy(s_note.topic, topic.c_str());
    std::string note = db::sql_string("SELECT note from notes WHERE rowid=" + sid + ";");
    user::editor.SetText(note);
///    item_current = -1;
///    mouse_clicked = false;
  }

  void Imgui_draw(SDL_Window* window, ImGuiIO& io) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    {
      static float f = 0.0f;
      static int counter = 0;
      static char buf[256];

      ImGui::Begin("found");
      ImGui::InputText("search", buf, IM_ARRAYSIZE(buf));
      if (std::strncmp(buf, prevbuf, sizeof(prevbuf)) != 0) {
        // search in DB
        list_items.clear();
        db::sql_search_topics(buf, &list_items);
      }
      strcpy_s(prevbuf, sizeof(prevbuf), buf);

///      if (mouse_clicked) update_detail();
      // fill listbox with topics from DB
//      ImGui::ListBox("topics", &item_current, list_items.data(), static_cast<int>(list_items.size()), 30);
//      if (ImGui::IsItemHovered()) ImGui::SetTooltip("ListBox hovered %d", item_current);
/*      if (ImGui::BeginListBox("topics"))
      {
        for (int i = 0; i < list_items.size(); i++)
        {
          bool is_selected = (i == item_selected);
          if (ImGui::Selectable(list_items[i], is_selected))
          {
            // This runs when the user clicks the item
            item_selected = i;
            // Do whatever you want on click
            printf("Clicked item: %d\n", i);
            update_detail();
          }
          if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
*/
///      printf("%d", item_current);
      if (ListBoxSelectable("topics", &item_selected, list_items, 28))
      {
//        printf("Clicked item index = %d\n", item_selected);
        update_detail();
      }
      ImGui::End();


      ImGui::Begin(COMP_NAME);                               // Create an ImGUI window called <COMP> and append into it.
      if (ImGui::Button("New DB entry")) {
        user::editor.SetText("");
        s_note.topic[0] = '\0';
        s_note.tags[0] = '\0';
///        item_current = -1;
        curr_state = STATE_NEW;
      }
      ImGui::SameLine();
      if (ImGui::Button("Update DB")) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
//        db::write_sql("Note from BrAIn at run time");
        s_note.note = editor.GetText();
        switch (curr_state) {
          case STATE_NONE:
            std::cout << "No changes to save." << std::endl;
            break;
          case STATE_NEW:
            db::sql_insert(s_note);
            curr_state = STATE_EDIT;
            break;
          case STATE_EDIT:
            db::sql_update(editor.GetText()); // 2do: pass s_note
            break;
        }
      }
      ImGui::SameLine();
      curr_state == STATE_EDIT ? ImGui::Text("EDIT") : 
        (curr_state == STATE_NEW ? ImGui::Text("NEW") : ImGui::Text("No changes"));
      ImGui::InputText("topic", s_note.topic, IM_ARRAYSIZE(s_note.topic));
      ImGui::InputText("tags", s_note.tags, IM_ARRAYSIZE(s_note.tags));
      ImGui::InputText("created", buf, IM_ARRAYSIZE(buf));
      ImGui::InputText("modified", buf, IM_ARRAYSIZE(buf));
      ImGui::InputText("origin", s_note.origin, IM_ARRAYSIZE(s_note.origin));
      editor.Render("TextEditor");
      ImGui::End();
    }
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void Imgui_events() {
/*    static bool selected = false;

    // 2do, compensate text editor delay
    // (1) https://github.com/ChemistAion/ImTextEdit ... hmm,jo aber liegt nicht am TextEditor, Problem auch bei InputText
    // (2) https://github.com/ocornut/imgui/issues/1485
    // chatgpt: To detect which item in an ImGui ListBox was clicked with the mouse, you should not rely on the old ListBox() API (it does not provide per-item click events).
    if (ImGui::IsMouseClicked(0) && item_current >= 0) {
      printf("klicked");
//      mouse_clicked = true;
      std::string sid = std::to_string(db::rows_id[item_current]);
      std::string topic = db::sql_string("SELECT topic from notes WHERE rowid=" + sid + ";"); // Ugly, but works
      strcpy(s_note.topic,topic.c_str());
      std::string note = db::sql_string("SELECT note from notes WHERE rowid="+sid+";");
      user::editor.SetText(note);
//      user::editor.Render("TextEditor");
//      item_current = -1;
    }
*/
  }

} // namespace user

#endif // USER_HPP
