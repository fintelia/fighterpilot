
varying vec3 position;
varying vec3 sunDir;

uniform float time;
uniform float heightScale;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

uniform vec3 invScale;

uniform float minHeight;
uniform float heightRange;

void main()
{
	vec4 color;

	vec4 groundTexVal = texture2D(groundTex,position.xz * invScale.xz);
	vec3 n = normalize(vec3(groundTexVal.x * 2.0 - 1.0, groundTexVal.y/heightScale, groundTexVal.z * 2.0 - 1.0));

	//float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;

	float slopeAngle = acos(n.y);
	float slope = sqrt(1.0/(n.y*n.y) - 1.0);
	float height = minHeight + groundTexVal.a * heightRange;

	if(height < -0.001)
		discard;

	///////////GRASS AND SAND//////////
	float gAmount = pow(clamp((height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	color = mix(texture2D(sand,position.xz*0.0003), texture2D(grass,position.xz*0.00125), gAmount);

	////////////TREE CANOPY////////////
	color.rgb *= 1.0 - 0.3 * clamp((height - 37.0)*0.2, 0.0, 1.0) * max(clamp((148.0-height)*0.2,0.0,1.0),clamp((slopeAngle - 0.1) / 0.1, 0.0, 1.0));
	//color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);

	//////////////ROCK/////////////////
	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.10, 0.0,1.0);
	color = mix(color, /*vec4(0.35,0.25,0.20,1.0)*/texture2D(rock,position.xz*0.00125), rAmount);


	//////////////LIGHT/////////////////
	float NdotL = dot(n,normalize(sunDir));
	color = vec4(color.rgb*(NdotL*0.4+0.6),1.0);

	gl_FragColor = color;//* (0.9 + clamp(NdotL*0.5,0.0,0.5));
}