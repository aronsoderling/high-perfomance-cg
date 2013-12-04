uniform float iBeta;

uniform sampler2D x;
uniform sampler2D b;

uniform vec2 invRes;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = invRes * gl_FragCoord.xy;
	float xL = texture2D(x, coords - vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float xR = texture2D(x, coords + vec2(invRes.x, 0.0)).x * 2.0 - 1.0;
	float xB = texture2D(x, coords - vec2(0.0, invRes.y)).x * 2.0 - 1.0;
	float xT = texture2D(x, coords + vec2(0.0, invRes.y)).x * 2.0 - 1.0;

	float bC = texture2D(b, coords).x * 2.0 - 1.0;

	float c = (xL + xR + xB + xT - bC) *0.25;
	c = (c + 1.0) * 0.5;

	gl_FragData[0] = vec4(c, 0.0, 0.0, 1.0);
}
