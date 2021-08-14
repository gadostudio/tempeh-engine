#version 460 core

layout (set = 0, binding = 0) uniform Uniforms {
    mat4 transform_mat;
};

layout (location = 0) in vec3 coord;
layout (location = 1) in vec2 tex_coord_in;

layout (location = 0) out vec2 tex_coord_out;

void main()
{
    gl_Position = transform_mat * vec4(coord, 1.0f);
    tex_coord_out = tex_coord_in;
}
