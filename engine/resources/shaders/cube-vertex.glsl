#version 400

#pragma shader_stagevertex
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "/common/shared.glsl"

layout(push_constant) uniform PushConstants
{
    mat4 mvp;
    mat4 model;
    float time;
} pushConstants;

layout (set = 0, binding = 0) uniform UniformBuffer
{
    mat4 mvp;
} uniformBuffer;

layout (set = 0, location = 0) in vec3 pos;

layout (location = 0) out vec3 outColor;

void main()
{
    // mat4 mvp = pushConstants.clip * pushConstants.proj * pushConstants.view * pushConstants.model;

    float noiseScale = 3.0f;
    float time = pushConstants.time * .5f;

    vec3 modifiedPos = pos * vec3(1, sin(time) * .5f + 1.75f, 1);
    // + vec3(
    //     perlinNoise3D(pos * noiseScale + vec3(time, 0.0f, 0.0f)),
    //     perlinNoise3D((pos + vec3(255.0f, 151.0f, 125.0f)) * noiseScale + vec3(0.0f, time, 0.0f)) * .3f,
    //     perlinNoise3D((pos + vec3(457.0f, 347.0f, time + 574.0f)) * noiseScale + vec3(0.0f, time, 0.0f)) * .1f
    // ) * .2f;

    vec4 finalPos = pushConstants.mvp * vec4(modifiedPos, 1.0);
    gl_Position = finalPos;
    outColor = modifiedPos;
}