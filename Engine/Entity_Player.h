#pragma once

#include "IEntity.h"
#include "Texture.h"
#include "Mesh.h"

class Entity_Player :
	public IEntity
{
public:
	Entity_Player();
	Entity_Player(float X, float Y, float Z);
	~Entity_Player();

	Texture *const GetTexture()const override;
	Mesh *const GetMesh()const override;
	DirectX::XMFLOAT3 GetPosition()const override;
	DirectX::XMFLOAT3 GetRotation()const override;
	DirectX::XMFLOAT3 GetSize()const override;
	DirectX::XMMATRIX GetWorldMatrix()const override;

	void SetMesh(Mesh *const TheMesh);
	void SetTexture(Texture *const TheTexture);
	void Update(float DeltaTime) override;

private:
	void SetPosition(const DirectX::XMFLOAT3 &Position);

private:
	float m_x, m_y, m_z;
	float m_rx, m_ry, m_rz;
	float m_sx, m_sy, m_sz;
	float m_speed;
	Texture *m_pTexture;
	Mesh *m_pMesh;
};

