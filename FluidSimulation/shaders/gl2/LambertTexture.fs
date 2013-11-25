
/* diffuse texture sampler
*/
uniform sampler2D DiffuseTex;

uniform vec3 Ambient;

varying vec2 texcoord;
varying vec3 diffuse_color;


void main() {

	/* texture lookup
    */
	vec3 diffuse = diffuse_color * texture2D(DiffuseTex, texcoord).xyz;
	vec4 color = vec4(Ambient + diffuse, 1.0);

	/* write final color to buffer
    */
	gl_FragColor = color;

}
