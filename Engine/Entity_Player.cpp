#include "Entity_Player.h"

using namespace DirectX;

Entity_Player::Entity_Player()
{
}

Entity_Player::Entity_Player(float X, float Y, float Z)
	:
	m_x(X),
	m_y(Y),
	m_z(Z),
	m_rx(0.f),
	m_ry(0.f),
	m_rz(XMConvertToRadians(45.f)),
	m_sx(10.f),
	m_sy(10.f),
	m_sz(1.f),
	m_speed(0.4f)
{
}

Entity_Player::~Entity_Player()
{
}

Texture * const Entity_Player::GetTexture() const
{
	return m_pTexture;
}

Mesh * const Entity_Player::GetMesh() const
{
	return m_pMesh;
}

DirectX::XMFLOAT3 Entity_Player::GetPosition() const
{
	return DirectX::XMFLOAT3(m_x, m_y, m_z);
}

DirectX::XMFLOAT3 Entity_Player::GetRotation() const
{
	return DirectX::XMFLOAT3(m_rx, m_ry, m_rz);
}

DirectX::XMFLOAT3 Entity_Player::GetSize() const
{
	return DirectX::XMFLOAT3(m_sx, m_sy, m_sz);
}

DirectX::XMMATRIX Entity_Player::GetWorldMatrix() const
{
	auto translation = XMMatrixTranslation(m_x, m_y, m_z);
	auto rotation = XMMatrixRotationRollPitchYaw(m_rx, m_ry, m_rz);
	auto scale = XMMatrixScaling(m_sx, m_sy, m_sz);

	return (rotation * scale * translation);
}

void Entity_Player::SetMesh(Mesh * const TheMesh)
{
	m_pMesh = TheMesh;
}

void Entity_Player::SetTexture(Texture * const TheTexture)
{
	m_pTexture = TheTexture;
}

void Entity_Player::Update(float DeltaTime)
{
	// Load speed 
	auto xmSpeed = XMVectorReplicate(m_speed);

	// Load position
	auto position = GetPosition();
	auto xmPosition = XMLoadFloat3(&position);
	
	// Load orientation
	auto orientation = GetRotation();
	auto xmOrientation = XMLoadFloat3(&orientation);
	
	// Create rotation matrix
	auto rotate = XMMatrixRotationRollPitchYawFromVector(xmOrientation);
	
	// Create transformed vector direction
	auto xmDirection = XMVector3TransformCoord(XMVectorSet(1.f,0.f,0.f,0.f), rotate);

	// Multiply speed and direction, then add position
	//xmPosition = XMVectorMultiplyAdd(xmDirection, xmSpeed, xmPosition);

	// Store new position and set as current position
	XMStoreFloat3(&position, xmPosition);
	SetPosition(position);
}

void Entity_Player::SetPosition(const DirectX::XMFLOAT3 & Position)
{
	m_x = Position.x;
	m_y = Position.y;
	m_z = Position.z;
}
