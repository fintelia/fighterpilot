#version 330

uniform sampler2D tex;

in vec2 texCoord;
out vec4 FragColor;

void main()
{
	FragColor = vec4(max(texture(tex, texCoord).rgb - 1, 0), 1);
}
