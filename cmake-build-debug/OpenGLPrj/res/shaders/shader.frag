#version 330 core
out vec4 FragColor;
out vec4 openColor;
out vec4 start;
out vec4 end;

uniform vec4 colorS;
uniform vec4 colorE;
uniform vec4 color;
uniform vec4 open;

void main()
{
    FragColor = color;
    openColor = open;
    start = colorS;
    end = colorE;
}
