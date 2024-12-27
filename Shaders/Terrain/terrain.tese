#version 460 core                                                                                                                

layout (quads, fractional_even_spacing, cw) in;                   

in vec2 tc_position[];                                            
out vec2 te_texcoord;                                             
out vec3 world_pos;                                               
out vec3 view_direction;                                          

uniform mat4 model_matrix;                                        
uniform mat4 view_matrix;                                         
uniform mat4 projection_matrix;                                   

uniform sampler2D height_map;                                     
uniform float height_step;                                        
uniform float grid_spacing;                                       
uniform int scale_factor;                                         
uniform vec3 view_pos;                                            

void main(void)                                                   
{                                                                 
   ivec2 t_size = textureSize(height_map, 0) * scale_factor;      
   vec2 div = t_size / 64.0;                                      

   te_texcoord = tc_position[0].xy + gl_TessCoord.st / div;       

   vec4 position;                                                 
   position.xz = te_texcoord.st * t_size * grid_spacing;          
   position.y = texture(height_map, te_texcoord).r * height_step; 
   position.w = 1.0;                                              

   if(te_texcoord.s == 0.0 || te_texcoord.t == 0.0 ||             
      te_texcoord.s == 1.0 || te_texcoord.t == 1.0)               
       position.y = -1.0f;                                        

    // Transform position to world space
    vec4 world_position = model_matrix * position;

   view_direction = normalize(view_pos - world_position.xyz);

   world_pos = vec3( model_matrix * position);   
   //view_direction = normalize(position.xyz - view_pos);                      

   mat4 mvp = projection_matrix * view_matrix * model_matrix;     
   gl_Position = mvp * position;                                  
};