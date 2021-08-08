#version 460 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec3 vertexColor;

void main()
{
    fragColor = vec4(vertexColor, 1.0f);
}
