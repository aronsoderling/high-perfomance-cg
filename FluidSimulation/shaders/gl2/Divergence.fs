uniform vec2 invRes;

uniform sampler2D w;
uniform sampler2D b;

varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = gl_FragCoord.xy * invRes;
	vec2 wL = texture2D(w, coords - vec2(invRes.x, 0.0)).xy * 2.0 - 1.0;
	vec2 wR = texture2D(w, coords + vec2(invRes.x, 0.0)).xy * 2.0 - 1.0;
	vec2 wB = texture2D(w, coords - vec2(0.0, invRes.y)).xy * 2.0 - 1.0;
	vec2 wT = texture2D(w, coords + vec2(0.0, invRes.y)).xy * 2.0 - 1.0;
	vec2 wC = texture2D(w, coords).xy * 2.0 - 1.0;

	float bL = texture2D(b, coords - vec2(invRes.x, 0.0)).x;
	float bR = texture2D(b, coords + vec2(invRes.x, 0.0)).x;
	float bB = texture2D(b, coords - vec2(0.0, invRes.y)).x;
	float bT = texture2D(b, coords + vec2(0.0, invRes.y)).x;
	
	if(bL > 0){
		wL = -wC;
	}
	if(bR > 0){
		wR = -wC;
	}
	if(bB > 0){
		wB = -wC;
	}
	if(bT > 0){
		wT = -wC;
	}
	
	float r = 0.5 *(wR.x - wL.x + wT.y - wB.y);

	gl_FragData[0] = vec4((r + 1.0) * 0.5, 0.0, 0.0, 1.0);
}