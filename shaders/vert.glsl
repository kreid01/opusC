#version 330 core

layout(location = 0) in vec3 aPos;

out vec2 LocalPos;

uniform vec3 scale;
uniform vec3 position;

void main() {
   LocalPos = aPos.xy;

   vec3 scaled_pos = aPos * scale;
   vec3 adjustedPos = scaled_pos + position;

   gl_Position = vec4(adjustedPos.x, adjustedPos.y, 0.0, 1.0);
}
