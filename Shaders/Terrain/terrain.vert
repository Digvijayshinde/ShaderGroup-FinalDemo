#version 460 core

layout (location = 0 ) in vec2 a_position;                                       
out vec2 v_position;                                      

void main(void)                                           
{   
   v_position = a_position;                               
};
