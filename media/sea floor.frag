
varying vec2 texCoord;
uniform sampler2D tex;
void main()
{
	float alpha=1.0;
	float d=gl_FragCoord.z/gl_FragCoord.w;
	if(d>9900.0)
		discard;
	else if(d>8900.0)
		alpha=1.0-(d-8900.0)/1000.0;

	gl_FragColor = vec4(texture2D(tex,texCoord).rgb,alpha);
	//gl_FragColor=vec4(1.0,0.0,0.0,1.0);
}