#pragma once

#include "Model.h"

// You must add a Get function for every game object and resource
// you want to use in the renderer.
class GraphicsClient
{
public:
	friend Model;
	GraphicsClient(Model &TheModel);

	const Camera &GetCamera()const;
	const Entity_Player &GetPlayer()const;
	
private:
	Model &m_Model;
};