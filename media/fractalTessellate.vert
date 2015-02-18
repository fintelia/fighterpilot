#version 330

uniform sampler2DArray heightmap;
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

	vec3 t = vec3(texOffset + vec2(x,z) * texStep, 0);
	while(all(greaterThan(t.xy, vec2(0.25,0.25))) && 
	      all(lessThan(t.xy, vec2(0.75,0.75))) && 
		  t.z < textureSize(heightmap,0).z - 1){
	break;
		t = vec3(2.0*(t.xy-0.5) + 0.5, t.z + 1.0);
	}
	
	vec3 texVal = texture(heightmap, t).xyz;
    float y = texVal.r;
    
	pos = position + vec3(x,y,z) * scale;

    // TODO: eliminate this x1000 fudge factor
	slope = (texVal.yz*2.0 - 1.0) * scale.y / (scale.xz / texStep) * 1000.0 * 100.0 * pow(2,t.z);
	texCoord = vec2(0);
	curvature = t.z;
}
