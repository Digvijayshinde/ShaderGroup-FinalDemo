
#version 460 core

in vec2 a_texcoord_out;
in vec4 a_color_out;

//Vignitte
uniform vec2 u_resolution;
uniform float u_intensity;
uniform float u_radius;

//FBM noise 
uniform float u_time;


//constant
float softness = 0.3f;

//Common variable
uniform sampler2D u_textureSampler;
out vec4 FragColor;
uniform int u_effectType;
vec4 color;


      float random (in vec2 _st)
      {
          return fract(sin(dot(_st.xy, vec2(12.9898,78.233))) * 43758.5453123);
      }

      float noise (in vec2 _st)
      {
         vec2 i = floor(_st);
         vec2 f = fract(_st);
         float a = random(i);
         float b = random(i + vec2(1.0, 0.0));
         float c = random(i + vec2(0.0, 1.0));
         float d = random(i + vec2(1.0, 1.0));
         vec2 u = f * f * (3.0 - 2.0 * f);
         return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
      }

      int NUM_OCTAVES =  10;

      float fbm ( in vec2 _st)
      {
         float v = 0.0;
         float a = 0.3;
         vec2 shift = vec2(100.0);
         mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));

         for (int i = 0; i < NUM_OCTAVES; ++i)
         {
             v += a * noise(_st);
             _st = rot * _st * 2.0 + shift;
             a *= 0.5;
         }
      return v;
      }


vec4 grayscale(vec4 inputColor)
{    
	float average = 0.2126 * inputColor.r + 0.7152 * inputColor.g + 0.0722 * inputColor.b;
    //average = clamp(average + 0.1, 0.0, 1.0); // Add 0.1 to lift overall brightness
	return vec4(average, average, average, 1.0);
}


vec4 getFBMNoise()
 {
     vec2 resolution = vec2(1024.0, 768.0);
	vec2 st = gl_FragCoord.xy / resolution * 3.; 
     vec3 colortemp = vec3(0.0);
     vec4 noiseColor;

     vec2 q = vec2(0.0);
     q.x = fbm( st + 0.00*u_time);
     q.y = fbm( st + vec2(1.0));

     vec2 r = vec2(0.0);

     r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*u_time );
     r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*u_time);   

     float f = fbm(st+r);
    colortemp = mix(vec3(0.101961, 0.619608, 0.666667), vec3(0.666667, 0.666667, 0.498039), clamp((f * f) * 4.0, 0.0, 1.0));
    colortemp = mix(colortemp, vec3(0, 0, 0.164706), clamp(length(q), 0.0, 1.0));
    colortemp = mix(colortemp, vec3(0.666667, 1.0, 1.0), clamp(length(r.x), 0.0, 1.0));
    noiseColor =  vec4((f * f * f + .6 * f * f + .5 * f) * colortemp,1.0);
    noiseColor = grayscale(noiseColor);
    return noiseColor;
 }

void main(void)
{
    float blendFactor = 0.5; // Adjust this value to control blending

    if(u_effectType==1)
    {
             color=texture(u_textureSampler,a_texcoord_out)*vec4(1.0f,1.0f,1.0f,1.0f);
            vec2 rePos=gl_FragCoord.xy/u_resolution-0.5f;
            float len=length(rePos);
            float v=smoothstep(u_radius,u_radius-softness,len);
            color.rgb=mix(color.rgb,color.rgb*v,u_intensity);       	
   }
   else if(u_effectType==2)

        color=getFBMNoise()+texture(u_textureSampler,a_texcoord_out);

   else{
          color=texture(u_textureSampler,a_texcoord_out);
   }
     FragColor=color;
}





