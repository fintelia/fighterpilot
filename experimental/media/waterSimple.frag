
varying vec2 texCoord;
varying float height;
varying vec3 pos;

uniform sampler2D noise;
uniform sampler2D ground;
uniform sampler2D tex;
uniform vec3 eyePos;
uniform float time;


void main()
{
	float waves=(texture2D(noise,texCoord*30.0).r + texture2D(noise,(texCoord+vec2(time/10000.0,time/10000.0/2.4))*1.53).r + texture2D(noise,(texCoord+vec2(time/10000.0/5.3,time/10000.0))*2.7).r)/3.0;
	float depth=height-texture2DLod(ground,vec2(clamp(texCoord.x,0.0,1.0),clamp(texCoord.y,0.0,1.0)),0.0).r;

	vec4 color=vec4(0.3,0.3,1.0,
		clamp(clamp(depth*3.0-0.05, 0.0, 1.0)*(0.7 + (texCoord.x-0.5)*(texCoord.x-0.5)+(texCoord.y-0.5)*(texCoord.y-0.5) + (gl_FragCoord.z/gl_FragCoord.w*gl_FragCoord.z/gl_FragCoord.w)/(9000.0*9000.0))0.0,1.0)*acos(dot(normalize(eyePos-pos),vec3(0.0,1.0,0.0))));

	color.rgb=(	texture2D(tex,texCoord*10.0).rgb + 
				texture2D(tex,(texCoord+vec2(time/50000.0		,time/50000.0/2.4))*1.53).rgb + 
				texture2D(tex,(texCoord+vec2(time/50000.0/5.3	,time/50000.0))*2.7).rgb)/3.0;
				
	gl_FragColor = color;
}