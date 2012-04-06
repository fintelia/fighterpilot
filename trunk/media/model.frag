
varying vec2 texCoord;
varying vec3 normal;
varying vec4 color;
varying vec3 lightDir;
varying vec4 position;

varying vec3 halfVector;

uniform sampler2D tex;

void main()
{
	vec4 Color = vec4(color.rgb * texture2D(tex,texCoord).rgb * max(dot(normalize(normal), lightDir), 0.5),1.0);

	//if(position.y < 0.0)
	//	discard;
	//Color.a *= 1.0 + clamp(position.y/fwidth(position.y),-1.0,0.0)*(position.y*0.1);
	//if(Color.a > 0.99)
		//gl_FragDepth = gl_FragCoord.z;
	//else
		//gl_FragDepth = 1.0;


	//specular highlights (doesn't look good on all models)
	//Color.rgb += vec3(0.5, 0.5, 0.35) * pow(max(dot(normal,normalize(halfVector)),0.0),40.0);


	gl_FragColor = Color;
}