#version 300 es

precision mediump float;

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D sprite;

void main()
{
	vec4 texel = texture(sprite, texCoords);

	// Nothing to show and nothing to write to the depth buffer either: a
	// fully transparent fragment that still wrote depth would hide whatever
	// is drawn behind it afterwards.
	if (texel.a < 0.01) {
		discard;
	}

	fragColor = texel;
}
