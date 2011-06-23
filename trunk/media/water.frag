varying vec2 texCoord;
uniform sampler2D noise;
uniform sampler2D ground;
uniform sampler2D tex;
varying float height;
varying vec3 pos;
uniform float time;
uniform vec3 eyePos;

void main()
{
	float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>14000.0)discard;
	float waves=(texture2D(noise,texCoord*30.0).r + texture2D(noise,(texCoord+vec2(time/10000.0,time/10000.0/2.4))*1.53).r + texture2D(noise,(texCoord+vec2(time/10000.0/5.3,time/10000.0))*2.7).r)/3.0;
	float depth=height-texture2DLod(ground,vec2(clamp(texCoord.x,0.0,1.0),clamp(texCoord.y,0.0,1.0)),0.0).r;
	float radius=sqrt((texCoord.x-0.5)*(texCoord.x-0.5)+(texCoord.y-0.5)*(texCoord.y-0.5));

	vec4 color=vec4(0.3,0.3,1.0,1.0);

	color.a = clamp(depth*3.0-0.05, 0.0, 1.0)*(0.7 + radius*radius + (dist*dist)/(9000.0*9000.0));
	color.a = clamp(color.a,0.0,1.0);

	//color.rg *= waves*0.5 + radius * 0.5;
	//color.b  *= clamp(radius*0.3+0.7,0.0,1.0);
	float t=time/50000.0;//time*(depth*depth)/10000.0;
	color.rgb=(	texture2D(tex,texCoord*10.0).rgb + 
				texture2D(tex,(texCoord+vec2(t		,t/2.4))*1.53).rgb + 
				texture2D(tex,(texCoord+vec2(t/5.3	,t))*2.7).rgb)/3.0;
	//color.rgb=(texture2D(tex,texCoord*7.0).rgb+;
	//color.rgb=vec3(depth,depth,depth);
	//if(dist>9000.0) color.a*=1.0-(dist-9000.0)/5000.0;
	
	//color.a*=acos(dot(normalize(eyePos-pos),vec3(0.0,1.0,0.0)));
	color.a = (1.0 - (1.0 - color.a) * dot(normalize(eyePos-pos),vec3(0.0,1.0,0.0)) );
	gl_FragColor = color;
}