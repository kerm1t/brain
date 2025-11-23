/*
  LT-Stack
  (C) 2025
  author: w.schulz

  construct grid, cube gpu-bufs
*/

#include <vector>

#ifndef DRAW_PRIMITIVES_HPP
#define DRAW_PRIMITIVES_HPP

void gpu_cube() { // set cube data and push to gpu vbo
  static const GLfloat g_vertex_buffer_data[] = {
      -1.0f,-1.0f,-1.0f, // triangle 1 : begin
      -1.0f,-1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f, // triangle 1 : end
       1.0f, 1.0f,-1.0f, // triangle 2 : begin
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f,-1.0f, // triangle 2 : ...
       1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f,-1.0f,
       1.0f,-1.0f,-1.0f,
       1.0f, 1.0f,-1.0f,
       1.0f,-1.0f,-1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f,-1.0f,
       1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f,-1.0f, 1.0f,
       1.0f,-1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f,-1.0f,-1.0f,
       1.0f, 1.0f,-1.0f,
       1.0f,-1.0f,-1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f,-1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f,-1.0f,
      -1.0f, 1.0f,-1.0f,
       1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
       1.0f,-1.0f, 1.0f
  };
  static const GLfloat g_color_buffer_data[] = {
    0.583f, 0.771f, 0.014f,
    0.609f, 0.115f, 0.436f,
    0.327f, 0.483f, 0.844f,
    0.822f, 0.569f, 0.201f,
    0.435f, 0.602f, 0.223f,
    0.310f, 0.747f, 0.185f,
    0.597f, 0.770f, 0.761f,
    0.559f, 0.436f, 0.730f,
    0.359f, 0.583f, 0.152f,
    0.483f, 0.596f, 0.789f,
    0.559f, 0.861f, 0.639f,
    0.195f, 0.548f, 0.859f,
    0.014f, 0.184f, 0.576f,
    0.771f, 0.328f, 0.970f,
    0.406f, 0.615f, 0.116f,
    0.676f, 0.977f, 0.133f,
    0.971f, 0.572f, 0.833f,
    0.140f, 0.616f, 0.489f,
    0.997f, 0.513f, 0.064f,
    0.945f, 0.719f, 0.592f,
    0.543f, 0.021f, 0.978f,
    0.279f, 0.317f, 0.505f,
    0.167f, 0.620f, 0.077f,
    0.347f, 0.857f, 0.137f,
    0.055f, 0.953f, 0.042f,
    0.714f, 0.505f, 0.345f,
    0.783f, 0.290f, 0.734f,
    0.722f, 0.645f, 0.174f,
    0.302f, 0.455f, 0.848f,
    0.225f, 0.587f, 0.040f,
    0.517f, 0.713f, 0.338f,
    0.053f, 0.959f, 0.120f,
    0.393f, 0.621f, 0.362f,
    0.673f, 0.211f, 0.457f,
    0.820f, 0.883f, 0.371f,
    0.982f, 0.099f, 0.879f
  };

  // (a) vertices
  glGenBuffers(1, &cube_vbo);
  glGenBuffers(1, &cube_vbo_col);

  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  GLenum err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
  glEnableVertexAttribArray(vpos_location);
  err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }

  // (b) color
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  err = glGetError();
  if (err != 0)
  {
    int i = 1;
  }
  glGenBuffers(1, &cube_vbo_tex);
  // (c) add texture coordinates. even if we do not use textures, it will be very handy
  // to create shader effects
  //  https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06
  GLfloat g_tex_buffer_data[2 * 4 * 6] = {
    // front
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
  };
  for (int i = 1; i < 6; i++)
    memcpy(&g_tex_buffer_data[i * 4 * 2], &g_tex_buffer_data[0], 2 * 4 * sizeof(GLfloat));
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_tex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_tex_buffer_data), g_tex_buffer_data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(tex_location);
  glVertexAttribPointer(tex_location, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
};

void gpu_cube_render() {
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void gpu_cube_free() {
  glDeleteBuffers(1, &cube_vbo);
  glDeleteBuffers(1, &cube_vbo_col);
}


std::vector<float> axes_vertices;
std::vector<float> axes_colors;
void axes_create() {
  float len = 5.0f;
  // x 
  axes_vertices.push_back(0); axes_vertices.push_back(.0f); axes_vertices.push_back(0.0f);
  axes_vertices.push_back(len); axes_vertices.push_back(0.0f); axes_vertices.push_back(0.0f);
  axes_colors.push_back(1.0f); axes_colors.push_back(0.0f); axes_colors.push_back(0.0f);
  axes_colors.push_back(1.0f); axes_colors.push_back(0.0f); axes_colors.push_back(0.0f);

  // y 
  axes_vertices.push_back(0); axes_vertices.push_back(.0f); axes_vertices.push_back(0.0f);
  axes_vertices.push_back(.0f); axes_vertices.push_back(len); axes_vertices.push_back(0.0f);
  axes_colors.push_back(.0f); axes_colors.push_back(1.0f); axes_colors.push_back(0.0f);
  axes_colors.push_back(.0f); axes_colors.push_back(1.0f); axes_colors.push_back(0.0f);

  // z 
  axes_vertices.push_back(0); axes_vertices.push_back(.0f); axes_vertices.push_back(0.0f);
  axes_vertices.push_back(0.0f); axes_vertices.push_back(0.0f); axes_vertices.push_back(len);
  axes_colors.push_back(.0f); axes_colors.push_back(0.0f); axes_colors.push_back(1.0f);
  axes_colors.push_back(.0f); axes_colors.push_back(0.0f); axes_colors.push_back(1.0f);

  glGenBuffers(1, &axes_vbo);
  glGenBuffers(1, &axes_vbo_col);
};
void axes_free() {
  glDeleteBuffers(1, &grid_vbo);
  glDeleteBuffers(1, &grid_vbo_col);
};
void axes_gpu_push_buffers() {
  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * axes_vertices.size(), &axes_vertices[0], GL_STATIC_DRAW);
  // (b) colors
  glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * axes_colors.size(), &axes_colors[0], GL_STATIC_DRAW);
};
void axes_render() {
  glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(axes_vertices.size()));
};


std::vector<float> grid_vertices;
std::vector<float> grid_colors;

void grid_create() {
  for (int x = -50; x <= 50; x += 10) {
    // 2do, somewhat it didn't work to push a point to a vector of points, instead of a vector of floats <-- on ARM? 
    grid_vertices.push_back(x);
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(x);
    grid_vertices.push_back(50.0f);
    grid_vertices.push_back(0.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
  for (int y = -50; y <= 50; y += 10) {
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(y);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(50.0f);
    grid_vertices.push_back(y);
    grid_vertices.push_back(0.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
  glGenBuffers(1, &grid_vbo);
  glGenBuffers(1, &grid_vbo_col);
};

void grid_free() {
  glDeleteBuffers(1, &grid_vbo);
  glDeleteBuffers(1, &grid_vbo_col);
};

void grid_gpu_push_buffers() {
  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_vertices.size(), &grid_vertices[0], GL_STATIC_DRAW);
  // (b) colors
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_colors.size(), &grid_colors[0], GL_STATIC_DRAW);
};

void grid_render() {
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(grid_vertices.size()));
};


#endif // DRAW_PRIMITIVES_HPP