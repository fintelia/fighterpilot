
uniform float size1;
uniform float size2;
attribute float life;
varying float alpha;

void main()
{
	gl_Position = ftransform();
	alpha = life;
	

	float s = 100.0 * size1;//mix(size2*100.0,size1*100.0,min(10.0*life,1.0));	
	gl_PointSize = s/sqrt(gl_Position.z*gl_Position.w);
}
