varying vec4 gl_FragCoord;
vec4 encodeDepth(float depth)
{
	float d0 = depth;
	float d1 = depth * 256.0;
	float d2 = depth * 256.0 * 256.0;
	float d3 = depth * 256.0 * 256.0 * 256.0;

	vec4 result;

	result.a = mod(d3, 1.0);
	result.r = mod((d2 - result.a / 256.0), 1.0);
	result.g = mod((d1 - result.r / 256.0), 1.0);
	result.b = d0 - result.g / 256.0;
	return result;
}
void main() {
	/* Write depth to shadowmap */
	gl_FragData[0] = encodeDepth(gl_FragCoord.z);

}
