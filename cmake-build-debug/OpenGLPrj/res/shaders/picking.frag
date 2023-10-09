#version 330 core
out vec4 FragColor;

uniform ivec4 code;

void main() {
    FragColor = vec4(code.r/ 255.0, code.g/255.0, code.b/255.0, code.a/255.0);
}
