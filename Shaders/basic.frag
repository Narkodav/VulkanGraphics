#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) flat in uint textureId;

layout(set = 1, binding = 0) uniform sampler2D textures[];
//layout(set = 0, binding = 1) uniform ContrastControl {
//    float contrast; // 0.0 = grayscale, 1.0 = normal, >1.0 = increased contrast
//} contrastUbo;

layout(location = 0) out vec4 outColor;

vec4 adjustContrast(vec4 color, float contrast) {
    // Convert to grayscale using luminance weights
    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    
    // Interpolate between grayscale and original color based on contrast
    vec3 adjustedColor = mix(vec3(luminance), color.rgb, contrast);
    
    return vec4(adjustedColor, color.a);
}

void main() {
    vec4 texColor = texture(textures[textureId], fragTexCoord);
    outColor = adjustContrast(texColor, 1.0);
}