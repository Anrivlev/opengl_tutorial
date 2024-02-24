#version 330 core

layout (location = 0) in vec3 Position;

uniform float gScale;

uniform mat4 gTransformation;

void main() {
    gl_Position = gTransformation * vec4(Position, 1.0);
}