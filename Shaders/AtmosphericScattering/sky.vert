#version 450 core                                                                      
 layout (location = 0) in vec3 a_position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 v3CameraPos;
uniform vec3 v3LightPos;
uniform vec3 v3InvWavelength;
uniform float fCameraHeight;
uniform float fCameraHeight2;
uniform float fOuterRadius;
uniform float fOuterRadius2;
uniform float fInnerRadius;
uniform float fInnerRadius2;
uniform float fKrESun;
uniform float fKmESun;
uniform float fKr4PI;
uniform float fKm4PI;
uniform float fScale;
uniform float fScaleDepth;
uniform float fScaleOverScaleDepth;

out vec3 out_color1;
out vec3 out_color2;
out vec3 out_direction;

 int nSamples = 3;
 float fSamples = 3.0;

float scale(float fCos) {
float x = 1.0 - fCos;
return (fScaleDepth * exp(- 0.00287 + x * (0.459 + x * (3.83 + x * (- 6.8 + x * 5.25)))));
}

void main(void) {                                                                                            
            //get the ray from the camera to the vertex(far point) and its length
vec3 v3Ray = a_position - v3CameraPos;
float fFar = length(v3Ray);
v3Ray = v3Ray / fFar;                                                                     

            //calculate the ray's starting position, then its scattering offset
vec3 v3Start = v3CameraPos;
float fHeight = length(v3Start);
float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
float fStartAngle = dot(v3Ray, v3Start) / fHeight;
float fStartOffset = fStartDepth * scale(fStartAngle);                                    

            //initialize the scattering loop variables
float fSampleLength = fFar / fSamples;
float fScaledLength = fSampleLength * fScale;
vec3 v3SampleRay = v3Ray * fSampleLength;
vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;                                        

            //now loop through the sample rays
vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
for(int i = 0;i < nSamples;i ++) {
float fHeight = length(v3SamplePoint);
float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
float fLightAngle = dot(v3LightPos, v3SamplePoint) / fHeight;
float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
vec3 v3Attenuate = exp(- fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
v3SamplePoint += v3SampleRay;
}

out_color1 = v3FrontColor * (v3InvWavelength * fKrESun);
out_color2 = v3FrontColor * fKmESun;
out_direction = v3CameraPos - a_position;
gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}
