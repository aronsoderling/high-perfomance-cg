uniform float timeStep;
uniform vec2 invRes;
uniform sampler2D velocityTexture;
uniform sampler2D xTexture;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = gl_FragCoord.xy;

	vec2 u = texture2D(velocityTexture, invRes * coord).xy;
	vec2 pos = invRes * (coord - timeStep * u);
	
	gl_FragData[0] = texture2D(xTexture, pos);
}
