
attribute float life;
varying float alpha;

void main()
{
	gl_Position = ftransform();
	alpha = 0.5*life;
	
	float s = mix(20000.0,10000.0,life);
	gl_PointSize =  s/sqrt(gl_Position.z*gl_Position.w);
}
