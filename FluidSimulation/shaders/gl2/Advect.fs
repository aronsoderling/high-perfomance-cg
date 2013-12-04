uniform float timeStep;
uniform vec2 invRes;
uniform sampler2D velocityTexture;
uniform sampler2D xTexture;
uniform float dissipation;

varying vec4 gl_FragCoord;
void main()
{
	vec2 coord = invRes * gl_FragCoord.xy;
	vec2 u = (texture2D(velocityTexture, coord).xy * 2.0) - 1.0;
	vec2 pos = coord - timeStep * u;
	
	gl_FragData[0] = vec4(texture2D(xTexture, pos).xy * dissipation, 0.0, 1.0);
}
