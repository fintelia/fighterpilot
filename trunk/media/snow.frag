

varying vec3 position;
varying vec3 sunDir, sunHalfVector;
varying vec3 lightDirections[4];

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

uniform vec3 eyePos;


uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float invLightStrengths[4];

void main()
{
	//if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	//if(!gl_FrontFacing)
	//{
	//	gl_FragColor = vec4(0.0,0.0,0.0,1.0);		not supported on some hardware
	//	return;
	//}

	vec4 groundTexVal = texture2D(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale);
	vec3 v = groundTexVal.xyz;
	vec3 n = normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);
	float slopeAngle = acos(n.y);
	n = normalize( mat3(n,t,b) * texture2D(snow_normals, position.xz*0.00125).xyz*2.0-1.0 );

	//float NdotL = dot(n,lightDir);


//	//////////////////////////////
//	vec4 fractalColor = texture2D(fractalNormals, position.xz*0.0013);
////	color *= mix(1.0, 0.5+0.5*fractalColor.a, rAmount);

//	vec3 positionToEye = eyePos - position;
//	vec3 positionToEye_tangentSpace = mat3(t,n,b)*positionToEye;
//	vec2 offset = -normalize(positionToEye_tangentSpace.xz) * (1.0 - fractalColor.a);

//	fractalColor = texture2D(fractalNormals, (position.xz+offset)*0.0023);
//	n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), 1.0/*rAmount*/));
//	//////////////////////////

//	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ dot(n,vec(0,1,0);//clamp((acos(n.y) - 0.45) / 0.04, 0.0,1.0);
//	color = mix(texture2D(snow,position.xz*0.00125).rgb, vec3(0.15,0.14,0.13)*3.0/*0.5*texture2D(rock,position.xz*0.00125 * 16.0).rgb*/, 0.1/*rAmount*/); //vec4(0.35,0.25,0.20,1.0)


//	vec3 color = vec3(1,1,1);

//	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.06, 0.0,1.0);
	//n = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), normalize((texture2D(fractalNormals, position.xz*0.0023).xyz*2.0 - 1.0)/*+ 0.8*(texture2D(fractalNormals, position.xz*0.000625).xyz*2.0 - 1.0)*/), rAmount));

	vec4 fractalColor = texture2D(fractalNormals, position.xz*0.0023);
	//color.rgb *= mix(1.0, 0.5+0.5*fractalColor.a, rAmount);
	//vec2 offset = vec2(0,0);//(mat3(t,n,b)*normalize(position - eyePos)).xz * (texture2D(fractalNormals, position.xz*0.0023).a - 0.5) * 0.1;

	vec3 positionToEye = eyePos - position;
	vec3 positionToEye_tangentSpace = mat3(t,n,b)*positionToEye;
	vec2 offset = -normalize(positionToEye_tangentSpace.xz) * (1.0 - fractalColor.a);

	fractalColor = texture2D(fractalNormals, (position.xz+offset)*0.0023 * 0.5);
	n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), 1.0));

	////////////////////////
	float rAmount = clamp((1.60 - acos(n.y) + (1.5 - 3.0*groundTexVal.a)) / 0.06 , 0.0,1.0);
	vec3 color = mix(texture2D(snow,position.xz*0.00125).rgb, vec3(0.15,0.14,0.13)*2.0/*0.5*texture2D(rock,position.xz*0.00125 * 16.0).rgb*/,rAmount); //vec4(0.35,0.25,0.20,1.0)
//	color *= mix(1.0, 0.5+0.5*fractalColor.a, rAmount);




	vec3 light = vec3(1,1,1) * max(dot(n,normalize(sunDir))*0.5+0.5, 0.0);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	}

	float r = length(position.xz - vec2(0.5,0.5)) * 2.0;

//	gl_FragColor = //vec4(NdotL*0.7+0.3,NdotL*0.7+0.3,NdotL*0.7+0.3,1.0);
//	vec4(texture2D(snow,position.xz*0.00125).rgb /* (1.1-texture2D(LCnoise,position.xz*21.435).r*0.2)*/ * (light*0.3+0.7),1.0);

	gl_FragColor = vec4(color.rgb * (light*0.45+0.5), 1.0);//vec4(texture2D(snow,position.xz*0.00125).rgb * (light*0.5+0.5),1.0);
}