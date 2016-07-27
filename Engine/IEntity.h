#pragma once
#include "Includes.h"

class Texture;
class Mesh;

class IEntity
{
	virtual Texture *const GetTexture()const = 0;
	virtual Mesh *const GetMesh()const = 0;
	virtual DirectX::XMFLOAT3 GetPosition()const = 0;
	virtual DirectX::XMFLOAT3 GetRotation()const = 0;
	virtual DirectX::XMFLOAT3 GetSize()const = 0;
	virtual DirectX::XMMATRIX GetWorldMatrix()const = 0;
	virtual void Update(float DeltaTime) = 0;

};