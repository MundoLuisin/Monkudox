#pragma once

#include "Material.h"

class MeshRenderer
{
public:

	bool castShadows = true;
	bool receiveShadows = true;

	std::vector<Material> materials;

	//MeshRenderer();

};