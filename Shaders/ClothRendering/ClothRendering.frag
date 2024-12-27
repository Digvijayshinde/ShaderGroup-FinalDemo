#version 460 core 

in vec3 out_vPosition; 
in vec3 out_vNormal; 
in vec3 out_viewVector; 
uniform vec4 u_color; 
out vec4 FragColor; 
float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;} 
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;} 
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);} 
float noise(vec3 p) 
{ 
	vec3 a = floor(p); 
	vec3 d = p - a; 
	d = d * d * (3.0 - 2.0 * d); 
	vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0); 
	vec4 k1 = perm(b.xyxy); 
	vec4 k2 = perm(k1.xyxy + b.zzww); 
	vec4 c = k2 + a.zzzz; 
	vec4 k3 = perm(c); 
	vec4 k4 = perm(c + 1.0); 
	vec4 o1 = fract(k3 * (1.0 / 41.0)); 
	vec4 o2 = fract(k4 * (1.0 / 41.0)); 
	vec4 o3 = o2 * d.z + o1 * (1.0 - d.z); 
	vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x); 
	return o4.y * d.y + o4.x * (1.0 - d.y); 
} 
float pattern(vec3 pos, float freq) 
{ 
	vec3 fp = fract(pos * freq); 
	fp *= (1.0 - fp); 
	return dot(fp, vec3(1.0, 1.0, 1.0)); 
} 
void main(void) 
{ 
	vec3 pos = out_vPosition * 2.5;
	float noisy = float(2.0 * noise(pos) - 1.0);
	float patt = 0.0;
	float freq = 1.47;
	float scale = 0.5;
	float noiseScale = 0.07;
	float sheen = 0.74 * 0.4;
	float furriness = 20.0;
	vec4 lightDir = vec4(0.6, 0.6, -.514, 0) * 1.8;
	{
	int i = 1;
	patt += pattern(pos + noiseScale * noisy, freq) * scale;
	freq *= 2.0;
	scale *= 0.7;
	}
	{
		int i = 2; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	{ 
		int i = 3; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	{ 
		int i = 4; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	{ 
		int i = 5; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	{ 
		int i = 6; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	{ 
		int i = 7; 
		patt += pattern(pos + noiseScale * noisy, freq) * scale; 
		freq *= 2.0; 
		scale *= 0.7; 
	} 
	float diffuse = 0.25 * (1.0 + dot(out_vNormal, vec3(lightDir.x, lightDir.y, -lightDir.z))); 
	float cosView = clamp(dot(normalize(out_viewVector), out_vNormal), 0.0, 1.0); 
	float shine = pow(1.0 - cosView * cosView, furriness); 
	FragColor = ((patt * u_color) + (sheen * shine)) * diffuse; 
};