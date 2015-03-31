#version 330

in vec3 position;
in float flogz;

out vec4 FragColor;

void main()
{
    //float r = length(position.xz); 
	FragColor = vec4(fract(position.y*100)*.5+.5, fract(position.y/500)*.5+.5, 1, 1);
    if(position.y != 0)
        FragColor = vec4(1,0,0,1);
    
	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(2000000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	gl_FragDepth = 0.5;//log2(flogz) * Fcoef_half;
}
