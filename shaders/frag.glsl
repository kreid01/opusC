#version 410 core

in vec2 LocalPos;

out vec4 FragColor;
uniform bool circle;

void main() {
    if(circle && length(LocalPos) > 0.5) {
        discard;
    }

    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
