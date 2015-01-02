#version 330

in vec2 texCoord;
out vec4 FragColor;

const int NUM_WAVES = 16;
uniform float amplitudes[NUM_WAVES];
uniform float frequencies[NUM_WAVES];
uniform float waveSpeeds[NUM_WAVES];
uniform vec2 waveDirections[NUM_WAVES];

uniform float time;
uniform float amplitudeScale;

/*
 * gets the height and slope of a wave. 
 * lookup is periodic, with period = 1.
 */
vec2 waveLookup(float t)
{
	return vec2(0.5 * sin(t*6.2831853),
				cos(t*6.2831853) * 3.1415926);
}

void main()
{
	FragColor = vec4(0.5,0,1,0);

	for(int i=0; i < NUM_WAVES; i++){
		vec2 w = waveLookup(frequencies[i] * dot(texCoord,waveDirections[i]) 
							+ 0.001*waveSpeeds[i]*time);

		FragColor.x += amplitudes[i] * w.x * amplitudeScale;
		FragColor.yw += amplitudes[i] * w.y * waveDirections[i];
	}

	FragColor.yzw = normalize(FragColor.yzw);
}
