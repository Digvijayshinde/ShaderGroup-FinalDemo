#version 450 core                                               
       
in vec3 out_color1;                                           
in vec3 out_color2;                                             
out vec4 FragColor;

void main(void) {
    FragColor = vec4(out_color1 * 0.5 + 0.25 * out_color2, 1.0);
}
