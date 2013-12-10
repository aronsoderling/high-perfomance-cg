uniform float timeStep;
uniform vec2 invRes;
uniform float dissipation;
uniform vec4 baseColor;
uniform float k;

uniform sampler2D v;
uniform sampler2D x;
uniform sampler2D b;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes * gl_FragCoord.xy;
	float border = texture2D(b, coord).x;
	vec4 xOut = baseColor;

	if(border == 0){
		vec2 u = (texture2D(v, coord).xy * 2.0) - 1.0;
		vec2 pos = coord - timeStep * u;
		xOut.xy = texture2D(x, pos).xy * dissipation;
	}
	xOut.x = xOut.x + k;
	gl_FragData[0] = xOut;
}
