#include "DebugDrawer.h"
#include "D3DUtility.h"
#include "Camera.h"

bool DebugDrawer::DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, LineType lineType /* = LINE_STRIP */)
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

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		gD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		gD3DDevice->SetTransform(D3DTS_VIEW, &camera->ViewMatrix());
		gD3DDevice->SetTransform(D3DTS_PROJECTION, &camera->ProjMatrix());

		gD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		gD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		gD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points.size() - 1, &verts[0], sizeof(VertexXYZD));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawCircle(const Vector3& center, const Vector3& normal, float radius, D3DCOLOR color, 
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

		Assert(DrawLine(points, color, camera));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}
