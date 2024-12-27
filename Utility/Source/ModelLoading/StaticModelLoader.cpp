#include "../../Headers/ModelLoading/StaticModelLoader.h"

int initializeStaticModel(StaticModel* pStaticModel, string const& path)
{
	//code
	return loadStaticModel(pStaticModel, path);
}

int loadStaticModel(StaticModel* pStaticModel, string const& path)
{
	Model* model = NULL;
	model = new Model(path, false);
	if (model == NULL)
	{
		PrintLog("Faield to load obj model loading");
		return (-1);
	}
	pStaticModel->model = model;
	return (0);
}

void uninitializeStaticModel(StaticModel* pStaticModel)
{
	//code
	if (pStaticModel)
	{
		if (pStaticModel->model)
		{
			delete (pStaticModel->model);
			pStaticModel->model = NULL;
		}
		pStaticModel = NULL;
	}
}
