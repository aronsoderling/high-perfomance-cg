#version 150

in vec3 Vertex;
in vec3 Normal;
in vec2 Texcoord;

uniform mat4 WorldViewProjection;

void main()
{
	gl_Position = WorldViewProjection * vec4(Vertex.xyz, 1.0);
}

