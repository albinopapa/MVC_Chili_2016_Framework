#pragma once

#include "Model.h"

class GameClient
{
public:
	GameClient(Model &TheController);

	void SetCamera(const Camera &Cam);
	void ShaderList(std::vector<Shader *> &ShaderList);
	void TextureList(std::vector<Texture *> &TextureList);
	void MeshList(std::vector<Mesh *> &MeshList);
	void Update(float DeltaTime);


private:
	Model &m_Model;
};
