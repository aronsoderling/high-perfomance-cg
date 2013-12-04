uniform vec2 invRes;
uniform sampler2D copyTexture;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes*gl_FragCoord.xy;
	gl_FragData[0] = texture2D(copyTexture, coord);
}
