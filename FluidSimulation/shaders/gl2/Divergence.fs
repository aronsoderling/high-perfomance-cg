uniform vec2 invRes;

uniform sampler2D w;

varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = gl_FragCoord.xy * invRes;
	vec2 wL = (texture2D(w, coords - vec2(invRes.x, 0.0)).rg * 2.0) - 1.0;
	vec2 wR = (texture2D(w, coords + vec2(invRes.x, 0.0)).rg * 2.0) - 1.0;
	vec2 wB = (texture2D(w, coords - vec2(0.0, invRes.y)).rg * 2.0) - 1.0;
	vec2 wT = (texture2D(w, coords + vec2(0.0, invRes.y)).rg * 2.0) - 1.0;

	float r = 0.5 *(wR.x - wL.x) + (wT.y - wB.y);

	gl_FragData[0] = vec4((r + 1.0) / 2.0, 0.0, 0.0, 1.0);
}