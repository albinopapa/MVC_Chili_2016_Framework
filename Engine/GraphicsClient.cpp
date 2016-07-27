#include "GraphicsClient.h"

GraphicsClient::GraphicsClient(Model & TheModel)
	:
	m_Model(TheModel)
{
}

const Camera & GraphicsClient::GetCamera() const
{
	return m_Model.m_cam;
}

const Entity_Player & GraphicsClient::GetPlayer() const
{
	return m_Model.m_Player;
}
