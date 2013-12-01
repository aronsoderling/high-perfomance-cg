uniform float timeStep;
uniform vec2 invRes;
uniform sampler2D v;
uniform sampler2D t;
uniform sampler2D d;


varying vec4 gl_FragCoord;
void main()
{
	float sigma = 1.0;
	float kappa = 0.05;
	float ambientT = 0.0;
	vec2 coord = invRes * gl_FragCoord.xy;

	vec2 V = texture2D(v, coord).xy * 2.0 - 1.0;
	float T = texture2D(t, coord).x * 2.0 - 1.0;
	vec2 color = V;

	if(T > 0.5){
		float D = texture2D(d, coord).x * 2.0 - 1.0;
		color += (timeStep *(T - ambientT) * sigma - D * kappa) * vec2(0.0, 1.0);
	}
	vec4 colorOut = vec4((color.xy + 1.0)*0.5, 0.0, 1.0);
	gl_FragData[0] = colorOut;
}
