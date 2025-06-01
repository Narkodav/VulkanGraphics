#version 450

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 2) in vec4 inModelTransform0; // First column
layout(location = 3) in vec4 inModelTransform1; // Second column
layout(location = 4) in vec4 inModelTransform2; // Third column
layout(location = 5) in vec4 inModelTransform3; // Fourth column

layout(location = 0) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformTransforms {
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    mat4 inModelTransform = mat4(
        inModelTransform0,
        inModelTransform1,
        inModelTransform2,
        inModelTransform3
    );

    gl_Position = ubo.proj * ubo.view * inModelTransform * inPosition;
    fragTexCoord = inTexCoord;
}