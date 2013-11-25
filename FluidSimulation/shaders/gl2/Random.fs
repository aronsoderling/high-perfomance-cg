uniform vec2 invRes;
varying vec4 gl_FragCoord;

void main()
{
	vec2 xy = gl_FragCoord.xy * invRes;
	gl_FragData[0] = vec4(xy.x, xy.y, 0.0, 1.0);
}
