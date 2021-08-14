#version 460 core

layout (set = 1, binding = 0) uniform texture2D t;
layout (set = 1, binding = 1) uniform sampler s;

layout (location = 0) in vec2 texture_coord;

layout (location = 0) out vec4 frag_color;

void main()
{
//    vec4(1.0, 1.0, 1.0 * texture_coord.y, 1.0); //
    frag_color = texture(sampler2D(t, s), texture_coord);
}
