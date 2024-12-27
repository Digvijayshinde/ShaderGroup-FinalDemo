#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../../Utility/Headers/ModelLoading/StaticModelLoader.h"

class ObjModelLoadingShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeObjModelLoadingShaderProgram();
	void displayObjModelLoadingShader(StaticModel* model, mat4 modelMatrix, mat4 viewMatrix,int  modelEffectType=MODEL_NO_EFFECT);
	void deleteShaderProgramObject();
};
