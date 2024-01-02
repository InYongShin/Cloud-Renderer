#version 410 core

out vec4 outColor;

in vec3 worldPos;
in vec3 normal;
// in vec2 texCoord;
in vec3 texCoord;
in mat3 lookAt;

uniform vec2 viewport;

uniform vec3 camPos;
uniform vec3 camDir;

// Volumetric Cube
uniform float width;
uniform float height;
uniform vec3 center;

uniform vec4 backColor;


float tonemap_sRGB(float u) {
	float u_ = abs(u);
	return u_>0.0031308?( sign(u)*1.055*pow( u_,0.41667)-0.055):(12.92*u);
}
vec3 tonemap( vec3 rgb, mat3 csc, float gamma ){
	vec3 rgb_ = csc*rgb;
	if( abs( gamma-2.4) <0.01 )
		return vec3( tonemap_sRGB(rgb_.r), tonemap_sRGB(rgb_.g), tonemap_sRGB(rgb_.b) );
	return sign(rgb_)*pow( abs(rgb_), vec3(1./gamma) );
}


// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))


vec3 hash33(vec3 p)
{
	uvec3 q = uvec3(ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z)*UI3;
	return -1. + 2. * vec3(q) * UIF;
}

vec3 hash( vec3 p ) // replace this by something better. really. do
{
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}



float rand(vec3 p) 
{
    return fract(sin(dot(p, vec3(12.345, 67.89, 412.12))) * 42123.45) * 2.0 - 1.0;
}


float noise( in vec3 x )
{
    // grid
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    // quintic interpolant
    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);

    
    // gradients
    vec3 ga = hash( p+vec3(0.0,0.0,0.0) );
    vec3 gb = hash( p+vec3(1.0,0.0,0.0) );
    vec3 gc = hash( p+vec3(0.0,1.0,0.0) );
    vec3 gd = hash( p+vec3(1.0,1.0,0.0) );
    vec3 ge = hash( p+vec3(0.0,0.0,1.0) );
    vec3 gf = hash( p+vec3(1.0,0.0,1.0) );
    vec3 gg = hash( p+vec3(0.0,1.0,1.0) );
    vec3 gh = hash( p+vec3(1.0,1.0,1.0) );
    
    // projections
    float va = dot( ga, w-vec3(0.0,0.0,0.0) );
    float vb = dot( gb, w-vec3(1.0,0.0,0.0) );
    float vc = dot( gc, w-vec3(0.0,1.0,0.0) );
    float vd = dot( gd, w-vec3(1.0,1.0,0.0) );
    float ve = dot( ge, w-vec3(0.0,0.0,1.0) );
    float vf = dot( gf, w-vec3(1.0,0.0,1.0) );
    float vg = dot( gg, w-vec3(0.0,1.0,1.0) );
    float vh = dot( gh, w-vec3(1.0,1.0,1.0) );
	
    // interpolation
    return va + 
           u.x*(vb-va) + 
           u.y*(vc-va) + 
           u.z*(ve-va) + 
           u.x*u.y*(va-vb-vc+vd) + 
           u.y*u.z*(va-vc-ve+vg) + 
           u.z*u.x*(va-vb-ve+vf) + 
           u.x*u.y*u.z*(-va+vb+vc-vd+ve-vf-vg+vh);
}


// Gradient noise by iq (modified to be tileable)
float gradientNoise(vec3 x, float freq)
{
    // grid
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    // quintic interpolant
    vec3 u = w * w * w * (w * (w * 6. - 15.) + 10.);

    
    // gradients
    vec3 ga = hash33(mod(p + vec3(0., 0., 0.), freq));
    vec3 gb = hash33(mod(p + vec3(1., 0., 0.), freq));
    vec3 gc = hash33(mod(p + vec3(0., 1., 0.), freq));
    vec3 gd = hash33(mod(p + vec3(1., 1., 0.), freq));
    vec3 ge = hash33(mod(p + vec3(0., 0., 1.), freq));
    vec3 gf = hash33(mod(p + vec3(1., 0., 1.), freq));
    vec3 gg = hash33(mod(p + vec3(0., 1., 1.), freq));
    vec3 gh = hash33(mod(p + vec3(1., 1., 1.), freq));
    
    // projections
    float va = dot(ga, w - vec3(0., 0., 0.));
    float vb = dot(gb, w - vec3(1., 0., 0.));
    float vc = dot(gc, w - vec3(0., 1., 0.));
    float vd = dot(gd, w - vec3(1., 1., 0.));
    float ve = dot(ge, w - vec3(0., 0., 1.));
    float vf = dot(gf, w - vec3(1., 0., 1.));
    float vg = dot(gg, w - vec3(0., 1., 1.));
    float vh = dot(gh, w - vec3(1., 1., 1.));
	
    // interpolation
    return va + 
           u.x * (vb - va) + 
           u.y * (vc - va) + 
           u.z * (ve - va) + 
           u.x * u.y * (va - vb - vc + vd) + 
           u.y * u.z * (va - vc - ve + vg) + 
           u.z * u.x * (va - vb - ve + vf) + 
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);

}


// Fbm for Perlin noise based on iq's blog
float perlinfbm(vec3 p, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    noise = abs(noise*2.+1.);
    
    // return clamp(noise-p.y*9., 0.0, 1.0);
    return noise;
}


vec3 volumetricMarch(vec3 ro, vec3 rd){
    float depth = 0.0;
    vec4 color = vec4(.31, .73, .87, .0);
    // vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    for(int i=0; i<100; ++i){
        vec3 p = ro + depth * rd;
        // float density = fbm(p);
        float density = perlinfbm(p,4.0,8);
        // float density = noise(p);
        density = remap(density, .5, 1., 0., 1.); // fake cloud coverage
        
        if(density > 0.01){
             vec4 c = vec4(mix(vec3(1.),vec3(0.),density), density);
             c.a *= 0.4;
             c.rgb *= c.a;
             color += c*(1.0-color.a);
        }
        depth += max(0.05, 0.02*depth);
    }
    
    return clamp(color.rgb, 0.0, 1.f);
}





void main(){


    // vec3 uv = texCoord*2.-1.;
    vec2 uv = (gl_FragCoord.xy - .5*viewport.xy)/viewport.y;
    vec3 rd = lookAt * normalize(vec3(uv.x, uv.y, 1));

    vec3 ro = camPos;

    // vec3 front = normalize(vec3(0.0, 0.0, 1.0));
    // vec3 right = normalize(cross(front, vec3(0.0,1.0,0.0)));
    // vec3 up = normalize(cross(right, front));
    // mat3 lookAt_ = mat3(right, up, front);
    // vec3 rd = lookAt * normalize(vec3(uv, 1.0));
    // vec3 rd = lookAt_ * normalize(uv);
    // vec3 rd = camDir;
    

    vec3 cloudColor = volumetricMarch(ro, rd);
    
    outColor = vec4(tonemap(cloudColor, mat3(1), 2.4), 1.0);

}
