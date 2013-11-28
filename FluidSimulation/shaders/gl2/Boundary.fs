uniform vec2 invRes;
uniform sampler2D x;

varying vec4 gl_FragCoord;

void main()
{
	vec2 coord = gl_FragCoord.xy * invRes;

	vec2 u = (texture2D(velocityTexture, invRes * coord).xy * 2.0) - 1.0;
	vec2 pos = invRes * (coord - timeStep * u);
	
	gl_FragData[0] = vec4(texture2D(xTexture, pos).xy, 0.0, 1.0);
}

void boundary(half2 coords : WPOS,    // grid coordinates
              
   half2 offset : TEX1,    // boundary offset
              
   out half4 bv : COLOR,   // output value
              
   uniform half scale,     // scale parameter
              
   uniform samplerRECT x)  // state field
{
  bv = scale * h4texRECT(x, coords + offset);
}