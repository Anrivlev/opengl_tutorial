#version 330 core

layout (location = 0) in vec3 Position;

uniform float gScale;

uniform mat4 gRotation;

void main() {
    // gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
    gl_Position = gRotation * vec4(Position, 1.0);
}