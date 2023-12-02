#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//layout (binding = 1) uniform sampler2D tex;
layout (location = 0) in int polygonIndex;
layout (location = 0) out vec4 outColor;

void main() {
    // polygonIndex = 
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}