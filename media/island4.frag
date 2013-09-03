// fragment shader
#version 400

//in VertexData{
//	vec3 worldPosition;
//	vec3 sunDir, sunHalfVector;
//	vec3 lightDirections[4];
//}teData;
struct VertexData{
	vec3 worldPosition;
	vec3 sunDir, sunHalfVector;
	vec3 lightDirections[4];
};
in VertexData teData;
in vec2 teNormalMapCoord;

uniform float time;

uniform samplerCube sky;
uniform sampler2D groundTex;
uniform sampler2D LCnoise;
uniform sampler2D noiseTex;
uniform sampler2D sand;
uniform sampler2D sand2;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D grass_normals;
uniform sampler2D snow_normals;
uniform sampler2D fractalNormals;
uniform sampler2D treesTexture;

uniform sampler2D grassDetail;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float invLightStrengths[4];

uniform vec3 eyePos;

uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform vec2 ttex_halfPixel;
uniform vec2 ttex_origin;
uniform vec2 ttex_invScale;

uniform float minHeight;
uniform float heightRange;

out vec4 FragColor;
in vec3 teNormal;
in vec3 teColor;

void main()
{
	vec3 diffuse;
	vec3 specular = vec3(0,0,0);
	vec3 position = teData.worldPosition;
	vec3 sunDir = teData.sunDir;

	/////////FRAGMENT ATRRIBUTES////////
	vec4 groundTexVal = texture(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale);
	vec3 v = groundTexVal.xyz;
	vec3 n = teNormal;//normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);


	
	float slopeAngle = acos(n.y);
	float slope = sqrt(1.0/(n.y*n.y) - 1.0);
	
	vec4 fractalColor = texture(fractalNormals, teNormalMapCoord);
	n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), clamp((slopeAngle - 0.40) / 0.06, 0.0,1.0)/*clamp((1.0 - n.y)*20.0,0.0,1.0)*/));
	
	float height = minHeight + groundTexVal.a * heightRange;

	///////////EARLY DISCARD//////////
	if(height < -45.0)
		discard;

	///////////GRASS AND SAND//////////
	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	//float gAmount = pow(clamp(noiseVal + (height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	float gAmount = pow(clamp((noiseVal + (height-6.0)/20.0 - 1.5), 0.0, 1.0), 0.5);
	//gAmount = 0.5-0.5*cos(gAmount * 3.141592);
	diffuse = mix(texture2D(sand,position.xz*0.0003).rgb, mix(vec3(1,1,1), texture2D(grassDetail, position.xz*0.042).rgb * 2.0,clamp(gAmount,0.0,1.0)) * texture2D(grass,position.xz*0.00125 *4.0).rgb/* / texture2D(grassDetail, position.xz*0.0532,3.0)*/, gAmount);

	////////////TREE CANOPY////////////
	//diffuse *= 1.0 - 0.6 * clamp((height - 37.0)*0.2, 0.0, 1.0) * max(clamp((148.0-height)*0.2,0.0,1.0),clamp((slopeAngle - 0.1) / 0.1, 0.0, 1.0));
	
	//////////////ROCK/////////////////
	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.06, 0.0,1.0);
	diffuse = mix(diffuse, vec3(0.15,0.14,0.13), rAmount);
	specular = vec3(0.1 * rAmount);
//	vec4 fractalColor = texture(fractalNormals, teNormalMapCoord);
//	n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), rAmount));
//	color.rgb *= mix(1.0, 0.7+0.3*fractalColor.a, rAmount);
	
	
	///////////////LIGHT///////////////
//	sunDir = vec3(0,1,0);
	
	vec4 treesTextureColor = texture(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale); //tree shadows
	vec3 light = vec3(1,1,0.7) * (max(dot(n,normalize(sunDir))*0.5+0.5 - treesTextureColor.a*0.3, 0.0));
	specular = vec3(0.025 * rAmount * min(pow(max(dot(n,normalize(teData.sunHalfVector)),0.0),8.0), 1.0));
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	}

	////////////WATER EFFECT///////////
	float waterAlpha = clamp(1.0 + clamp((-height)/fwidth(height),1.0,0.0)*(height*0.03), 0.0,1.0);
	light *= waterAlpha;
	//color.a *= waterAlpha;
	
	////////////////FOG////////////////
	vec3 eyeDirection = position.xyz-eyePos;
	//diffuse = vec3(mix(diffuse*max(light,0.0), texture(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)));
	//diffuse = vec3(mix(diffuse*max(light,0.0), vec3(0.5,0.5,0.5), clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,0.75)));
	diffuse = vec3(mix(diffuse*max(light,0.0), vec3(0.5,0.5,0.5), clamp(1.0 - 100000000.0 / dot(eyeDirection,eyeDirection),0.0,0.75)));
	
	FragColor = vec4(diffuse + specular, waterAlpha);
}