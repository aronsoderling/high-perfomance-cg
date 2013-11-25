

varying vec2 tc;

#define EXTRACT_DEPTH(cc)	((cc).b + (cc).g / 256.0 + (cc).r / (256.0 * 256.0) + (cc).a / (256.0 * 256.0 * 256.0))

void main()
{
	//vec4 diffuseTexture = texture2D(diffuseTextureBuffer, tc);	
	//vec4 light = texture2D(lightBuffer,tc);

	vec3 ambient = vec3(0.0);


	/* Final lighting */
	gl_FragData[0] = vec4(ambient,1.0);

}

