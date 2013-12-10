uniform float iBeta;

uniform sampler2D p;
uniform sampler2D d;
uniform sampler2D b;

uniform vec2 invRes;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = invRes * gl_FragCoord.xy;
	float pL = texture2D(p, coords - vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float pR = texture2D(p, coords + vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float pB = texture2D(p, coords - vec2(0.0, invRes.y)).x * 2.0 - 1.0;
	float pT = texture2D(p, coords + vec2(0.0, invRes.y)).x * 2.0 - 1.0;
	float pC = texture2D(p, coords).x * 2.0 - 1.0;

	float bL = texture2D(b, coords - vec2(invRes.x, 0.0)).x;
	float bR = texture2D(b, coords + vec2(invRes.x, 0.0)).x;
	float bB = texture2D(b, coords - vec2(0.0, invRes.y)).x;
	float bT = texture2D(b, coords + vec2(0.0, invRes.y)).x;
	
	if(bL > 0){
		pL = pC;
	}
	if(bR > 0){
		pR = pC;
	}
	if(bB > 0){
		pB = pC;
	}
	if(bT > 0){
		pT = pC;
	}
	
	float dC = texture2D(d, coords).x * 2.0 - 1.0;

	float c = (pL + pR + pB + pT - dC) *0.25;
	c = (c + 1.0) * 0.5;

	gl_FragData[0] = vec4(c, 0.0, 0.0, 1.0);
}
