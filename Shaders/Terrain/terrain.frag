#version 460 core                                                                                                         
      
in vec2 te_texcoord;                                                                                                      
in vec3 world_pos;                                                                                                        
in vec3 light_direction;                                                                                                  
in vec3 view_direction;                                                                                                   

out vec4 frag_color;                                                                                                      

uniform mat4 model_matrix;                                                                                                
uniform sampler2D splat_map;                                                                                              
uniform sampler2D grass_sampler;                                                                                          
uniform sampler2D grass_normal;                                                                                           
uniform sampler2D rock_sampler;                                                                                           
uniform sampler2D rock_normal;                                                                                            

uniform sampler2D height_map;                                                                                             
uniform float height_step;                                                                                                
uniform float grid_spacing;                                                                                               
uniform int scale_factor;                                                                                                 
uniform float tex_tiling_factor;                                                                                          

uniform vec3 light_position;                                                                                              
uniform vec3 view_pos;                                                                                                    

mat3 getTBN(vec3 N)                                                                                                       
{                                                                                                                         
   vec3 Q1 = dFdx(world_pos);                                                                                             
   vec3 Q2 = dFdy(world_pos);                                                                                             
   vec2 st1 = dFdx(te_texcoord);                                                                                          
   vec2 st2 = dFdy(te_texcoord);                                                                                          

   vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);                                                                           
   //vec3 B = -normalize(cross(N, T));                                                                                      
   vec3 B = cross(N, T);
   return (mat3(T, B, N));                                                                                                
}                                                                                                                         

float get_height(float s, float t)                                                                                        
{                                                                                                                         
   return (texture(height_map, vec2(s, t)).r * height_step);                                                              
}                                                                                                                         

vec3 apply_fog(vec3 color, float distance, vec3 rayOri, vec3 rayDir)                                                      
{                                                                                                                         
   float a = 0.0005f;                                                                                                     
   float b = 0.0002f;                                                                                                     
   float fogAmount = (a/b) * exp(-rayOri.y * b) * (1.0f - exp(-distance * rayDir.y * b)) / rayDir.y;                      
   vec3 fogColor = vec3(0.5f, 0.6f, 0.7f);                                                                                
   return (mix(color, fogColor, 0.0));                                                                              
}                                                                                                                         

void main(void)                                                                                                           
{                                                                                                                         
   vec4 splat_color = texture(splat_map, te_texcoord);                                                                    
   float delta = 1.0 / (textureSize(height_map, 0).x * scale_factor);                                                     

   vec3 x_delta;                                                                                                          
   x_delta[0] = tex_tiling_factor * grid_spacing;                                                                         
   x_delta[1] = get_height(te_texcoord.s + delta, te_texcoord.t) - get_height(te_texcoord.s - delta, te_texcoord.t);      
   x_delta[2] = 0.0;                                                                                                      

   vec3 z_delta;                                                                                                          
   z_delta[0] = 0.0;                                                                                                      
   z_delta[1] = get_height(te_texcoord.s, te_texcoord.t + delta) - get_height(te_texcoord.s, te_texcoord.t - delta);      
   z_delta[2] = tex_tiling_factor * grid_spacing;                                                                         

   mat3 normal_matrix = mat3(transpose(inverse(model_matrix)));                                                           
   vec3 normal = normalize(normal_matrix * normalize(cross(z_delta, x_delta)));                                                      
   mat3 TBN = getTBN(normal);                                                                                             

   vec3 grass_normal = texture(grass_normal, te_texcoord * tex_tiling_factor).xyz * 2.0 - 1.0;                            
   vec3 rock_normal = texture(rock_normal, te_texcoord * tex_tiling_factor).xyz * 2.0 - 1.0;  
   float total_weight = splat_color.r + splat_color.g;

   //vec3 tangent_normal = grass_normal * splat_color.g + rock_normal * splat_color.r;
   vec3 tangent_normal = normalize((grass_normal * splat_color.g + rock_normal * splat_color.r)/ total_weight);
   normal = normalize(TBN * tangent_normal);                                                                              

   //vec3 normalized_view_vector = normalize(-world_pos);    
   vec3 normalized_view_vector = normalize(view_direction);                                                                   
   
   vec3 normalized_light_direction = normalize(light_position - world_pos);                                               
   vec3 reflection_vector = reflect(-normalized_light_direction, normal);                                                 

   vec3 grass_color = texture(grass_sampler, te_texcoord * tex_tiling_factor).rgb * splat_color.g;                        
   vec3 rock_color = texture(rock_sampler, te_texcoord * tex_tiling_factor).rgb * splat_color.r;                          
   vec3 color = (grass_color + rock_color)*0.7;                                                                                 

   vec3 light_ambient = vec3(0.05f, 0.05f, 0.05f);                                                                           
   vec3 light_diffuse = vec3(1.0f, 1.0f, 1.0f);                                                                           
   float shininess = 16.0f;
   vec3 light_specular = vec3(0.5f, 0.5f, 0.5f);

   vec3 ambient = light_ambient * color;                                                                                  
   vec3 diffuse = light_diffuse * color * max(dot(normalized_light_direction, normal), 0.0f);                             
   vec3 specular = light_specular * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), shininess);

   vec3 fog_color = apply_fog(ambient + diffuse + specular, length(view_direction), view_pos, view_direction);                       

    //gamma correction
   float gamma = 2.0f;                                                                                                    
   frag_color = vec4(pow(fog_color, vec3(1/gamma)), 1.0f);                                                                
}