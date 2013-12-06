uniform vec2 invRes;
uniform sampler2D densityTexture;
uniform sampler2D temperatureTexture;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes*gl_FragCoord.xy;
	float dens = texture2D(densityTexture, coord).r;
	float temp = texture2D(temperatureTexture, coord).r;

	gl_FragData[0] = vec4(dens.r*temp.r, dens.r*temp.r, dens.r*temp.r, 1.0);
}
