#version 460 core                                                                                                              

layout (vertices = 1) out;                                        

in vec2 v_position[];
out vec2 tc_position[];                                          

void main(void)                                                   
{                                                                 
   tc_position[gl_InvocationID] = v_position[gl_InvocationID];    

   gl_TessLevelOuter[0] = 64.0f;                                  
   gl_TessLevelOuter[1] = 64.0f;                                  
   gl_TessLevelOuter[2] = 64.0f;                                  
   gl_TessLevelOuter[3] = 64.0f;                                  

   gl_TessLevelInner[0] = 64.0f;                                  
   gl_TessLevelInner[1] = 64.0f;                                  
};