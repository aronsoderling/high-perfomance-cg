attribute vec3 Vertex;

uniform mat4 WorldViewProjection;
uniform mat4 WorldView;
uniform mat4 World;
uniform vec2 nearFar;

varying vec3 worldPos;

void main()
{
    worldPos = (World * vec4(Vertex, 1.0)).xyz;
    gl_Position = WorldViewProjection * vec4(Vertex, 1.0);
}


