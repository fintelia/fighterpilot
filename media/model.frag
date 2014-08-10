
varying vec2 texCoord;
varying float flogz;
//varying vec3 normal;
//varying vec3 tangent;

centroid varying vec4 position;
centroid varying vec3 lightDir;
centroid varying vec3 halfVector;

uniform vec4 diffuse;
uniform vec3 specular;
uniform float hardness;

uniform sampler2D tex;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

varying vec3 normal;
varying vec3 eyeDirection;

void main()
{
	vec4 Specular, Diffuse;
	vec3 Normal, sunDir;

	//Normal = texture2D(normalMap,texCoord).xyz;
	//Normal.xy = Normal.xy * 2.0 - 1.0;
	Normal = normalize(normal);//vec3(0,1,0);
	sunDir = normalize(lightDir);


	float NdotL = max(dot(Normal, sunDir)*0.5+0.5,0.0);

	Diffuse = vec4(diffuse.rgb * texture2D(tex,texCoord).rgb  * min(NdotL+0.25,1.0),diffuse.a * texture2D(tex,texCoord).a);
	//Specular.rgb = specular * texture2D(specularMap,texCoord).rgb * min(pow(max(dot(Normal,normalize(halfVector)),0.0),hardness), 1.0);
	Specular.rgb = specular * texture2D(specularMap,texCoord).rgb * vec3(pow(max(0.0, dot(reflect(-sunDir, Normal), normalize(eyeDirection))), hardness));
	Specular.a =  max(Specular.r, max(Specular.g, Specular.b));

	gl_FragColor = /*vec4(vec3(NdotL),1);//*/Diffuse + Specular;

	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(2000000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	gl_FragDepth = log2(flogz) * Fcoef_half;
	
}
