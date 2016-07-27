#include "Mesh.h"


void Mesh::Init()
{
	PrimitiveMaker pm;
	pm.CreateColor(1.f, 0.f, 0.f, 0.f);
	pm.CreatePlane({ 0.f,0.f,0.f }, { 1.f,1.f });
	auto vertList = pm.GetVertices();
	auto color = pm.GetColor();

	std::vector<VertexPositionColorType> vertices(vertList.size());
	for (int i = 0; i < vertList.size(); ++i)
	{
		vertices[i] = { vertList[i], color };
	}
	
	m_pVertices = std::move(vertices);
}

void Mesh::VertexList(std::vector<VertexPositionColorType> &&Vertices)
{
	m_pVertices = std::move(Vertices);
}

const std::vector<VertexPositionColorType> &Mesh::VertexList() const
{
	return m_pVertices;
}
