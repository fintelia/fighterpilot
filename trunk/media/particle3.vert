
#version 330

//out float radius;
//out float angle;
//out vec4 color;

out VertexData{
	vec4 color;
	float radius;
	float angle;
}vertexOut;


in vec3 Position;
in vec2 TexCoord;
in vec4 Color4;

uniform mat4 cameraProjection;

void main()
{
	vertexOut.radius = TexCoord.x;
	vertexOut.angle = TexCoord.y;
	vertexOut.color = Color4;

	gl_Position = vec4(Position,1.0);
}
