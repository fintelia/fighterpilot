#version 330

uniform vec3 eye;
uniform vec3 fwd;
uniform vec3 up;
uniform vec3 right;
uniform float Rg;
uniform float Rt;
uniform vec3 sunDirection;
uniform vec3 sunRadiance;


uniform sampler2D transmittance;
uniform sampler3D inscattering;

in vec2 position;

vec3 lookup_T(float r, float u){
	float tu = (r - Rg) / (Rt - Rg);
	float tv = u * 0.5 + 0.5;
	return texture(transmittance, vec2(tu, tv)).rgb;
}

vec3 lookup_S(float r, float u, float us, float v){
	float ur = sqrt(   ((r-Rg)*(r-Rg))  /  ((Rt-Rg)*(Rt-Rg))   );
	float uu = 0.5 * u + 0.5;
	float uus = (1.0 - exp(-3*us - 0.6))  /  (1.0 - exp(-3.6));
	float uv = 0.5 * v + 0.5;
	// float uv_a = clamp(floor(uv * 8- 0.5), 0.0, 7.0) / 8;
	// float uv_b = clamp(ceil(uv * 8- 0.5), 0.0, 7.0) / 8;
	// float t = fract(uv * 8 - 0.5);
	// uus = clamp(uus, 0.5 / 32, 31.5 / 32);
	// return mix(texture(inscattering, vec3(ur, uu, uus / 8 + uv_a)).rgb,
	//            texture(inscattering, vec3(ur, uu, uus / 8 + uv_b)).rgb, t);

	uv = clamp(uv, 0.5 / 8, 7.5 / 8);
	float uus_a = clamp(floor(uus * 32 - 0.5), 0.0, 31.0) / 32;
	float uus_b = clamp(ceil(uus * 32- 0.5), 0.0, 31.0) / 32;
	float t = fract(uus * 32 - 0.5);
	return mix(texture(inscattering, vec3(ur, uu, uus_a + uv / 32)).rgb,
	           texture(inscattering, vec3(ur, uu, uus_b + uv / 32)).rgb, t);
}

void main()
{
	// vec3 earthCenter = vec3(0.0, -Rg, 0.0);

// ---------------------------------- position ---------------------------------
	// vec3 direction = normalize(fwd + right*(2.0*position.x-1.0) +
    //                            up*(2.0*position.y-1.0));
	// vec3 centerToEye = eye - earthCenter;
	// //float a = dot(direction,direction); <-- always equal to 1
	// float b = 2.0 * dot(centerToEye, direction);
	// float c = dot(centerToEye, centerToEye) - atmosphereRadius*atmosphereRadius;

	// float discriminant = b*b - 4.0 * c;
	// if(discriminant <= 0.0)
	// 	discard;

	// float sqrt_discriminant = sqrt(discriminant);

	// float t0 = 0.5 * (-sqrt_discriminant-b);
	// float t1 = 0.5 * (sqrt_discriminant-b);

	// if(t0 < 0 && t1 < 0)
	// 	discard;

	// float t = (t0 > 0) ? t0 : t1;
	// vec4 worldPosition = vec4(eye + direction * t, 1.0);
	// vec4 cameraPosition = cameraProjection * worldPosition;
	 
// ----------------------------------- color -----------------------------------

	vec3 x = eye + vec3(0, Rg, 0);
	float r = length(x);
	vec3 s = sunDirection;
	vec3 v = normalize(fwd + right * (2.0 * position.x - 1.0) +
	                   up * (2.0 * position.y - 1.0));

	// Compute L0
	vec3 L0 = lookup_T(r, v.y) * sunRadiance * 1e-7 *
	          smoothstep(cos(0.005 * 6), cos(0.005 * 4), dot(v, s));

	// Compute S[L]
	vec3 SL = lookup_S(r, dot(v,x)/r, dot(s,x)/r, dot(v,s));
	// SL = lookup_S(Rg, 0.5, -0.5, 0.5);
	//SL = texture(inscattering, vec3(0.5, position.y, position.x)).rgb;
	//	SL = texture(transmittance, position).rgb;
	
	// Compute final color
	vec3 color = L0 + SL;
	//if(position.y > 0.5 && position.x > 0.5)
	// 	color = vec3(0,1,0);
// ---------------------------------- clouds -----------------------------------
	// vec3 normal = normalize(worldPosition.xyz - earthCenter);
	// float randomIntensity =	(
    //     0.75 * texture2D(noise,
    //       worldPosition.xz*0.000001+0.00001*time*vec2(0.732546,0.680718)).r +
    //     0.25 * texture2D(noise,
    //       worldPosition.xz*0.000005+0.00004*time*vec2(-0.362358,0.932039)).r);
	// float clouds = clamp((exp(randomIntensity)-1.5) * 0.25 *
    //                      normal.y*normal.y*normal.y *
    //                      (1.0 - eye.y / (0.01 * earthRadius)), 0.0, 0.6);
	//color = mix(color, vec3(0.8,0.8,0.8), clouds);

	gl_FragColor = vec4(color, 1);
}
