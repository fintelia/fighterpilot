
#version 330
 
layout(points) in;
layout(points, max_vertices=1) out;
 
in VertexData{
	float shouldDiscard;
}vertexIn[1];

out float placeHolder;

void main()
{
	placeHolder = 1.0;
	if(vertexIn[0].shouldDiscard == 0.0)
		EmitVertex();
}