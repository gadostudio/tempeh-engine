#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (set = 0, binding = 0) uniform Uniforms {
    mat4 transform_mat;
};

layout (location = 0) out vec3 vertexColor;

void main()
{
    gl_Position = transform_mat * vec4(pos, 1.0f);
    vertexColor = color;
}
