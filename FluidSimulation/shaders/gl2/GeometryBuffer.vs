attribute vec3 Vertex;
attribute vec3 Normal;
attribute vec3 Binormal;
attribute vec3 Tangent;
attribute vec3 Texcoord;

uniform mat4 WorldInverseTranspose;
uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

uniform vec2 nearFar;

varying vec3 worldNormal;
varying vec3 worldBinormal;
varying vec3 worldTangent;
varying vec2 texcoords;

void main() {
	worldNormal = NormalMatrix * Normal;
	worldTangent = NormalMatrix * Tangent;
	worldBinormal = NormalMatrix * Binormal;

	texcoords = Texcoord.xy;
   	gl_Position = WorldViewProjection * vec4(Vertex, 1.0);
}
