#version 330 core
out vec4 FragColor;
out vec4 openColor;
out vec4 closeColor;

uniform vec4 color;
uniform vec4 open;
uniform vec4 close;

void main()
{
    FragColor = color;
    openColor = open;
    closeColor = close;
}
