
varying vec2 modelPosition;	//length will be at least 0.995 (assuming disk has 64 sides)
varying vec3 worldPosition;
varying vec2 texCoord;

varying vec3 lightDirection;
varying vec3 eyeDirection;

uniform sampler3D oceanNormals;
uniform samplerCube sky;
uniform float time;


uniform sampler2D groundTexture;
uniform vec2 invGroundScale;
uniform vec2 halfPixelOffset;
uniform float groundHeightScale;
uniform float texSeaLevel;

//float height(vec3 textureCoordinates)
//{
//	return 0.5*texture3D(oceanNormals,textureCoordinates).w + 0.3*texture3D(oceanNormals,vec3(textureCoordinates.xy*7.345, textureCoordinates.z)).w + 0.2*texture3D(oceanNormals,vec3(textureCoordinates.xy*20.345, textureCoordinates.z)).w;
//}

//vec3 normal(vec3 textureCoordinates, vec2 e)
//{
//	float h0 = height(textureCoordinates);
//	float hx = height(textureCoordinates+vec3(e.x,0,0));
//	float hz = height(textureCoordinates+vec3(0,e.y,0));

//	return normalize(vec3((h0-hx)/e.x, 1.0, (h0-hz)/e.y));
//}

//see: http://maverick.inria.fr/Publications/2006/BD06a/water06.pdf

void main()
{
	vec3 n;
	vec3 color;
	float NdotL;

	n = texture3D(oceanNormals,vec3(texCoord.xy*6.234,0.0002*time)).xyz * 0.30 + texture3D(oceanNormals,vec3(texCoord.xy * 64.0,0.0003*time)).xyz * 0.45 + texture3D(oceanNormals,vec3(texCoord.xy * 512.0,0.0005*time)).xyz * 0.25; //must sum to 1
	n.xz = n.xz * 2.0 - vec2(1.0,1.0);
	n = normalize(n);

	NdotL = dot(n,normalize(lightDirection));

//	float power = 2.0;
//	float bias = 0.2;
//	float fresnel = clamp(bias + (1.0-bias)*pow(1.0 - NdotL, power), 0.0,1.0); //0.0 = bias; 5.0 = power <--- these change the water reflections

	float cosAngle = dot(n, normalize(eyeDirection));
	float R0 = 0.2;
	float fresnel = clamp(R0 + (1.0-R0)*pow(1.0-cosAngle, 5.0), 0.0, 1.0);

	vec3 r = reflect(-normalize(eyeDirection),n);
	r.y = max(r.y,0.0);
	vec3 cReflect = textureCube(sky, r).rgb;

	const vec3 darkColor = vec3(0.08, 0.24, 0.26);
	const vec3 lightColor = vec3(0.12, 0.26, 0.55);
	const vec3 groundColor = vec3(0.482,0.337,0.227);
							   //texture2D(groundTexture, (worldPosition.xz - halfPixelOffset)*invGroundScale).a; //
	float depth = (texSeaLevel - texture2D(groundTexture, (worldPosition.xz)*invGroundScale - halfPixelOffset).a)*groundHeightScale;



//	vec3 cRefract = mix(groundColor, darkColor, pow()

//	color = mix(darkColor, lightColor, fresnel);
//	color = mix(color, cReflect, fresnel * 0.3 /* *shorelineAlpha*/);//   *   (0.6 + 0.4 * NdotL);

//	color = mix(darkColor, groundColor, vec3(pow(3, -depth/7), pow(3, -depth/30), pow(3, -depth/70)));
	color = mix(darkColor, cReflect, fresnel * 0.20 * clamp(0.02*(depth-20.0),0.0,1.0)) * (0.9 + 0.1 * NdotL);
	color = mix(color, textureCube(sky, vec3(modelPosition.x,0,modelPosition.y)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection), 0.0, 1.0));

	gl_FragColor = vec4(vec3(color),1.0);
}
