#version 460

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_color;

layout (location = 0) out vec3 f_color;

void main()
{
    f_color = v_color;
    gl_Position = vec4(v_pos, 1.0f);
}