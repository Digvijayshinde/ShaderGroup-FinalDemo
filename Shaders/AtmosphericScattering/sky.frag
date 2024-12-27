 #version 450 core

 in vec3 out_color1;                                                                                          
 in vec3 out_color2;                                                                                          
 in vec3 out_direction;                                                                                       
 
 uniform vec3 v3LightPos;                                                                                     
 uniform float g;                                                                                             
 uniform float g2;                                                                                            
 
 out vec4 FragColor;                                                                                          
 
 const float gamma = 2.2f;                                                                                    
 const float exposure = 1.0f;                                                                                 

 void main(void)                                                                                              
 {                                                                                                            
    float fCos = dot(v3LightPos, out_direction) / length(out_direction);                                      
    float fMiePhase = 1.5f * ((1.0f-g2) / (2.0f+g2)) * (1.0f+fCos*fCos) / pow(1.0f + g2 - 2.0f*g*fCos, 1.5f); 
    float fRayleightPhase = 0.75f + 0.75 * fCos * fCos;                                                       
 
    vec4 color;
    color.rgb = out_color1 * fRayleightPhase + out_color2 * fMiePhase;                                        
    color.a = color.b;                                                                                        
 
     //gamma correction and hdr
    color.rgb = vec3(1.0f) - exp(-color.rgb * exposure);                                                      
    color.rgb = pow(color.rgb, vec3(1.0f / gamma));                                                           
    FragColor = color;                                                                                        
 }
