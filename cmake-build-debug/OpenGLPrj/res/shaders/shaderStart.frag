#version 330 core

uniform uint objectID;
out uvec4 FragColor;

void main() {
    FragColor = uvec4(objectID, 0, 0, 0);
}
