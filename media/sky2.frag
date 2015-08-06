
uniform vec3 eye;
uniform vec3 fwd;
uniform vec3 up;
uniform vec3 right;
uniform mat4 cameraProjection;
uniform float time;
uniform sampler2D noise;
uniform vec2 mapCenter;
uniform vec3 lightDirection;

varying vec2 position;

//see: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
const float earthRadius = 6.3675e6;
const float atmosphereRadius = earthRadius * 1.025;//6.5267e6;
const float Kr = 0.0025; //* 0.75;
const float Km = 0.0010; //* 0.05;
const float sunEnergy = 20.0;
const float PI = 3.141592653589793238;
const float fKrESun = Kr * sunEnergy;  // Kr * ESun
const float fKmESun = Km * sunEnergy;  // Km * ESun
const float fKr4PI = Kr * 4.0 * PI;    // Kr * 4 * PI
const float fKm4PI = Km * 4.0 * PI;    // Km * 4 * PI
const float fExposure = 1.75;
const int nSamples = 5;
const float fSamples = 5.0;

// 1 / pow(wavelength, 4) for the red, green, and blue channels
const vec3 v3InvWavelength = vec3(5.6020, 9.4733, 19.6438);			
// 1 / (fOuterRadius - fInnerRadius)
const float fScale = 1.0 / (atmosphereRadius - earthRadius);
// The scale depth (i.e. the altitude at which the atmosphere's average density
// is found)
const float fScaleDepth = 0.25;
// fScale / fScaleDepth
const float fScaleOverScaleDepth = fScale / fScaleDepth;
// The Mie phase asymmetry factor
const float g = -0.999;
const float g2 = g * g;


float scale(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth *
        exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main()
{
	vec3 earthCenter = vec3(0.0, -earthRadius, 0.0);

// ---------------------------------- position ---------------------------------
	vec3 direction = normalize(fwd + right*(2.0*position.x-1.0) +
                               up*(2.0*position.y-1.0));
	vec3 centerToEye = eye - earthCenter;
	//float a = dot(direction,direction); <-- always equal to 1
	float b = 2.0 * dot(centerToEye, direction);
	float c = dot(centerToEye, centerToEye) - atmosphereRadius*atmosphereRadius;

	float discriminant = b*b - 4.0 * c;
	if(discriminant <= 0.0)
		discard;

	float sqrt_discriminant = sqrt(discriminant);

	float t0 = 0.5 * (-sqrt_discriminant-b);
	float t1 = 0.5 * (sqrt_discriminant-b);

	if(t0 < 0 && t1 < 0)
		discard;

	float t = (t0 > 0) ? t0 : t1;
	vec4 worldPosition = vec4(eye + direction * t, 1.0);
	vec4 cameraPosition = cameraProjection * worldPosition;
	gl_FragDepth = min(0.5 + 0.5 * cameraPosition.z / cameraPosition.w,
                       0.9999999);
	 
// ----------------------------------- color -----------------------------------
// see: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
	vec3 color = vec3(0,0,1);

	// Get the ray from the camera to the vertex, and its length (which is the
	// far point of the ray passing through the atmosphere)
	vec3 v3Ray = direction;
	float fFar = t;

	// Calculate the ray's starting position, then calculate its scattering
	// offset
	float fHeight = length(centerToEye);
	float fDepth = exp(fScaleOverScaleDepth * (earthRadius - fHeight));
	float fStartAngle = dot(v3Ray, centerToEye) / fHeight;
	float fStartOffset = fDepth*scale(fStartAngle);

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = eye + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint - earthCenter);
		float fDepth = exp(fScaleOverScaleDepth * (earthRadius - fHeight));
		float fLightAngle = dot(lightDirection, v3SamplePoint - earthCenter)
            / fHeight;
		float fCameraAngle = dot(v3Ray, fwd);
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle)
                                                 - scale(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors
	vec3 color2 = v3FrontColor * fKmESun;
	vec3 color1 = v3FrontColor * (v3InvWavelength * fKrESun);

	float fCos = -dot(normalize(lightDirection), normalize(direction));
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos)
        / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	color = color1 + fMiePhase * color2;
//	color = vec3(1.0) - exp(-color * fExposure);

// ---------------------------------- clouds -----------------------------------
	vec3 normal = normalize(worldPosition.xyz - earthCenter);
	float randomIntensity =	(
        0.75 * texture2D(noise,
          worldPosition.xz*0.000001+0.00001*time*vec2(0.732546,0.680718)).r +
        0.25 * texture2D(noise,
          worldPosition.xz*0.000005+0.00004*time*vec2(-0.362358,0.932039)).r);
	float clouds = clamp((exp(randomIntensity)-1.5) * 0.25 *
                         normal.y*normal.y*normal.y *
                         (1.0 - eye.y / (0.01 * earthRadius)), 0.0, 0.6);
	//color = mix(color, vec3(0.8,0.8,0.8), clouds);

	gl_FragColor = vec4(color, 1);
}
