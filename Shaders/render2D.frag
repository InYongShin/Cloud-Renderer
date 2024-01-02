#version 410 core

const float pi = 3.141592f;

out vec4 outColor;

uniform vec2 viewport;

uniform vec3 camPos;
uniform vec3 camDir;
uniform mat4 lookAt;

uniform vec3 lightPos;
uniform vec3 lightCol;

uniform vec3 boundsMin;
uniform vec3 boundsMax;

uniform float r1;
uniform float r2;

uniform float rayStep;
uniform int maxStep;

uniform float lightStep;
uniform int maxLightStep;

uniform float frequency;
uniform int numOctaves;
uniform float H;

uniform float coverage;

uniform float absorption;
uniform float lightAbsorptionToSun;

uniform float g;

uniform vec4 backColor;


float tonemap_sRGB(float u) {
	float u_ = abs(u);
	return u_>0.0031308?( sign(u)*1.055*pow( u_,0.41667)-0.055):(12.92*u);
}
vec3 tonemap( vec3 rgb, mat3 csc, float gamma ) {
	vec3 rgb_ = csc*rgb;
	if( abs( gamma-2.4) <0.01 )
		return vec3( tonemap_sRGB(rgb_.r), tonemap_sRGB(rgb_.g), tonemap_sRGB(rgb_.b) );
	return sign(rgb_)*pow( abs(rgb_), vec3(1./gamma) );
}


float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}


// from iq
vec3 hash( vec3 p ) {
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}


// from iq
// returns 3D value noise
float perlin( vec3 x )
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


float perlinFBM( vec3 x, float freq, float H )
{    
    float G = exp2(H);
    float amp = 1.0;
    float noise = 0.0;
    for( int i=0; i<numOctaves; ++i )
    {
        noise += amp * perlin(freq*x);
        freq *= 2.0;
        amp *= G;
    }
    return noise;
}


float worley(vec3 x)
{    
    vec3 id = floor(x);
    vec3 p = fract(x);
    
    float minDist = 10000.;
    for(float x=-1.; x<=1.; ++x)
    {
        for(float y=-1.; y<=1.; ++y)
        {
            for(float z=-1.; z<=1.; ++z)
            {
                vec3 offset = vec3(x, y, z);
            	vec3 h = hash(id + offset) * .5 + .5;
    			h += offset;
            	vec3 d = p - h;
           		minDist = min(minDist, dot(d, d));
            }
        }
    }
    
    // inverted worley noise
    return 1. - minDist;
}


float worleyFBM(vec3 p, float freq)
{
    return worley(p*freq) * .625 + worley(p*freq*2.) * .25 + worley(p*freq*4.) * .125;
}


float sampleDensity(vec3 p){
    float pfbm = perlinFBM(p, frequency, H);
    float wfbm = worleyFBM(p, frequency);
    float perlinWorley = remap(pfbm, 0., 1., wfbm, 1.);
    float density = remap(perlinWorley, wfbm-1., 1., 0., 1.);
    density = remap(density, 1.-coverage, 1., 0., 1.);
    return density;
}


float HGPhase(float g, float cosTheta){
    return (1-g*g) / (4*pi * pow(1 + g*g - 2*g*cosTheta ,3/2));
}


float lightMarch(vec3 rd, vec3 p){
    vec3 toLight = normalize(lightPos - p);
    
    float totalDensity = 0;
    for(int i=0; i<maxLightStep; ++i){
        p += toLight * lightStep;
        totalDensity += max(0, sampleDensity(p) * lightStep);
    }

    // float cosTheta = dot(rd, toLight);
    // float phase = HGPhase(g, -cosTheta);

    // float transmittance = 2 * exp(-totalDensity * lightAbsorptionToSun) * (1-exp(-2*totalDensity * lightAbsorptionToSun)) * phase;
    float transmittance = exp(-totalDensity * lightAbsorptionToSun);

    float darknessThreshold = 0.1;
    return darknessThreshold + transmittance * (1-darknessThreshold); 
}


vec2 rayBoxDist(vec3 boundsMin_, vec3 boundsMax_, vec3 ro, vec3 rd){
    vec3 t0 = (boundsMin_ - ro) / rd;
    vec3 t1 = (boundsMax_ - ro) / rd;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    float distA = max(max(tmin.x, tmin.y), tmin.z);
    float distB = min(tmax.x, min(tmax.y, tmax.z));

    float distToBox = max(0, distA);
    float distInsideBox = max(0, distB - distToBox);

    return vec2(distToBox, distInsideBox);
}


void main() {

    vec2 uv = (2*gl_FragCoord.xy - viewport) / viewport.y;
    
    vec3 front = camDir;
    vec3 right = normalize(cross(front, vec3(0,1,0)));
    vec3 up = normalize(cross(right, front));
    mat3 lookAt_ = mat3(right, up, front);

    vec3 rd = lookAt_ * normalize(vec3(uv,1.));
    vec3 ro = camPos;

    vec4 col = vec4(.31, .73, .87, 1);

    vec2 rayBoxInfo = rayBoxDist(boundsMin, boundsMax, ro, rd);
    float distToBox = rayBoxInfo.x;
    float distInsideBox = rayBoxInfo.y;

    float transmittance = 1;
    vec3 lightEnergy = vec3(0);

    if(distInsideBox > 0){
        float stepLength = distInsideBox / maxStep;
        float totalStepLength = distToBox;
        for(int i=0; i<maxStep; ++i){
            vec3 p = ro + totalStepLength * rd;
            float density = sampleDensity(p);

            if(density>0.001){
                // float lightTransmittance = lightMarch(rd, p);
                // vec3 toLight = normalize(lightPos - p);
                // float cosTheta = dot(rd, toLight);
                // float phase = HGPhase(g, -cosTheta);
                transmittance *= exp(-density * stepLength * absorption);
                // transmittance *= 2*(1 - exp(-2*density * rayStep));
                // lightEnergy += density * stepLength * transmittance * lightTransmittance;
                lightEnergy += density * stepLength * transmittance;
                
                // 3D texture

                if(transmittance < 0.01) break;
            }
            totalStepLength += stepLength;
        }
    }
    
    vec3 cloudCol = lightEnergy * lightCol; // light color = vec3(1)
    col.rgb = col.rgb * transmittance + cloudCol;

    // outColor = vec4(vec3(transmittance), 1.);
    outColor = vec4(tonemap(col.rgb,mat3(1),2.4), col.a);

}
