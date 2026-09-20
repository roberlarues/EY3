#version 300 es

layout(location = 0) in vec3 position;   // model space
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 viewProjection;   // built from where the player's eyes are
uniform vec2 spriteFrame;      // frame within the sheet: x = start, y = width

out vec3 worldNormal;
out vec2 uv;

void main()
{
	// Every scaling in this game is either uniform or in the plane of the
	// surface, so the model matrix rotates normals correctly on its own.
	worldNormal = normalize(mat3(model) * normal);
	uv = vec2(spriteFrame.x + texCoords.x * spriteFrame.y, texCoords.y);

	gl_Position = viewProjection * model * vec4(position, 1.0);
}
