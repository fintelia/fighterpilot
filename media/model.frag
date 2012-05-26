
varying vec2 texCoord;
varying vec3 normal;

varying vec3 lightDir;
varying vec4 position;

varying vec3 halfVector;

uniform vec4 diffuse;
uniform vec3 specular;
uniform float hardness;

uniform sampler2D tex;

void main()
{

	vec3 Normal = normalize(normal);

	vec4 Diffuse = vec4(diffuse.rgb * texture2D(tex,texCoord).rgb  * max(dot(Normal, lightDir), 0.5),diffuse.a);
	vec4 Specular = vec4(specular,1.0) * pow(max(dot(Normal,normalize(halfVector)),0.0),hardness);
	Specular.a = max(Specular.r ,max(Specular.g, Specular.b));

	gl_FragColor = Diffuse + Specular;
}