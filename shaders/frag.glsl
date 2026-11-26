#version 330 core

in vec2 LocalPos;

out vec4 FragColor;
uniform bool circle;
uniform vec4 colour;

void main() {
    if(circle && length(LocalPos) > 0.5) {
        discard;
    }

    FragColor = colour;
}
