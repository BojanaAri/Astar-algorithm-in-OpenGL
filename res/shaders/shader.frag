#version 330 core
out vec4 FragColor;
out vec4 openColor;

uniform vec4 color;
uniform vec4 open;

void main()
{
    FragColor = color;
    openColor = open;
}
