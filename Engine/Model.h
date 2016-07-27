#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Entity_Player.h"


class Mesh;
class Texture;
class Shader;


class Model
{
	friend class GraphicsClient;
	friend class GameClient;
	
public:
	Model();
private:
	Camera m_cam;
	Entity_Player m_Player;
	Mesh m_plane;

	std::vector<Shader *> m_shaderList;
	std::vector<Texture *> m_textureList;
	std::vector<Mesh *> m_meshList;

};
