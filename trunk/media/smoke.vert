
attribute float life;
varying float alpha;

void main()
{
	gl_Position = ftransform();
	alpha = life*0.5;
	
	float s = mix(20000.0,10000.0,life);
	gl_PointSize =  s/gl_Position.z;
}
