
uniform vec4 viewConstraint;
uniform vec4 color = vec4(1,1,1,1);
varying vec2 texCoord;

void main()
{
	//vec2 size = fwidth(texCoord) / viewConstraint.zw;
	//vec2 radiusVector = 2.0*texCoord - vec2(1.0,1.0);
	//vec2 absNormalizedRadiusVector = abs(normalize(radiusVector));
	//float width = max(absNormalizedRadiusVector.x*size.x,absNormalizedRadiusVector.y*size.y);
	//float r = length(radiusVector) + 2.0*width;
	//gl_FragColor = color * vec4(1.0,1.0,1.0,1.0 - clamp(0.5*abs(1.0 - r)/width,0.0,1.0));


	vec2 rVector = 2.0 * texCoord - vec2(1.0,1.0);
	vec2 abs_rVector = abs(rVector);
	float width = fwidth(texCoord.x) * (abs_rVector.x+abs_rVector.y) / dot(abs_rVector, viewConstraint.zw); 
	float r =  length(rVector) + 2.0*width;
	gl_FragColor = color * vec4(1.0,1.0,1.0,1.0 - clamp(0.5*abs(1.0 - r)/width,0.0,1.0));
}