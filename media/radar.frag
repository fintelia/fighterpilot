
uniform float radarAng;
varying vec2 texCoord;
//uniform sampler2D radarTexture;
//
//uniform vec2 mapCenter;
//uniform float mapRadius;


void main()
{
	float intensity;
	vec2 loc=texCoord-vec2(0.5,0.5);
	float myAng=degrees(atan(loc.y,loc.x)+3.14159)-18.0;
	float radius=sqrt(loc.x*loc.x*4.0+loc.y*loc.y*4.0);

	if(myAng < 0)
		myAng += 360.0;

	loc.x=radius*cos(radians(myAng))/2.0;
	loc.y=radius*sin(radians(myAng))/2.0;


	float dif=720.0-(myAng+radarAng);
	if(dif>360.0)
		dif-=360.0;

	float ndif=radarAng+myAng;
	if(ndif>360.0)
		ndif-=360.0;

	if(dif-320.0>=0.0)
		intensity=(dif-320.0)/40.0;
	else if(ndif-357.0>=0.0)
		intensity=(ndif-357.0)/3.0;
	else
		intensity=0.0;
	//vec4f texel=tex(vec2(-0.005,-0.005))	+tex(vec2(0.0,-0.005))	+tex(vec2(0.005,-0.005))+
	//			tex(vec2(-0.005,0.0))		+tex(vec2(0.0,0.0))		+tex(vec2(0.005,0.0))+
	//			tex(vec2(-0.005,0.005))		+tex(vec2(0.0,0.005))	+tex(vec2(0.005,0.005));
	//texel=texel/9.0;

	//vec4 backColor=vec4(0.05,0.25+0.2*texCoord.y,0.04, clamp(-40.0*radius+40.0,0.0,1.0));//green
	//vec4 backColor=vec4(0.14,0.37,0.58, clamp(-40.0*radius+40.0,0.0,1.0)*(1.0-0.6*radius)*(1.0-0.6*radius));
	if(radius>1.0) discard;
	vec4 backColor=mix(vec4(0.11,0.35,0.52,1.0),vec4(0.11,0.35,0.52,0.3),radius/1.25);

	if(abs(loc.y)<0.003)	intensity=1.0;
	if(abs(loc.y)<0.006)	intensity=max(intensity, 1.0-(abs(loc.y)-0.003)/0.006);

	if(abs(loc.x)<0.003)	intensity=1.0;
	if(abs(loc.x)<0.006)	intensity=max(intensity, 1.0-(abs(loc.x)-0.003)/0.006);

	if(abs(radius-0.33)<0.005)	intensity=1.0;
	if(abs(radius-0.33)<0.010)	intensity=max(intensity, 1.0-(abs(radius-0.33)-0.005)/0.01);

	if(abs(radius-0.66)<0.005)	intensity=1.0;
	if(abs(radius-0.66)<0.010)	intensity=max(intensity, 1.0-(abs(radius-0.66)-0.005)/0.01);

	//intensity = min(1.0, intensity + texture2D(radarTexture,texCoord).a);

	//if((mapCenter.x-texCoord.x)*(mapCenter.x-texCoord.x) + (mapCenter.y-texCoord.y)*(mapCenter.y-texCoord.y) > mapRadius*mapRadius)
		//intensity = 1.0;

	//color=vec4(  vec3(0.05,0.79,0.04)*intensity + texture2D(radarTexture,texCoord).rgb*(1.0-intensity),    texture2D(radarTexture,texCoord).a);
	gl_FragColor = vec4(mix(backColor.rgb,vec3(0.19,0.58,0.87),intensity),backColor.a);//green": (0.05,0.79,0.04); blue: 
}