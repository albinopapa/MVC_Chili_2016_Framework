#include "Mesh_Textured.h"



Mesh_Textured::Mesh_Textured(ID3D11Device *pDevice3D)
{
	PrimitiveMaker pm;
	pm.CreateCube({ 0.f, 0.f, 0.f }, { 2.f, 2.f, 2.f });

	auto verts = pm.GetVertices();
	auto indices = pm.GetIndices();
	auto normals = pm.GetNormals();
	auto uvs = pm.GetUVs();


}


Mesh_Textured::~Mesh_Textured()
{
}
