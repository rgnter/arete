#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform buf {
    mat4 mvp;
} ubuf;

layout (location = 0) in vec3 pos;

void main() {
    gl_Position = ubuf.mvp * vec4(pos, 1.0);
}