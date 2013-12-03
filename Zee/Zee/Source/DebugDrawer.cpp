#include "DebugDrawer.h"
#include "D3DUtility.h"
#include "Camera.h"
#include "Engine.h"

bool DebugDrawer::DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, 
						   const D3DXMATRIX& matWorld /*= IDENTITY_MATRIX*/, LINE_TYPE lineType /* = LINE_STRIP */)
{
	bool isSucceed = false;

	Assert(NULL != camera);

	if(lineType == LINE_STRIP && points.size() > 1)
	{
		std::vector<VertexXYZD> verts;
		for(size_t i = 0; i < points.size(); ++i)
		{
			VertexXYZD vert(points[i].x, points[i].y, points[i].z, color);
			verts.push_back(vert);
		}

		IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

		d3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		d3dDevice->SetTransform(D3DTS_VIEW, &camera->ViewMatrix());
		d3dDevice->SetTransform(D3DTS_PROJECTION, &camera->ProjMatrix());
	
		d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
		d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		d3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points.size() - 1, &verts[0], sizeof(VertexXYZD));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawCircle(const Vector3& center, const Vector3& normal, float radius, D3DCOLOR color, bool isSolid,
							 Camera* camera, int numSegments /*= 64*/)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;

		float theta = 0;
		float step = 2 * PI / numSegments;
		for(int i = 0; i <= numSegments; ++i)
		{
			Vector3 point(radius * cos(theta), 0, radius * sin(theta));
			points.push_back(point);

			theta += step;
		}

		Vector3 normalizedN = normal.Normalized();
		if(FloatUnequal(fabsf(normalizedN.Dot(Vector3(0, 1, 0))), 1, 0.0001f))
		{
			Quaternion rotation = Quaternion::VectorRotation(Vector3(0, 1, 0), normalizedN);

			for(size_t i = 0; i < points.size(); ++i)
			{
				Vector3& point = points[i];
				point = point * rotation;

				point += center;
			}
		}

		if(isSolid)
		{
			Assert(drawSolidTriFan(points, color, camera));
		}
		else
		{
			Assert(DrawLine(points, color, camera));
		}
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawSquare(const Vector3& center, const Quaternion& orient, float size, 
							 D3DCOLOR color, bool isSolid, Camera* camera)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;
		points.push_back(Vector3(-size / 2, 0, -size / 2));
		points.push_back(Vector3( size / 2, 0, -size / 2));
		points.push_back(Vector3( size / 2, 0,  size / 2));
		points.push_back(Vector3(-size / 2, 0,  size / 2));
		points.push_back(Vector3(-size / 2, 0, -size / 2));

		//if(FloatUnequal(fabsf(normalizedN.Dot(Vector3(0, 1, 0))), 1, 0.0001f))
		{
			for(size_t i = 0; i < points.size(); ++i)
			{
				Vector3& point = points[i];
				point = point * orient;

				point += center;
			}
		}

		if(isSolid)
		{
			Assert(drawSolidTriFan(points, color, camera));
		}
		else
		{
			Assert(DrawLine(points, color, camera));
		}
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawBox(const Vector3& center, const Quaternion& rotation, const Vector3& size, D3DCOLOR color, Camera* camera)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;
		points.reserve(8);

		float posX = size.x / 2;
		float negX = - size.x / 2;

		float posY = size.y / 2;
		float negY = - size.y / 2;

		float posZ = size.z / 2;
		float negZ = - size.z / 2;

		points.push_back(Vector3(negX, posY, posZ));
		points.push_back(Vector3(posX, posY, posZ));
		points.push_back(Vector3(negX, posY, negZ));
		points.push_back(Vector3(posX, posY, negZ));

		points.push_back(Vector3(negX, negY, posZ));
		points.push_back(Vector3(posX, negY, posZ));
		points.push_back(Vector3(negX, negY, negZ));
		points.push_back(Vector3(posX, negY, negZ));

		for(size_t i = 0; i < points.size(); ++i)
		{
			points[i] = points[i] * rotation;
			points[i] += center;
		}

		std::vector<Vector3> pointSets;
		pointSets.push_back(points[0]);
		pointSets.push_back(points[1]);
		pointSets.push_back(points[3]);
		pointSets.push_back(points[2]);
		pointSets.push_back(points[0]);
		pointSets.push_back(points[4]);
		pointSets.push_back(points[6]);
		pointSets.push_back(points[7]);
		pointSets.push_back(points[5]);
		pointSets.push_back(points[4]);
		pointSets.push_back(points[6]);
		pointSets.push_back(points[2]);
		pointSets.push_back(points[3]);
		pointSets.push_back(points[7]);
		pointSets.push_back(points[5]);
		pointSets.push_back(points[1]);

		Assert(DrawLine(pointSets, color, camera));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawAABBox(const AABBox& box, D3DCOLOR color, Camera* camera)
{
	Vector3 center = 0.5f * (box.mMin + box.mMax);
	Vector3 size = box.mMax - box.mMin;

	return DrawBox(center, Quaternion(0, 0, 0), size, 0xffff0000, camera);
}

bool DebugDrawer::drawSolidTriFan(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera)
{
	bool isSucceed = false;

	Assert(NULL != camera);

	if(points.size() >= 3)
	{
		std::vector<VertexXYZD> verts;
		for(size_t i = 0; i < points.size(); ++i)
		{
			VertexXYZD vert(points[i].x, points[i].y, points[i].z, color);
			verts.push_back(vert);
		}

		IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		d3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		d3dDevice->SetTransform(D3DTS_VIEW, &camera->ViewMatrix());
		d3dDevice->SetTransform(D3DTS_PROJECTION, &camera->ProjMatrix());

		d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, false);		// TODO:¹ÜÀíRenderState
		d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		
		d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points.size() - 1, &verts[0], sizeof(VertexXYZD));

		d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	isSucceed = true;
Exit:
	return isSucceed;
	
}

void DebugDrawer::SetRenderState()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3dDevice->SetRenderState(D3DRS_LIGHTING, false);

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
