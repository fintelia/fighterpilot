
//uniform sampler2D tex;
uniform sampler2D tex;
uniform sampler2D noiseTex;
varying float g;
varying vec2 texCoord;

uniform float time;

void main()
{
	vec3 color = texture2D(tex, texCoord).rgb;
	float intensity = dot(color.rgb,vec3(0.2989, 0.5870, 0.1140));

	//if(intensity < 0.2)
	//{
	//	color *= 4.0;
	//}
  
	vec3 n = texture2D(noiseTex, texCoord*0.85 + vec2(0.4*sin(time*0.05),0.4*cos(time*0.05))).rgb;

    vec3 visionColor = vec3(0.1, 0.95, 0.2);
    color = 0.7 * ((vec3(intensity) + n*0.15) * vec3(0.1, 0.95, 0.2) + pow(intensity,5.0)*3.5);

	color = pow(color,vec3(g,g,g));
	gl_FragColor = vec4(color, 1.0);
}