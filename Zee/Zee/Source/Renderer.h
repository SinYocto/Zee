#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"

class Geometry;
class Material;
class Camera;
class Billboard;

class Renderer
{
public:
	static void Begin(ShadingMethod method);
	static void SetRenderState(ShadingMethod method);

	static void DrawMesh(const D3DXMATRIX& matWorld, Geometry* geo, Material* material, Camera* camera);
	static void DrawBillboard(const Vector3& pos, Billboard* billborad, Camera* camera);

	static void End(ShadingMethod method);
};

#endif