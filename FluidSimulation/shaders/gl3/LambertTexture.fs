#version 150

/* diffuse texture sampler
*/
uniform sampler2D DiffuseTex;

uniform vec3 Ambient;

in vec2 texcoord;
in vec3 diffuse_color;

out vec4 fragColor;

void main() {

	/* texture lookup
    */
	vec3 diffuse = diffuse_color * texture2D(DiffuseTex, texcoord).xyz;
	vec4 color = vec4(Ambient + diffuse, 1.0);

	/* write final color to buffer
    */
	fragColor = color;

}
