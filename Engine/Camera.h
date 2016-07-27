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
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: Camera
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	// Initialize the camera object
	bool Initialize( const XMFLOAT3 &Position, const XMFLOAT3 &Rotation,
		const XMUINT2 &ScreenSize, const XMFLOAT2 &ScreenClipDepth );
    // set the position and rotation of the camera object. 
	void SetPosition( const XMFLOAT3 &Position );
	void SetRotation( const XMFLOAT3 &Rotation );

	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetRotation()const;

    // create the view matrix based on the position and rotation of the camera. 
	void Render();

    // retrieve the view matrix from the camera object so that the shaders can use it for rendering.
	XMMATRIX GetViewMatrix()const;
	XMMATRIX GetProjectionMatrix()const;
	XMMATRIX GetOrthoMatrix()const;

private:
	XMFLOAT3 m_Position, m_Rotation;
	XMMATRIX m_ViewMatrix, m_ProjectionMatrix, m_OrthoMatrix;

};