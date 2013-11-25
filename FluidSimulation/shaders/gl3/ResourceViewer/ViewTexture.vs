#version 150

in vec2 Vertex;
out vec2 vtx;

void main()
{
	vtx = Vertex.xy * 0.5 + 0.5;
	gl_Position = vec4(Vertex.xy, 0.0, 1.0);
}


