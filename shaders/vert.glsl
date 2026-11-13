#version 410 core

layout(location = 0) in vec3 aPos;

uniform float u_scale;

void main() {
   vec3 shrunkPos = aPos * u_scale;
   gl_Position = vec4(shrunkPos.x, shrunkPos.y, 0.0, 1.0);
}
