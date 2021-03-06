
/* Per vertex Lambert shader + texturing */

/* variables per vertex
*/
attribute vec3 Vertex;
attribute vec3 Normal;
attribute vec3 Texcoord;

/* variables per primitive
*/
uniform mat4 World;					/* transform to world space */
uniform mat4 WorldViewProjection;	/* transform to screen space */
uniform mat4 WorldInverseTranspose;	/* transform to world space (normals) */

uniform vec3 ViewPosition;
uniform vec3 LightPosition;
uniform vec3 LightColor;

/* 'varying' variables are interpolated over the triangle
	and available per pixel in the fragment shader
 */
varying vec2 texcoord;
varying vec3 diffuse_color;


void main() {

	vec3 worldPosition = (World * vec4(Vertex.xyz, 1.0)).xyz;
	vec3 worldNormal = (WorldInverseTranspose * vec4(Normal.xyz, 1.0)).xyz;
	vec3 viewVector = normalize(ViewPosition - worldPosition);

	/* diffuse shading
	*/
	vec3 lightVector = normalize(LightPosition - worldPosition);
	float lightNormalCos = max(0.0, dot(normalize(worldNormal), lightVector));
	diffuse_color = LightColor * lightNormalCos;

	texcoord = vec2(Texcoord.x, Texcoord.y);

	/* set the final position for this vertex
	*/
	gl_Position = WorldViewProjection * vec4(Vertex.xyz, 1.0);

}



