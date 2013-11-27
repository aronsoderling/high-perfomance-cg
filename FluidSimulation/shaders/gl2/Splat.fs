uniform vec2 invRes;
uniform vec2 pos;
uniform float radius;

uniform sampler2D x;
varying vec4 gl_FragCoord;

void main()
{
	vec2 coord = invRes * gl_FragCoord.xy;

	vec4 color = texture2D(x, coord);
	//vec4 color = vec4(rg.x, rg.y, 0.0, 1.0); 
	
	//vec2 dist = pos - gl_FragCoord.xy;
	
	//float distance = length(dist);
	//if(distance < radius){
		//float r = min(radius - distance, 1.0); 
		//color = vec4(rg.x + r,rg.y + r, 0.0, 1.0);
	//}
	gl_FragData[0] = color;
}