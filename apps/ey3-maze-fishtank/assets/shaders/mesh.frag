#version 300 es

precision mediump float;

in vec3 worldNormal;
in vec2 uv;
out vec4 fragColor;

uniform sampler2D surface;
uniform vec3 tint;

// The light comes from over the player's left shoulder, so the faces of a
// cube separate from each other and the box reads as a box.
const vec3 lightDirection = vec3(-0.35, 0.45, 0.82);
const float ambient = 0.55;

void main()
{
	vec4 texel = texture(surface, uv);

	// Cut out rather than blend: a transparent texel is simply not drawn, so
	// the depth buffer sorts everything and the scene needs no ordering.
	if (texel.a < 0.5) {
		discard;
	}

	float diffuse = max(dot(normalize(worldNormal), normalize(lightDirection)), 0.0);
	fragColor = vec4(texel.rgb * tint * (ambient + (1.0 - ambient) * diffuse), 1.0);
}
