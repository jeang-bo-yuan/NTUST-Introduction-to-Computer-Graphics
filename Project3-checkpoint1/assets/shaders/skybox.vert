#version 430 core
layout (location = 0) in vec3 position;

uniform vec3 translate = vec3(0, 0, 0);
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform uint frame;

out vec3 tex_coord;

void main() {
  gl_Position = proj_matrix * mat4(mat3(view_matrix)) * vec4(position, 1);
  tex_coord = position;
}
