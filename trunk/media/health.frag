uniform float health;
uniform float angle;
varying vec2 texCoord;
const float width = 0.05;

void main()
{
	//vec4 color = vec4(0.14,0.37,0.78,0.4);
	vec4 color = vec4(0.11,0.35,0.52,0.4);

	vec2 loc=texCoord;
	loc.y=1.0-loc.y;
	float radius = sqrt((loc.x-0.5)*(loc.x-0.5) + (loc.y-0.5)*(loc.y-0.5));
	float ang = atan(loc.y-0.5,loc.x-0.5);
	loc.x=radius*cos(ang+angle)+0.5;
	loc.y=radius*sin(ang+angle)+0.5;




	if((loc.y-0.5)*(loc.y-0.5)/(width*width/4.0) + (loc.x+health-1.0)*(loc.x+health-1.0)/(0.01*0.01) < 1.0 )
		color.a = 0.6;	
	else if((loc.y-0.5)*(loc.y-0.5)/(width*width/4.0) + loc.x*loc.x/(0.01*0.01) < 1.0 )
		color.a=0.3;
	else if((loc.y-0.5)*(loc.y-0.5)/(width*width/4.0) + (loc.x-1.0)*(loc.x-1.0)/(0.01*0.01) < 1.0 )
		color.a=0.9;
	else if(loc.y<0.5-width/2.0 || loc.y>0.5+width/2.0 || loc.x<0.0 || loc.x>1.0)
		discard;
	else if(loc.x>1.0-health)
		color.a = 0.9;

	loc.y = (loc.y-(0.5-width/2.0))/width;


	//color.rgb *= loc.y*0.3+0.7;
	//color.rgb += max(loc.y-0.4,0.0)*0.3;
	color.rgb = color.rgb * (1.0-loc.y) + vec3(0.19,0.58,0.78) * loc.y;


	gl_FragColor = color;
}