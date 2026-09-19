#version 300 es

layout(location = 0) in vec2 quadPosition;   // unit quad, (0,0) top-left .. (1,1) bottom-right
layout(location = 1) in vec2 quadTexCoords;

uniform vec2 screenSize;   // viewport, in pixels
uniform vec4 spriteRect;   // x, y, width, height, in pixels, origin at the top-left corner
uniform vec2 spriteFrame;  // frame within the sheet: x = where it starts, y = how wide it is
uniform float depth;       // NDC depth: -1.0 is nearest, 1.0 is farthest

out vec2 texCoords;

void main()
{
	// A sheet is a horizontal strip of frames, so only u picks the frame.
	texCoords = vec2(spriteFrame.x + quadTexCoords.x * spriteFrame.y, quadTexCoords.y);

	// Pixel coordinates (y growing downwards, like touch events) to NDC.
	vec2 pixel = spriteRect.xy + quadPosition * spriteRect.zw;
	vec2 ndc = pixel / screenSize * 2.0 - 1.0;

	gl_Position = vec4(ndc.x, -ndc.y, depth, 1.0);
}
