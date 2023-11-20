#version 430 core
layout(location = 0) in vec3 pos;

uniform vec3 translate = vec3(0, 0, 0);
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform uint frame;
uniform bool draw_wave = true;

out vec3 vs_world_pos;
out vec3 vs_normal;

// y  = sin(x / 50)
// y' = cos(x / 50) / 50

void main() {
  if (draw_wave) {
    // changes over frames
    float offset = float(frame) / 20;

    vs_world_pos = vec3(pos.x, 5 * sin((pos.x + offset) * 3.14 / 50.f), pos.z) + translate;
    gl_Position = proj_matrix * view_matrix * vec4(vs_world_pos, 1);

    float slope = 5 * cos((pos.x + offset) * 3.14 / 50.f) * 3.14 / 50.f;
    vs_normal = normalize(vec3(-slope, 1, 0));
  }
  else {
    vs_world_pos = pos + translate;
    gl_Position = proj_matrix * view_matrix * vec4(vs_world_pos, 1);
    vs_normal = vec3(0, 1, 0);
  }
}
