#version 330

out vec4 FragColor;

in VertexData{
	vec2 texCoord;
	vec3 color;
}vertexIn;

void main()
{
	vec2 tCentered = 2.0*vertexIn.texCoord-vec2(1.0, 1.0);
	float rSquared = dot(tCentered, tCentered);
	float r = length(tCentered);

	float alpha = min(5.0 - 5.0*r, 1);
	FragColor = vec4(vec3(0.2,0.23,0.11)*vertexIn.color*alpha * 0.25,alpha);
}