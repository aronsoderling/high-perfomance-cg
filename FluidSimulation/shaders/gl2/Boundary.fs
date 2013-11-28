uniform vec2 invRes;
uniform sampler2D x;
uniform vec2 offset;
uniform float scale;

varying vec4 gl_FragCoord;

void main()
{
	vec2 coord = gl_FragCoord.xy * invRes;
	vec2 pos = coord + offset * invRes;
	vec2 xvec = (texture2D(x, pos).xy*2.0)-1.0;
	xvec = (xvec*scale+1.0)*0.5;

	gl_FragData[0] = vec4(0.0, 0.0, 1.0, 1.0);
}
