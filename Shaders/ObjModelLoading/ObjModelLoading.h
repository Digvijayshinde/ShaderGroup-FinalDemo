#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../../Utility/Headers/ModelLoading/StaticModelLoader.h"

class ObjModelLoadingShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeObjModelLoadingShaderProgram();
	void displayObjModelLoadingShader(StaticModel* model, mat4 *modelMatrix, mat4 viewMatrix, int instanceCount, int  modelEffectType = MODEL_DIRECTIONLIGHT);
	void deleteShaderProgramObject();
};
