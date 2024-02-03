#version 400

#pragma shader_stagefragment
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "common/shared.glsl"

layout(push_constant) uniform PushConstants
{
    layout(offset = 132) float time;
} pushConstants;

layout (location = 0) in vec3 inColor;
layout (location = 0) out vec4 outColor;

void main() {
     float noiseScale = 1.0f;
     float time = pushConstants.time;
     outColor = vec4(
         vec3(.3f)
          + vec3(
              perlinNoise3D(inColor * noiseScale),
              perlinNoise3D((inColor + vec3(255.0f,151.0f,125.0f)) * noiseScale) * .3f,
              perlinNoise3D((inColor + vec3(457.0f,347.0f,574.0f)) * noiseScale) * .1f
          )
          + vec3(
              perlinNoise3D(inColor * noiseScale + vec3(time, 0.0f, 0.0f)) * .8f,
              perlinNoise3D((inColor + vec3(255.0f, 151.0f, 125.0f)) * noiseScale + vec3(0.0f, time, 0.0f)) * .1f,
              perlinNoise3D((inColor + vec3(457.0f, 347.0f, time + 574.0f)) * noiseScale + vec3(0.0f, time, 0.0f)) * .3f
          )
         * .7f,
         1
     );
    //outColor = vec4(vec3(.8f), 1);
}