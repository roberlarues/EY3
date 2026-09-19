#version 300 es

precision mediump float;

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D sprite;

// ey3's Texture uploads RGB only, so there is no alpha channel to mask a
// sprite with. Transparency is done with a color key instead: texels painted
// pure magenta (see COLOR_KEY in sprite.h) are thrown away.
const vec3 colorKey = vec3(1.0, 0.0, 1.0);

void main()
{
	vec3 texel = texture(sprite, texCoords).rgb;

	if (all(lessThan(abs(texel - colorKey), vec3(0.02)))) {
		discard;
	}

	fragColor = vec4(texel, 1.0);
}
