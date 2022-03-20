#version 450 core

layout(binding = 0, std140) uniform Color {
    vec4 color;
};

vec2 pos[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

layout(location = 0) out vec4 o_color;

void main()
{
    o_color = color;
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
}
