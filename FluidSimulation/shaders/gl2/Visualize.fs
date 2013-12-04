uniform vec2 invRes;
uniform sampler2D visualizeTexture;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes*gl_FragCoord.xy;
	float r = texture2D(visualizeTexture, coord).r - 0.5;

	gl_FragData[0] = vec4(r, r, r, 1.0);
}
