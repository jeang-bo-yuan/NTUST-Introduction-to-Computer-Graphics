#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
   vec3 color;
} f_in;

uniform sampler2D u_texture;

void main()
{   
    vec3 color = vec3(texture(u_texture, f_in.texture_coordinate));
    color.x = color.x * f_in.color.x;
    color.y = color.y * f_in.color.y;
    color.z = color.z * f_in.color.z;
    f_color = vec4(color, 1.0f);
}