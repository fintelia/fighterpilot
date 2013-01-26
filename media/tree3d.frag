
varying vec2 texCoord;
//varying vec3 normal;
//varying vec3 tangent;

varying vec4 position;
//centroid varying vec3 lightDir;
//centroid varying vec3 halfVector;

uniform float transparency;
//uniform vec3 specular;
//uniform float hardness;

uniform sampler2D tex;
//uniform sampler2D specularMap;
//uniform sampler2D normalMap;

void main()
{
	//vec4 Specular, Diffuse;
	//vec3 Normal;
	//
	//Normal = texture2D(normalMap,texCoord).xyz;
	//Normal.xy = Normal.xy * 2.0 - 1.0;
	//
	//float NdotL = max(dot(normalize(Normal), normalize(lightDir))*0.5+0.5,0.0);
	//Diffuse = vec4(diffuse.rgb * texture2D(tex,texCoord).rgb  * min(NdotL+0.25,1.0),diffuse.a * texture2D(tex,texCoord).a);
	//Specular.rgb = specular * texture2D(specularMap,texCoord).rgb * min(pow(max(dot(Normal,normalize(halfVector)),0.0),hardness), 1.0);
	//
	//Specular.a = max(Specular.r, max(Specular.g, Specular.b));

	vec4 color = texture2D(tex,texCoord).rgba;
	gl_FragColor = vec4(color.rgb*color.a,color.a) * transparency;//vec4((Diffuse + Specular).rgb,texture2D(tex,texCoord).a);
}
