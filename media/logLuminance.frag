#version 330

in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D tex;
uniform float alpha;

void main()
{
	vec3 color = max(texture(tex, texCoord).rgb, 0);
    float luminance = max(dot(color, vec3(0.2126, 0.7152, 0.0722)), 1e-6);

//    FragColor.rgb = clamp(color, 0.0, 1.0);
    FragColor = vec4(log(luminance),0,0,alpha);
}
