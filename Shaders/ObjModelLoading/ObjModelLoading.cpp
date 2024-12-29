#include "ObjModelLoading.h"

int ObjModelLoadingShader::initializeObjModelLoadingShaderProgram() {

	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\ObjModelLoading\\ObjModelLoading.vert");
	shaderName.push_back("Shaders\\ObjModelLoading\\ObjModelLoading.frag");
	int retVal = shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of ObjModelLoading shader failed\n");
		return -1;
	}
	return 0;

}

void ObjModelLoadingShader::displayObjModelLoadingShader(StaticModel *staticModel,mat4 modelMatrix,mat4 viewMatrix,int modelEffectType) {


	shaderProgramObject.useProgram();
	
	//Common 
	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setInt("u_effectType", modelEffectType);

	//PointLight

	if (modelEffectType == MODEL_POINTLIGHT) {		
		shaderProgramObject.setInt("numOfPointLight", numOfPointLight);

		for (int i = 0; i < numOfPointLight; i++) {									//   numOfPointLight  declare in global.h
			string s = "pointLight_position[" + std::to_string(i) + "]";
			shaderProgramObject.setVec3("pointLight_position[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_position);
			shaderProgramObject.setVec3("pointLight_ambient[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_ambient);
			shaderProgramObject.setVec3("pointLight_diffuse[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_diffuse);
			shaderProgramObject.setVec3("pointLight_specular[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_specular);
			shaderProgramObject.setFloat("pointLight_constant[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_constant);
			shaderProgramObject.setFloat("pointLight_linear[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_linear);
			shaderProgramObject.setFloat("pointLight_quadratic[" + std::to_string(i) + "]", modelPointLightStruct[i].pointLight_quadratic);
		}
	}


	//directionLight
	shaderProgramObject.setVec3("directionLight_Direction", modelDirectionLightStruct.directionLight_Direction);
	shaderProgramObject.setVec3("directionLight_ambient", modelDirectionLightStruct.directionLight_ambient);
	shaderProgramObject.setVec3("directionLight_diffuse", modelDirectionLightStruct.directionLight_diffuse);
	shaderProgramObject.setVec3("directionLight_specular", modelDirectionLightStruct.directionLight_specular);
	shaderProgramObject.setFloat("directionLight_shininess", modelDirectionLightStruct.directionLight_shininess);

	staticModel->model->Draw(shaderProgramObject.getShaderProgramObject());
	shaderProgramObject.unUseProgram();

}

void ObjModelLoadingShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}
