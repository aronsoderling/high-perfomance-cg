#version 150

uniform vec3 Ambient;
in vec3 diffuse_color;
out vec4 fragColor;

void main() {

	/* write final color to buffer
    */
	fragColor = vec4(Ambient.xyz + diffuse_color, 1.0);
}
