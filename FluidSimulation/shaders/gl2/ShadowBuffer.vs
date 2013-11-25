attribute vec3 Vertex;

uniform mat4 WorldViewProjection;

void main() {
    gl_Position = WorldViewProjection * vec4(Vertex, 1.0);
}
