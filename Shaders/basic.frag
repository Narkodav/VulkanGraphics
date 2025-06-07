//#version 450
//
//layout(location = 0) in vec2 fragTexCoord;
//layout(location = 1) flat in uint textureId;
//
//layout(location = 0) out vec4 outColor;
//
//layout(set = 0, binding = 1) uniform sampler2D textures[];
//
//void main() {
//    outColor = texture(textures[textureIndex], fragTexCoord);
//}

#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) flat in uint textureId;

layout(location = 0) out vec4 outColor;

uniform sampler2D texSamp;

void main() {
    outColor = texture(texSamp, fragTexCoord);
}