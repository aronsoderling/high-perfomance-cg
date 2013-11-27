uniform sampler2D p;
uniform sampler2D w;
uniform vec2 invRes;

void main()
{
	vec2 coords = gl_FragCoord.xy * invRes;
	float pL = ((texture2D(p, coords - vec2(invRes.x, 0.0)).r) * 2.0) - 1.0;
	float pR = ((texture2D(p, coords + vec2(invRes.x, 0.0)).r) * 2.0) - 1.0;
	float pB = ((texture2D(p, coords - vec2(0.0, invRes.y)).r) * 2.0) - 1.0;
	float pT = ((texture2D(p, coords + vec2(0.0, invRes.y)).r) * 2.0) - 1.0;

	vec4 uNew = (texture2D(w, coords) * 2.0) - 1.0;
	uNew.xy = ((uNew.xy - 0.5 * vec2(pR - pL, pT - pB)) + 1.0) / 2.0;
	gl_FragData[0] = uNew;
}