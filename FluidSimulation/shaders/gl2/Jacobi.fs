uniform float alpha;
uniform float beta;

uniform sampler2D x;
uniform sampler2D b;

uniform vec2 invRes;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = invRes * gl_FragCoord.xy;
	vec4 xL = texture2D(x, coords - vec2(invRes.x, 0.0));
	vec4 xR = texture2D(x, coords + vec2(invRes.x, 0.0));
	vec4 xB = texture2D(x, coords - vec2(0.0, invRes.y));
	vec4 xT = texture2D(x, coords + vec2(0.0, invRes.y));

	vec4 bC = texture2D(b, coords);

	gl_FragData[0] = (xL + xR + xB + xT + alpha * bC) * beta;
}
