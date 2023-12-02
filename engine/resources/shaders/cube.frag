#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "shared.glsl"

layout (location = 0) flat in uint polygonIndex;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0 / polygonIndex, 1.0 / polygonIndex, 1.0 / polygonIndex, 1.0);
}