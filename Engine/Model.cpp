#include "Model.h"
#include <assert.h>
#include "GraphicsClient.h"
#include "GameClient.h"
#include "Includes.h"
#include "Mesh_Textured.h"

Model::Model()
	:
	m_Player(2.f, 0.f, 0.f)
{
	m_cam.Initialize({ 0.f,0.f,-5.f }, { 0.f, 0.f, 0.f }, { 800,600 }, { 0.1,1000.f });
	m_plane.Init();
	m_Player.SetMesh(&m_plane);
}


