
uniform vec3 ViewPosition;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

uniform float emissive;
uniform mat4 WorldInverseTranspose;


varying vec3 worldNormal;
varying vec3 worldBinormal;
varying vec3 worldTangent;
varying vec2 texcoords;
varying vec4 gl_FragCoord;

vec4 encodeDepth(float depth)
{
	float d0 = depth;
	float d1 = depth * 256.0;
	float d2 = depth * 256.0 * 256.0;
	float d3 = depth * 256.0 * 256.0 * 256.0;

	vec4 result;

	result.a = mod(d3, 1.0);
	result.r = mod((d2 - result.a / 256.0), 1.0);
	result.g = mod((d1 - result.r / 256.0), 1.0);
	result.b = d0 - result.g / 256.0;
	return result;
}

void main() {
	
	/* Depth value */
	gl_FragData[0] = encodeDepth(gl_FragCoord.z);
	

	/* World normal */

	vec3 N = normalize(worldNormal);
	vec3 T = normalize(worldTangent);
	vec3 B = normalize(worldBinormal);
	vec3 bumpNormal = texture2D(NormalTexture, texcoords).rgb * 2.0 - 1.0;
	vec3 newNormal = vec3(bumpNormal.x * B + bumpNormal.y * T + bumpNormal.z * N); 
	gl_FragData[1].xyz = (newNormal + 1.0)/2.0;
	
	/* Specularity */
	gl_FragData[1].w = texture2D(SpecularTexture,texcoords).r;

	/* Diffuse color */
	gl_FragData[2].xyz = texture2D(DiffuseTexture,texcoords).rgb;
	gl_FragData[2].w = 1.0; 
}
