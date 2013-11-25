uniform vec2 invRes;
uniform vec2 pos;
uniform float radius;

varying vec4 gl_FragCoord;
void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec2 dist = pos - gl_FragCoord.xy;
	float distance = length(dist);
	if(distance < radius){
		float alpha = 1.0;//length(normalize(dist));
		color = vec4(0.8, 0.8, 0.8, alpha);
	}

	gl_FragData[0] = color;
}