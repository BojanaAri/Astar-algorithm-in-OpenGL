#version 330 core
out vec4 start;
out vec4 end;

uniform vec4 colorS;
uniform vec4 colorE;

void main()
{
    start = colorS;
    end = colorE;
}
