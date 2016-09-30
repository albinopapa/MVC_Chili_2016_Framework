/*****************************************************************************************
*This camera class will to let DirectX 11 know from where and also how we are viewing   *
*the scene. The camera class will keep track of where the camera is and its current     *
*rotation. It will use the position and rotation information to generate a view matrix  *
*which will be passed into the HLSL shader for rendering.                               *
*                                                                                       *
*****************************************************************************************/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: camera.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include "Includes.h"
#include "Utilities.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "Mat4.h"
//#include "Input.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: Camera
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	// Default cause they don't do anything, so no need to define them
	Camera() = default;
	~Camera() = default;

	Mat4 GetViewMatrix()const;
	Mat4 GetProjectionMatrix()const;
	Mat4 GetOrthoMatrix()const;
	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetRotation()const;
	void SetPosition( const XMFLOAT3 &Position );
	void SetRotation( const XMFLOAT3 &Rotation );

	bool Initialize(
		const XMFLOAT3 &Position,
		const XMFLOAT3 &Rotation,
		const XMUINT2 &ScreenSize,
		const XMFLOAT2 &ScreenClipDepth );

	/*void GetInput( const Input& pInput );

	void GetMouseInput( const Input& pInput );*/
	void Update( Mouse &MouseRef, Keyboard &KbdRef );
	// ROTATE camera on the X axis
	void Pitch( float angle );

	// ROTATE camera on the Y axis
	void Yaw( float angle );

	// Translate camera position along the look vector
	void Walk( float distance );

	// Translate camera position along the look vector
	void FloatSink( float distance );

	// Translate camera position along the right vector
	void Strafe( float distance );

	void Render();

private:
	XMFLOAT3 m_Position = { 0.0f, 200.0f, 0.0f };
	// How many degrees is it rotated on the x, y, z axes
	// JUST a collection of three different angles
	XMFLOAT3 m_Orientation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_RightDir = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_UpDir = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 m_LookDir = { 0.0f, 0.0f, 1.0f };
	XMMATRIX m_ViewMatrix, m_ProjectionMatrix, m_OrthoMatrix;
	Mat4 m_view, m_proj;
};