uniform float iBeta;

uniform sampler2D x;
uniform sampler2D b;

uniform vec2 invRes;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coords = invRes * gl_FragCoord.xy;
	vec2 xL = texture2D(x, coords - vec2(invRes.x, 0.0)).xy * 2.0 - 1.0;
	vec2 xR = texture2D(x, coords + vec2(invRes.x, 0.0)).xy * 2.0 - 1.0;
	vec2 xB = texture2D(x, coords - vec2(0.0, invRes.y)).xy * 2.0 - 1.0;
	vec2 xT = texture2D(x, coords + vec2(0.0, invRes.y)).xy * 2.0 - 1.0;

	float bC = texture2D(b, coords).x * 2.0 - 1.0;

	gl_FragData[0] = vec4(((((xL + xR + xB + xT - bC) * iBeta) + 1.0) / 2.0).xy, 0.0, 1.0);
}
