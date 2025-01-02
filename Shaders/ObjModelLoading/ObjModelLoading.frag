#version 460 core

out vec4 FragColor;

uniform int isColor_godRays;
uniform bool enableGodRays;

in vec2 a_out_texcoord;
in vec3 N;
in vec3 P;
in vec3 viewPos;
in vec3 Normal;

//point light related uniform
uniform vec3 pointLight_position[20];
uniform vec3 pointLight_ambient[20];
uniform vec3 pointLight_diffuse[20];
uniform vec3 pointLight_specular[20];
uniform float pointLight_constant[20];
uniform float pointLight_linear[20];
uniform float pointLight_quadratic[20];
uniform int numOfPointLight;

//direction light direction
uniform vec3 directionLight_Direction;
uniform vec3 directionLight_ambient;
uniform vec3 directionLight_diffuse;
uniform vec3 directionLight_specular;
uniform float directionLight_shininess;

//Common varaibale
uniform sampler2D texture_diffuse1;
uniform int u_effectType;

vec4 color;

//pointLight
vec4 pointLight(){

   vec3 ambient[20]; 
   vec3 diffuse[20];
   vec3 specular[20];
   float attenuation[20];
   vec4 pointLightFinal;
   vec3 norm = normalize(Normal);

   for(int i=0;i<numOfPointLight;i++){
           vec4 diffuseTexture= texture(texture_diffuse1, a_out_texcoord);
           if (diffuseTexture.a < 0.1)
  discard;
           ambient[i] = pointLight_ambient[i] *diffuseTexture.rgb;
           
           vec3 lightDir = normalize(pointLight_position[i] - P);
           float diff = max(dot(norm, lightDir), 0.0);
           diffuse[i] = pointLight_diffuse[i] * diff * texture(texture_diffuse1, a_out_texcoord).rgb;

           vec3 viewDir = normalize(viewPos - P);
           vec3 reflectDir = reflect(-lightDir, norm);  
           float spec = pow(max(dot(viewDir, reflectDir), 0.0), 129.0);
            specular[i] = pointLight_specular[i] * spec * texture(texture_diffuse1, a_out_texcoord).rgb;

           float distance    = length(pointLight_position[i] - P);
            attenuation[i] = 1.0 / (pointLight_constant[i] + pointLight_linear[i] * distance + pointLight_quadratic[i] * (distance * distance));

           ambient[i]  *= attenuation[i];  
           diffuse[i]   *= attenuation[i];
           specular[i] *= attenuation[i];
           pointLightFinal+=vec4((ambient[i] + diffuse[i] + specular[i]),1.0);
     }
           return pointLightFinal ;
}

 //direction light
        vec4 getDirectionLight()
         {
             vec4 color;
             vec4 diffuseTexture=texture(texture_diffuse1,a_out_texcoord);
             if (diffuseTexture.a < 0.1)
		  discard;
             vec3 normal = normalize(N);
            vec3 lightDir = normalize(directionLight_Direction - P);
             vec3 viewDir = normalize(viewPos - P);
             vec3 reflectVec = reflect(-lightDir, normal);
             color = vec4((directionLight_ambient * diffuseTexture.rgb) + (directionLight_diffuse*max(dot(normal, lightDir), 0.0) * diffuseTexture.rgb) + (directionLight_specular*pow(max(dot(reflectVec, viewDir), 0.0), directionLight_shininess) *texture(texture_diffuse1, a_out_texcoord).rgb), 1.0);
            
             return color;
         }



void main(void)
{
    if(u_effectType==0)
    {
	    color=texture(texture_diffuse1, a_out_texcoord);
        if (color.a < 0.1)
			discard;
    }
	else if(u_effectType==1)
		color=pointLight();
    else if(u_effectType==2)
        color=getDirectionLight();

     if(enableGodRays)
     {
            if(isColor_godRays==1)
            {
                color = vec4(1.0);
            }
            else
            {
                color = vec4(0.0);
            }
      }

        

FragColor=color;
};
