
varying vec2 texCoord;
varying vec3 normal;

varying vec3 lightDir;
varying vec4 position;

varying vec3 halfVector;

uniform vec4 diffuse;
uniform vec3 specular;
uniform float hardness;

uniform sampler2D tex;
uniform sampler2D specularMap;


void main()
{
	vec4 Specular, Diffuse;
	vec3 Normal;

	Normal = normalize(normal);
	Diffuse = vec4(diffuse.rgb * texture2D(tex,texCoord).rgb  * max(dot(Normal, lightDir), 0.5),diffuse.a);
	Specular.rgb = specular * texture2D(specularMap,texCoord).rgb * pow(max(dot(Normal,normalize(halfVector)),0.0),hardness);
	Specular.a = max(Specular.r, max(Specular.g, Specular.b));

	gl_FragColor = Diffuse + Specular;
}