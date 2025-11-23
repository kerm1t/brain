#ifndef LT_APP_H
#define LT_APP_H

constexpr char* COMP_NAME = "BrAIn";
constexpr char* APP_TITLE = "BrAIn (c) 2025";
///constexpr char* ECAL_TITLE = "LT GEN";
///constexpr char* ECAL_INIT_OK = "GEN eCAL publishers initialized";

int bg_color[3] = { 135,130,121 }; // r,g,b

char* _strcat(char* a, char* b) {
  char buf[255];
  strcpy(buf, a);
  strcat(buf, b);
  return buf;
}

// eCAL Topics s. app_ecal.hpp



#endif // LT_APP_H
