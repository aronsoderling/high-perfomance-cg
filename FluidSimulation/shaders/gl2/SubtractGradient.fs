uniform sampler2D p;
uniform sampler2D w;
uniform sampler2D b;
uniform vec2 invRes;

void main()
{
	vec2 coords = gl_FragCoord.xy * invRes;
	float pL = texture2D(p, coords - vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float pR = texture2D(p, coords + vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float pB = texture2D(p, coords - vec2(0.0, invRes.y)).x * 2.0 - 1.0;
	float pT = texture2D(p, coords + vec2(0.0, invRes.y)).x * 2.0 - 1.0;
	float pC = texture2D(p, coords).x * 2.0 - 1.0;

	float bL = texture2D(b, coords - vec2(invRes.x, 0.0)).x;
	float bR = texture2D(b, coords + vec2(invRes.x, 0.0)).x;
	float bB = texture2D(b, coords - vec2(0.0, invRes.y)).x;
	float bT = texture2D(b, coords + vec2(0.0, invRes.y)).x;

	vec2 uOld = texture2D(w, coords).xy * 2.0 - 1.0;

	vec2 boundaryV = vec2(0.0,0.0);
	vec2 vMask = vec2(1.0,1.0);
	
	if(bL > 0){
		pL = pC;
		boundaryV.x = -uOld.x;
		vMask.x = 0.0;
	}
	if(bR > 0){
		pR = pC;
		boundaryV.x = -uOld.x;
		vMask.x = 0.0;
	}
	if(bB > 0){
		pB = pC;
		boundaryV.y = -uOld.y;
		vMask.y = 0.0;
	}
	if(bT > 0){
		pT = pC;
		boundaryV.y = -uOld.y;
		vMask.y = 0.0;
	}
	
	vec2 gradP = vec2(pR - pL, pT - pB) * 0.5;

	vec2 uNew = uOld - gradP;
	uNew = (vMask * uNew) + boundaryV;

	gl_FragData[0] = vec4((uNew + 1.0) * 0.5, 0.0, 1.0);
}