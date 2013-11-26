uniform sampler2D p;
uniform sampler2D w;
uniform vec2 invRes;

void main()
{
	vec2 coords = gl_FragCoord.xy * invRes;
	float pL = texture2D(p, coords - vec2(invRes.x, 0.0)).r;
	float pR = texture2D(p, coords + vec2(invRes.x, 0.0)).r;
	float pB = texture2D(p, coords - vec2(0.0, invRes.y)).r;
	float pT = texture2D(p, coords + vec2(0.0, invRes.y)).r;

	vec4 uNew = texture2D(w, coords);
	//uNew.xy = uNew.xy - 0.5 * vec2(pR - pL, pT - pB);
	gl_FragData[0] = uNew;
}