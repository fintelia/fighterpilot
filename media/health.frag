uniform float health;
uniform float angle;
varying vec2 texCoord;
const float width = 0.05;

uniform vec3 HUD_color_back=vec3(0.11,0.35,0.52);
uniform vec3 HUD_color=vec3(0.19,0.58,0.78);

void main()
{
	vec4 color = vec4(HUD_color_back,0.4);

	vec2 loc=texCoord;
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


	color.rgb = mix(color.rgb,HUD_color,loc.y);

	gl_FragColor = color;
}