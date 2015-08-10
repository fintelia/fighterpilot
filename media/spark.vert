#version 330

out vec2 texCoord;
out vec4 color;
out float depth;

uniform mat4 cameraProjection;

in vec3 Position;
in vec2 TexCoord;
in vec4 Color4;

void main()
{
	texCoord = TexCoord;
	color = Color4;

	gl_Position = cameraProjection * vec4(Position,1.0);
    depth = gl_Position.w;
}
