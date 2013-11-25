#version 120

attribute vec3 Vertex;
//attribute vec3 Normal;

uniform mat4 WorldViewProjection;
uniform vec3 minPos;
uniform vec3 maxPos;
uniform int hasNormals;
uniform float faded;

varying vec3 color;

void main() {
	gl_Position = WorldViewProjection * vec4(Vertex.xyz, 1.0);
//	if (hasNormals != 0)
//		color = Normal.xyz * 0.5 + 0.5;
//	else
		color = (maxPos - Vertex) / (maxPos - minPos);
	color.xyz *= faded;
		
}

