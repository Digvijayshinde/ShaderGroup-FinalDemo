#include "AtmosphericScattering.h"

int AtmosphericScatteringShader::initializeAtmosphericScatteringProgram() {

	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\AtmosphericScattering\\ground.vert");
	shaderName.push_back("Shaders\\AtmosphericScattering\\ground.frag");
	int retval = groundShaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retval != 0) {
		PrintLog("loading of atmosphereic ground shader failed\n");
		return -1;
	}
	shaderName.clear();

	shaderName.push_back("Shaders\\AtmosphericScattering\\sky.vert");
	shaderName.push_back("Shaders\\AtmosphericScattering\\sky.frag");
	 retval = skyShaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retval != 0) {
		PrintLog("loading of atmosphereic sky shader failed\n");
		return -1;
	}

    //sphere data
    GenerateSphere(innerRadius, 200, 200, vertices_inner_sphere, indices_inner_sphere);
    GenerateSphere(outerRadius, 200, 200, vertices_outer_sphere, indices_outer_sphere);

	wavelength = vec3(0.65f, 0.57f, 0.475f);
	wavelength[0] = powf(wavelength[0], 4.0f);
	wavelength[1] = powf(wavelength[1], 4.0f);
	wavelength[2] = powf(wavelength[2], 4.0f);

	fscale = 1.0f / (outerRadius - innerRadius);
	return 0;

}
void AtmosphericScatteringShader::useGroundAtmosphericScatteringProgram() {
	groundShaderProgramObject.useProgram();
}
void AtmosphericScatteringShader::useSkyAtmosphericScatteringProgram() {
	skyShaderProgramObject.useProgram();
}
void AtmosphericScatteringShader::displayGroundAtmosphericScatteringShader(mat4 modelMatrix,mat4 viewmatrix,vec3 camPos) {

    //View matix wil change accroding to camera position
    lightPos = lightPos / sqrtf(lightPos[0] * lightPos[0] + lightPos[1] * lightPos[1] + lightPos[2] * lightPos[2]);
    cameraHeight = sqrtf(camPos[0] * camPos[0] + camPos[1] * camPos[1] + camPos[2] * camPos[2]);

    groundShaderProgramObject.setMat4("modelMatrix", modelMatrix);
    groundShaderProgramObject.setMat4("viewMatrix", viewmatrix);
    groundShaderProgramObject.setMat4("projectionMatrix", prespectiveProjectionMatrix);

    groundShaderProgramObject.setVec3("v3CameraPos", camPos);
    groundShaderProgramObject.setVec3("v3LightPos", lightPos);
    groundShaderProgramObject.setVec3("v3InvWavelength", 1.0f / wavelength);

    groundShaderProgramObject.setFloat("fCameraHeight", cameraHeight);
    groundShaderProgramObject.setFloat("fCameraHeight2", cameraHeight * cameraHeight);
    groundShaderProgramObject.setFloat("fOuterRadius", outerRadius);
    groundShaderProgramObject.setFloat("fOuterRadius2", outerRadius * outerRadius);
    groundShaderProgramObject.setFloat("fInnerRadius", innerRadius);
    groundShaderProgramObject.setFloat("fInnerRadius2", innerRadius* innerRadius);

    groundShaderProgramObject.setFloat("fKrESun", rayleigh_constant * eSun);
    groundShaderProgramObject.setFloat("fKmESun", mie_constant * eSun);
    groundShaderProgramObject.setFloat("fKr4PI", rayleigh_constant * 4 * M_PI);
    groundShaderProgramObject.setFloat("fKm4PI", mie_constant * 4 * M_PI);
    groundShaderProgramObject.setFloat("fScale", fscale);
    groundShaderProgramObject.setFloat("fScaleDepth", scaleDepth);
    groundShaderProgramObject.setFloat("fScaleOverScaleDepth", fscale / scaleDepth);


    //draw sphere
    glBindVertexArray(vao_inner_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inner_sphere_indices);
    glDrawElements(GL_TRIANGLES, indices_inner_sphere.size(), GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
void AtmosphericScatteringShader::displaySkyAtmosphericScatteringShader(mat4 modelMatrix,mat4 viewMatrix,vec3 camPos) {

    //View matix wil change accroding to camera position
    lightPos = lightPos / sqrtf(lightPos[0] * lightPos[0] + lightPos[1] * lightPos[1] + lightPos[2] * lightPos[2]);
    cameraHeight = sqrtf(camPos[0] * camPos[0] + camPos[1] * camPos[1] + camPos[2] * camPos[2]);

    skyShaderProgramObject.setMat4("modelMatrix", modelMatrix);
    skyShaderProgramObject.setMat4("viewMatrix", viewMatrix);
    skyShaderProgramObject.setMat4("projectionMatrix", prespectiveProjectionMatrix);

    skyShaderProgramObject.setVec3("v3CameraPos", camPos);
    skyShaderProgramObject.setVec3("v3LightPos", lightPos);
    skyShaderProgramObject.setVec3("v3InvWavelength", 1.0f / wavelength);

    skyShaderProgramObject.setFloat("fCameraHeight", cameraHeight);
    skyShaderProgramObject.setFloat("fCameraHeight2", cameraHeight * cameraHeight);
    skyShaderProgramObject.setFloat("fOuterRadius", outerRadius);
    skyShaderProgramObject.setFloat("fOuterRadius2", outerRadius * outerRadius);
    skyShaderProgramObject.setFloat("fInnerRadius", innerRadius);
    skyShaderProgramObject.setFloat("fInnerRadius2", innerRadius * innerRadius);

    skyShaderProgramObject.setFloat("fKrESun", rayleigh_constant * eSun);
    skyShaderProgramObject.setFloat("fKmESun", mie_constant * eSun);
    skyShaderProgramObject.setFloat("fKr4PI", rayleigh_constant * 4 * M_PI);
    skyShaderProgramObject.setFloat("fKm4PI", mie_constant * 4 * M_PI);
    skyShaderProgramObject.setFloat("fScale", fscale);
    skyShaderProgramObject.setFloat("fScaleDepth", scaleDepth);
    skyShaderProgramObject.setFloat("fScaleOverScaleDepth", fscale / scaleDepth);
    skyShaderProgramObject.setFloat("g", g);
    skyShaderProgramObject.setFloat("g2", g*g);



    glBindVertexArray(vao_outer_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_outer_sphere_indices);
    glDrawElements(GL_TRIANGLES, indices_outer_sphere.size(), GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
void AtmosphericScatteringShader::unUseGroundAtmosphericScatteringProgram() {
	groundShaderProgramObject.unUseProgram();
}
void AtmosphericScatteringShader::unUseSkyAtmosphericScatteringProgram() {
	skyShaderProgramObject.unUseProgram();
}
void AtmosphericScatteringShader::deleteShaderProgramObject() {

	skyShaderProgramObject.uninitializeShader();
	groundShaderProgramObject.uninitializeShader();
}
void AtmosphericScatteringShader::increaseLightPosition(int pos, float value)
{
    lightPos[pos] += value;
}
void AtmosphericScatteringShader::decreaseLightPosition(int pos, float value)
{
    lightPos[pos] -= value;
}
void AtmosphericScatteringShader::increaseAndDecraseSunEmision(int flag, float value)   //0- for increase 1  and 1- decrease
{
    if (flag == 0)
        mie_constant += value;
    else
        mie_constant -= value;
}
void AtmosphericScatteringShader:: GenerateSphere(float radius, float sectorCount, float stackCount, std::vector<float>& vertices, std::vector<int>& indices)
{
    //variable declaration
    float x, y, z, xy;
    int t1, t2;

    float sectorStep = 2.0f * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    //code
    for (int i = 0; i <= stackCount; i++)
    {
        stackAngle = M_PI / 2.0f - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; j++)
        {
            sectorAngle = j * sectorStep;

            //vertices
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stackCount; i++)
    {
        t1 = i * (sectorCount + 1);
        t2 = t1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; j++, t1++, t2++)
        {
            if (i != 0)
            {
                indices.push_back(t1);
                indices.push_back(t2);
                indices.push_back(t1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(t1 + 1);
                indices.push_back(t2);
                indices.push_back(t2 + 1);
            }
        }
    }
    //setup vao and vbo for sphere
    glGenVertexArrays(1, &vao_inner_sphere);
    glBindVertexArray(vao_inner_sphere);
    glGenBuffers(1, &vbo_inner_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_inner_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, vertices_inner_sphere.size() * sizeof(float), vertices_inner_sphere.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_inner_sphere_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inner_sphere_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_inner_sphere.size() * sizeof(float), indices_inner_sphere.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_outer_sphere);
    glBindVertexArray(vao_outer_sphere);
    glGenBuffers(1, &vbo_outer_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_outer_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, vertices_outer_sphere.size() * sizeof(float), vertices_outer_sphere.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_outer_sphere_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_outer_sphere_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_outer_sphere.size() * sizeof(float), indices_outer_sphere.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
