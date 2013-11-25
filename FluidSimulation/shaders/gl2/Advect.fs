uniform float timeStep;
uniform vec2 invRes;
uniform sampler2D velocityTexture;
uniform sampler2D xTexture;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes * gl_FragCoord.xy;

	vec2 pos = coord - timeStep *invRes*texture2D(velocityTexture, coord).xy; 

	gl_FragData[0] = texture2D(xTexture, pos);
}
