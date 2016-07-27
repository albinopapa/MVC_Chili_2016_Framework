#pragma once

#include "Includes.h"
#include "Utilities.h"
#include "PrimitiveMaker.h"


class Mesh
{
public:

	Mesh() = default;
	virtual ~Mesh() = default;

	void Init();
	void VertexBuffer(ID3D11Buffer *pVertexBuffer);
	void IndexBuffer(ID3D11Buffer *pIndexBuffer);
	ID3D11Buffer *VertexBuffer()const;
	ID3D11Buffer *IndexBuffer()const;
	void VertexList(std::vector<VertexPositionColorType> &&V);
	const std::vector<VertexPositionColorType> &VertexList()const;

protected:
	std::vector<VertexPositionColorType> m_pVertices;
	comptr<ID3D11Buffer> m_pVertexBuffer, m_pIndexBuffer;
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
};

