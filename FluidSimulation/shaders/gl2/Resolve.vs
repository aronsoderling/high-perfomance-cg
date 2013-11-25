
attribute vec2 Vertex;

varying vec2 tc;

void main()
{
	tc = 0.5 + Vertex * 0.5;
	gl_Position = vec4(Vertex.xy, 0.0, 1.0);
}


