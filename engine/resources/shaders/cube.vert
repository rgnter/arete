#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform UniformBuffer
{
    mat4 mvp;
} uniformBuffer;

layout(push_constant) uniform PushConstants
{
    mat4 clip;
    mat4 proj;
    mat4 view;
    mat4 model;
} pushConstants;

layout (set = 0, location = 0) in vec3 pos;

void main()
{
    mat4 mvp = pushConstants.clip * pushConstants.proj * pushConstants.view * pushConstants.model;
    gl_Position = mvp * vec4(pos, 1.0);
}