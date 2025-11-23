/*
  LT-Stack
  (C) 2025
  author: w.schulz

  construct pointcloud gpu-bufs, render pointcloud and other
*/
#ifndef DRAW_HPP
#define DRAW_HPP

//#include "pointcloud/pointcloud.hpp"
#include "math/linmath.h"
///#include "features/freespace.h" // <-- verallgemeinern!! s. CompGEN

ImVec4 clear_color = ImVec4(90 / 255.0, 90 / 255.0, 20 / 255.0, 1.00f);
ImVec4 overrunnable_color = ImVec4(0.0f,1.0f,0.0f, 1.00f);
ImVec4 overridable_color = ImVec4(1.0f, 1.0f,0.0f, 1.00f);
ImVec4 obstacle_color = ImVec4(0.9f, 0.0f, 0.6f, 1.00f);

int colorcoding = 0;

class camera
{
public:
  vec3 rot;
  vec3 trans;
};
camera cam;
float zoom = -30.0f;

const char* vertex_shader_text =
#include "./shader/start.vs"
;
const char* fragment_shader_text =
#include "./shader/start.fs"
;

GLuint program;
GLint mvp_location;
GLint T_location;

// --------------
// vertex + color
// --------------
GLuint vertex_buffer;
GLuint colorbuffer;
// ... axes
GLuint axes_vbo;
GLuint axes_vbo_col;
// ... grid
GLuint grid_vbo;
GLuint grid_vbo_col;
// ... cube
GLuint cube_vbo;
GLuint cube_vbo_col;
GLuint cube_vbo_tex;


// (b) gpu shader attributes
GLint vpos_location, vcol_location, tex_location;

bool b_drawcube;

#include "draw_primitives.hpp"

void gpu_create_shaders() {  
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);
  // https://www.khronos.org/opengl/wiki/Example/GLSL_Shader_Compile_Error_Testing
  GLint isCompiled = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(vertex_shader); // Don't leak the shader.
    return;
  }
  // Shader compilation is successful.

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);
  isCompiled = 0;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(fragment_shader); // Don't leak the shader.
    return;
  }
  // Shader compilation is successful.

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
}

void gpu_create_buffers() { // Buffer auf der Grafikkarte erstellen
  // generate <n> buffer object names
  glGenBuffers(1, &vertex_buffer);
  GLenum err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
  glGenBuffers(1, &colorbuffer);
  err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
}

void gpu_free_buffers() {
  glDeleteBuffers(1, &vertex_buffer);
//  glDeleteBuffers(3, colorbuffer);
  glDeleteBuffers(1, &colorbuffer);
}

void gpu_create_variables() {
  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");
  tex_location = glGetAttribLocation(program, "texCoord");
  T_location = glGetUniformLocation(program, "T"); // translation
  GLenum err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
}
/*
void gpu_push_buffers(lloft::pointcloud p) {
    // (a) vertices
    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::nvertices*3, &lloft::vertices[0], GL_STATIC_DRAW);
    // create a new buffer object data store
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // create new data store for a buffer object, copy <data> into data store
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::nvertices*3, &lloft::colors[0], GL_STATIC_DRAW);
    // enable generic attribute --> s. link to shader ... vcol_location = glGetAttribLocation(program, "vCol");
    glEnableVertexAttribArray(vcol_location);
    // define (specify location and data format of) an array of attribute data
    // If pointer is not NULL, a non-zero named buffer object must be bound to the GL_ARRAY_BUFFER target 
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}
*/
void enable_colorbuffer(int color_coding) {
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

mat4x4 mvp;
void cam_update(float width, float height) { // or just view update
  float aspect;
  mat4x4 view, projection;

  // Calculate aspect of window
  if (height > 0)
    aspect = (float)width / (float)height;
  else
    aspect = 1.f;

  // Setup perspective projection matrix
  glMatrixMode(GL_PROJECTION);
  mat4x4_perspective(projection,
    65.f * (float)M_PI / 180.f,
    aspect,
    0.1f, 300.f);

  glMatrixMode(GL_MODELVIEW);
  {
    vec3 eye = { 3.f, 1.5f, 3.f };
    vec3 center = { 0.f, 0.f, 0.f };
    vec3 up = { 0.f, 1.f, 0.f };
    mat4x4_look_at(view, eye, center, up);
  }

  mat4x4_translate(view, cam.trans[0], cam.trans[1], zoom + cam.trans[2]);
  mat4x4_rotate_X(view, view, cam.rot[0]);
  mat4x4_rotate_Z(view, view, 90.0f + cam.rot[2]);
  mat4x4_rotate_Y(view, view, 225.f+/*180.0f + */cam.rot[1]);

  mat4x4_mul(mvp, projection, view);
}

void render(SDL_Window* window) { // window als param is misleading, da nur f. w,h, benutzt, gemalt wird in den context
  int width, height;
  SDL_GetWindowSize(window, &width, &height);
  cam_update(width, height);

  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // https://stackoverflow.com/questions/46020871/opengl-depth-testing-not-working-glew-sdl2

  glUseProgram(program);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
/*
  // (a) draw/render point cloud
  glPointSize(4);
  int npoints = lloft::nvertices;// .size();
  // now as we have multiple vbo (1 for point cloud, 1 for grid), we need to
  // (a) bind the buffer (pos+col) and 
  // (b) set the shader attribute
  // before drawing
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); // 2do, there is colorcoding and color_coding
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(npoints));
  */
  glLineWidth(3);
  axes_render();
  glLineWidth(1);

  grid_render();

  if (b_drawcube) {
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f); // pseudo camera position
    gpu_cube_render();
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f);
  }

  GLenum err = glGetError();
  if (err != 0)
  {
      int i = 1;
  }
}

void resize(int w, int h) {
  if (h == 0) h = 1;        // Prevent A Divide By Zero
  glViewport(0, 0, w, h);   // Reset The Current Viewport
 }

#endif // DRAW_HPP
