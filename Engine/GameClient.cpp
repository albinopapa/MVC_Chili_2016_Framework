#include "GameClient.h"

GameClient::GameClient(Model & TheController)
	:
	m_Model(TheController)
{
}

void GameClient::SetCamera(const Camera & Cam)
{
	m_Model.m_cam = Cam;
}

void GameClient::ShaderList(std::vector<Shader*> &ShaderList)
{
	m_Model.m_ShaderList = ShaderList;
}

void GameClient::MeshList(std::vector<Mesh*> &MeshList)
{
	m_Model.m_MeshList = MeshList;
}

void GameClient::Update(float DeltaTime)
{
	m_Model.m_cam.Render();
	m_Model.m_Player.Update(DeltaTime);
}
