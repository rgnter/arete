// Noise

vec4 mod289(vec4 x) {
   return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 perm(vec4 x) {
   return mod289(((x * 34.0) + 1.0) * x);
}

float noise(vec3 p, float noiseScale)
{
   p *= noiseScale;
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




// Perlin

#define PERLIN_NOISE_SCALE 2.0

float fade(float t) 
{
    return t*t*t*(t*(6.0*t-15.0)+10.0); 
}

float grad3D(float hash, vec3 pos) 
{
    int h = int(1e4*hash) & 15;
	float u = h<8 ? pos.x : pos.y,
 		  v = h<4 ? pos.y : h==12||h==14 ? pos.x : pos.z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float perlinNoise3D(vec3 pos)
{
	vec3 pi = floor(pos);
    vec3 pf = pos - pi;
    
    float u = fade(pf.x);
    float v = fade(pf.y);
    float w = fade(pf.z);
    
    return mix(mix(mix(
                    grad3D(noise(pi + vec3(0, 0, 0), PERLIN_NOISE_SCALE), pf - vec3(0, 0, 0)),
                    grad3D(noise(pi + vec3(1, 0, 0), PERLIN_NOISE_SCALE), pf - vec3(1, 0, 0)),
                    u
                ),
                mix(grad3D(noise(pi + vec3(0, 1, 0), PERLIN_NOISE_SCALE), pf - vec3(0, 1, 0)), 
                    grad3D(noise(pi + vec3(1, 1, 0), PERLIN_NOISE_SCALE), pf - vec3(1, 1, 0)),
                    u
                ),
                v
            ),
            mix(mix(grad3D(noise(pi + vec3(0, 0, 1), PERLIN_NOISE_SCALE), pf - vec3(0, 0, 1)), 
                    grad3D(noise(pi + vec3(1, 0, 1), PERLIN_NOISE_SCALE), pf - vec3(1, 0, 1)),
                    u
                ),
                mix(grad3D(noise(pi + vec3(0, 1, 1), PERLIN_NOISE_SCALE), pf - vec3(0, 1, 1)), 
                    grad3D(noise(pi + vec3(1, 1, 1), PERLIN_NOISE_SCALE), pf - vec3(1, 1, 1)),
                    u
                ),
                v
            ),
        w
    );
}