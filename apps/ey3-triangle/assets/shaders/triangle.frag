#version 300 es

precision mediump float;
out vec4 fragColor;

uniform vec3 color;

void main()
{
   fragColor = vec4 ( color, 0.8 );
}
