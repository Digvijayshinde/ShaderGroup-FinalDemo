#pragma once
#include "../../Headers/ModelLoading/Model.h"

typedef struct StaticModel
{
	Model* model;
	BOOL enableBlend;

	StaticModel()
	{
		model = NULL;
		enableBlend = FALSE;
	}
}StaticModel;


int initializeStaticModel(StaticModel* pStaticModel, std::string const& path);
int loadStaticModel(StaticModel* pStaticModel, std::string const& path);
void uninitializeStaticModel(StaticModel* pStaticModel);
