
uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;


uniform sampler2D shadowMap;

uniform vec2 invRes;

uniform mat4 ViewProjectionInverse;
uniform mat4 ViewProjection;
uniform vec3 ViewPosition;
uniform mat4 shadowViewProjection;

uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 LightDirection;
uniform float LightIntensity;
uniform float LightAngleFalloff;

uniform vec2 ShadowMapTexelSize;

varying vec4 gl_FragCoord;


#define EXTRACT_DEPTH(cc)	((cc).b + (cc).g / 256.0 + (cc).r / (256.0 * 256.0) + (cc).a / (256.0 * 256.0 * 256.0))

void main()
{

//calculate texture coordinates
vec2 texCoords = invRes * gl_FragCoord.xy;

//extract normal and depth values from buffers
vec3 normal = texture2D(normalAndSpecularBuffer,texCoords).rgb * 2.0 - 1.0;
float depth = EXTRACT_DEPTH(texture2D(depthBuffer,texCoords));
float ks = texture2D(normalAndSpecularBuffer,texCoords).a;


//perform inverse projection to obtain world space position
float x = texCoords.x * 2.0 - 1.0; //[0,1]
float y = texCoords.y * 2.0 - 1.0; //[0,1]
float z = depth * 2.0 - 1.0; //[0,1]
vec4 pixelWorldPosBeforeDiv = ViewProjectionInverse * vec4(x, y, z, 1.0); //[world]
vec3 pixelWorldPos = pixelWorldPosBeforeDiv.xyz/pixelWorldPosBeforeDiv.w; //[world perspective correct]

//project onto shadowViewProjection and compare with extracted shadow depth
vec4 shadowPixel = shadowViewProjection * vec4(pixelWorldPos.xyz, 1.0); // from world to shadowCamera [proj]
shadowPixel = shadowPixel / shadowPixel.w;  //[-1, 1]
vec2 shadowTextureCoord = (shadowPixel.xy + 1.0)/2.0; //[0, 1]
float shadowDepth = EXTRACT_DEPTH(texture2D(shadowMap, shadowTextureCoord))* 2.0 - 1.0; //[-1, 1]

float shadow = 1.0;
float shadowOffsetDepth = 0.0;
vec2 shadowTextureOffset = shadowTextureCoord; //[0, 1]
float shadowSum = 0.0;

for(float i = -1.5; i<=1.5; i+=1.0){
	shadowTextureOffset.x = shadowTextureCoord.x + ShadowMapTexelSize.x * i; //[0, 1]
	for(float j = -1.5; j<=1.5; j+=1.0){
		shadowTextureOffset.y = shadowTextureCoord.y + ShadowMapTexelSize.y * j; //[0, 1]
		shadowOffsetDepth = EXTRACT_DEPTH(texture2D(shadowMap, shadowTextureOffset))* 2.0 - 1.0; //[-1, 1]
		if(shadowOffsetDepth < shadowPixel.z){
			shadowSum += 1.0;
		}
	}
}
shadow = 1.0 - shadowSum/16.0;

//compute vectors
vec3 N = normalize(normal);
vec3 L = normalize(LightPosition-pixelWorldPos);
vec3 V = normalize(ViewPosition-pixelWorldPos);
vec3 R = normalize(reflect(-L,N));

//compute diffuse and specular
float diffuse = max(dot(L,N), 0.0);
float specular = ks * pow(max(dot(V,R), 0.0),33.0);


//compute angular falloff
float maxAngle = LightAngleFalloff;
float angle = dot(-L, normalize(LightDirection));
float angleFalloff = 0.0;
if(angle > maxAngle){
	angleFalloff = 1.0 - (1.0 - angle) * 1.0/(1.0 - maxAngle);
}

//compute distance falloff
float distance = dot(LightPosition-pixelWorldPos, LightPosition-pixelWorldPos);
float distanceFalloff = 1.0;// min(40.0/distance, 1.0);

//compute final color
vec3 color = LightColor * (diffuse + specular);

// Output final color
gl_FragData[0].xyz =  color * distanceFalloff  * angleFalloff * shadow;
gl_FragData[0].w = 1.0;
}

