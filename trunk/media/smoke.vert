
uniform float size1;
uniform float size2;
attribute float life;
varying float alpha;

void main()
{
	gl_Position = ftransform();
	alpha = 0.5*life;
	
	float s = mix(size2*100.0,size1*100.0,life);
	gl_PointSize =  s/sqrt(gl_Position.z*gl_Position.w);
}
