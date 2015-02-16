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
	vec3 texVal = texture(heightmap, texOffset + vec2(x,z) * texStep).xyz;
    float y = texVal.r;
    
	pos = position + vec3(x,y,z) * scale;
	//pos.y = 100.0 * y - 130.0;


    // TODO: eliminate this x1000 fudge factor
	slope = (texVal.yz*2.0 - 1.0) * scale.y / (scale.xz / texStep) * 1000.0 * 100.0;
	texCoord = vec2(0);
	curvature = 0;
}
