#version 330

uniform sampler2D heightmap;
uniform vec2 texOffset;
uniform float texStep;

uniform vec3 position;
uniform vec3 scale;
uniform int tileResolution;

out vec3 pos;
out vec2 slope;
out float curvature;
out vec2 texCoord;

void main()
{
	int x = gl_VertexID % tileResolution;
	int z = gl_VertexID / tileResolution;
	float y = texture(heightmap, texOffset + vec2(x,z) * texStep, 0).r;

	pos = position + vec3(x,y,z) * scale / (tileResolution - 1);

	slope = vec2(0);
	texCoord = vec2(0);
	curvature = 0;
}
