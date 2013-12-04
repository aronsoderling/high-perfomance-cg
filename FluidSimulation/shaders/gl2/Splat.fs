uniform vec2 invRes;
uniform vec2 pos;
uniform float radius;
uniform float f;

uniform sampler2D x;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coord = invRes * gl_FragCoord.xy;
	vec2 c = texture2D(x, coord).xy;
	vec4 color = vec4(c.x, c.y, 0.0, 1.0);
	
	float distance = distance(pos, gl_FragCoord.xy); 
	if(distance < radius){
		color = vec4(c.x + 0.5 *(f/distance), c.y, 0.0, 1.0); 
	}
	gl_FragData[0] = vec4(color.xy, 0.0, 1.0);
}