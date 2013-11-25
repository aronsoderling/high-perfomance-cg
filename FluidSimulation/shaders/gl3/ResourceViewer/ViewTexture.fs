#version 150

uniform sampler2D VTexture;
uniform int channel;
uniform int split;

in vec2 vtx;
out vec4 fragColor;

void main()
{
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	if (split != 0) {
		vec2 t = vtx * 2.0;
		vec2 tc = vec2(mod(t, 1.0));
		vec4 textureColor = texture2D(VTexture, tc);
		if (t.x < 1.0 && t.y < 1.0)
			fragColor.rgb = vec3(0.0, 0.0, textureColor.b);
		else if (t.x < 1.0 && t.y >= 1.0)
			fragColor.rgb = vec3(textureColor.r, 0.0, 0.0);
		else if (t.x >= 1.0 && t.y < 1.0)
			fragColor.rgb = textureColor.aaa;
		else
			fragColor.rgb = vec3(0.0, textureColor.g, 0.0);
	} else {
		vec4 textureColor = texture2D(VTexture, vtx);
		if (channel == 8) {
			fragColor.rgb = textureColor.aaa;
		} else {
			if ((channel & 1) != 0)
				fragColor.r = textureColor.r;
			if ((channel & 2) != 0)
				fragColor.g = textureColor.g;
			if ((channel & 4) != 0)
				fragColor.b = textureColor.b;
			if ((channel & 8) != 0) {
				vec2 c = vec2(mod(vtx.x * 64.0, 1.0), mod(vtx.y * 64.0, 1.0));
				float checker = 1.0;
				if ((c.x < 0.5 && c.y < 0.5) || (c.x >= 0.5 && c.y >= 0.5))
					checker = 0.75;
				fragColor.rgb = textureColor.a * fragColor.rgb + (1.0 - textureColor.a) * checker;
			}
		}
	}
	//fragColor = vec4(color.xyz, 1.0);
}
