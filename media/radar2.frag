
uniform float radarAng;
varying vec2 texCoord;
uniform sampler2D backgroundTexture;
uniform sampler2D radarTexture;
void main()
{
	vec4 color;
	float intensity;
	vec2 loc=texCoord-vec2(0.5,0.5);
	float myDist=sqrt(loc.y*loc.y+loc.x*loc.x)*360.0;
	
	float speed=2.0;

	float dif=(myDist-radarAng*speed);
	intensity=clamp(sin(radians(dif*3.0)),0.0,1.0)*0.2;
	
	float tmp=radarAng*speed-myDist;
	if(tmp<0.0)
		tmp+=360.0;
	if(tmp>=360.0)
		tmp-=360.0;
	if(tmp>180.0)
		intensity=0.0;

	//intensity = 0.0;
	intensity = min(1.0, intensity + texture2D(radarTexture,texCoord).a);
	color=vec4(  vec3(0.05,0.79,0.04)*intensity + texture2D(backgroundTexture,texCoord).rgb*(1.0-intensity),    texture2D(backgroundTexture,texCoord).a);
	gl_FragColor = color;
}