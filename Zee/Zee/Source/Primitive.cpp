#include"Primitive.h"

void Cube::constructGeometryData()
{
	numVertices = 24;
	numTriangles = 12;

	Vector3 *pos = new Vector3[24];
	pos[0] = Vector3(-1.0f, 1.0f,  1.0f);
	pos[1] = Vector3( 1.0f, 1.0f,  1.0f);
	pos[2] = Vector3(-1.0f, 1.0f, -1.0f);
	pos[3] = Vector3( 1.0f, 1.0f, -1.0f);

	pos[4] = Vector3( 1.0f, -1.0f,  1.0f);
	pos[5] = Vector3(-1.0f, -1.0f,  1.0f);
	pos[6] = Vector3( 1.0f, -1.0f, -1.0f);
	pos[7] = Vector3(-1.0f, -1.0f, -1.0f);

	pos[8]  = Vector3( 1.0f,  1.0f, 1.0f);
	pos[9]  = Vector3(-1.0f,  1.0f, 1.0f);
	pos[10] = Vector3( 1.0f, -1.0f, 1.0f);
	pos[11] = Vector3(-1.0f, -1.0f, 1.0f);

	pos[12] = Vector3(-1.0f,  1.0f, -1.0f);
	pos[13] = Vector3( 1.0f,  1.0f, -1.0f);
	pos[14] = Vector3(-1.0f, -1.0f, -1.0f);
	pos[15] = Vector3( 1.0f, -1.0f, -1.0f);

	pos[16] = Vector3(-1.0f,  1.0f,  1.0f);
	pos[17] = Vector3(-1.0f,  1.0f, -1.0f);
	pos[18] = Vector3(-1.0f, -1.0f,  1.0f);
	pos[19] = Vector3(-1.0f, -1.0f, -1.0f);

	pos[20] = Vector3( 1.0f,  1.0f, -1.0f);
	pos[21] = Vector3( 1.0f,  1.0f,  1.0f);
	pos[22] = Vector3( 1.0f, -1.0f, -1.0f);
	pos[23] = Vector3( 1.0f, -1.0f,  1.0f);

	Vector2 *uv = new Vector2[24];
	uv[0] = Vector2(0, 0);
	uv[1] = Vector2(1, 0);
	uv[2] = Vector2(0, 1);
	uv[3] = Vector2(1, 1);

	uv[4] = Vector2(0, 0);
	uv[5] = Vector2(1, 0);
	uv[6] = Vector2(0, 1);
	uv[7] = Vector2(1, 1);

	uv[8]  = Vector2(0, 0);
	uv[9]  = Vector2(1, 0);
	uv[10] = Vector2(0, 1);
	uv[11] = Vector2(1, 1);

	uv[12] = Vector2(0, 0);
	uv[13] = Vector2(1, 0);
	uv[14] = Vector2(0, 1);
	uv[15] = Vector2(1, 1);

	uv[16] = Vector2(0, 0);
	uv[17] = Vector2(1, 0);
	uv[18] = Vector2(0, 1);
	uv[19] = Vector2(1, 1);

	uv[20] = Vector2(0, 0);
	uv[21] = Vector2(1, 0);
	uv[22] = Vector2(0, 1);
	uv[23] = Vector2(1, 1);

	DWORD* indices = new DWORD[36];
	for(int i = 0; i < 6; i++)
	{
		indices[6*i + 0] = 4*i;
		indices[6*i + 1] = 4*i + 1;
		indices[6*i + 2] = 4*i + 2;

		indices[6*i + 3] = 4*i + 2;
		indices[6*i + 4] = 4*i + 1;
		indices[6*i + 5] = 4*i + 3;
	}

	positionData = pos;
	uvData = uv;
	indexData = indices;

	normalData = new Vector3[numVertices];
	tangentData = new Vector3[numVertices];
	bitangentData = new Vector3[numVertices];

}

void Cylinder::constructGeometryData()
{
	// 顶圆面和底圆面顶点数为segmentW + 2（不能实现中心点的不同uv坐标）
	// 柱面顶点数为(segmentW + 1) * (segmentH + 1)

	numVertices = (segmentsW + 1)*(segmentsH + 1) + 2*(segmentsW + 2);
	numTriangles = 2*segmentsW + 2*segmentsW*segmentsH;

	Vector3 *pos = new Vector3[numVertices];
	DWORD* indices = new DWORD[3*numTriangles];

	// 顶圆面
	pos[0] = Vector3(0, height, 0);
	float deltaTheta = 2*PI / segmentsW;
	for(int i = 0; i < segmentsW + 1; ++i)
	{
		float x = topRadius * cos(i*deltaTheta);
		float z = topRadius * sin(i*deltaTheta);

		pos[i+1] = Vector3(x, height, z);
	}

	for(int i = 0; i < segmentsW; ++i)
	{
		indices[3*i + 0] = i + 1;
		indices[3*i + 1] = 0;
		indices[3*i + 2] = i + 2;
	}

	// 底圆面
	pos[segmentsW + 2] = Vector3(0, 0, 0);
	for(int i = 0; i < segmentsW + 1; ++i)
	{
		float x = bottomRadius * cos(i*deltaTheta);
		float z = bottomRadius * sin(i*deltaTheta);

		pos[i + 1 + (segmentsW + 2)] = Vector3(x, 0, z);
	}

	for(int i = 0; i < segmentsW; ++i)
	{
		indices[3*i + 3*segmentsW + 0] = i + (segmentsW + 2) + 2;
		indices[3*i + 3*segmentsW + 1] = segmentsW + 2;
		indices[3*i + 3*segmentsW + 2] = i + (segmentsW + 2) + 1;
	}

	// 柱面
	float heightDelta = height / segmentsH;
	float radiusDelta = (topRadius - bottomRadius) / segmentsH;
	for(int i = 0; i <= segmentsH; ++i)
	{
		float circleHeight = i * heightDelta;
		float circleRadius = bottomRadius + i*radiusDelta;
		for(int j = 0; j <= segmentsW; ++j)
		{
			float x = circleRadius * cos(j*deltaTheta);
			float z = circleRadius * sin(j*deltaTheta);

			pos[i*(segmentsW+1) + 2*(segmentsW+2) + j] = Vector3(x, circleHeight, z);
		}
	}

	for(int i = 0; i < segmentsH; ++i)
	{
		for(int j = 0; j < segmentsW; ++j)
		{
			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 0] = i*(segmentsW+1) + 2*(segmentsW+2) + j;
			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 1] = (i+1)*(segmentsW+1) + 2*(segmentsW+2) + j;
			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 2] = (i+1)*(segmentsW+1) + 2*(segmentsW+2) + j+1;

			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 3] = i*(segmentsW+1) + 2*(segmentsW+2) + j;
			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 4] = (i+1)*(segmentsW+1) + 2*(segmentsW+2) + j+1;
			indices[2*3*segmentsW + 6*(segmentsW*i + j) + 5] = i*(segmentsW+1) + 2*(segmentsW+2) + j+1;
		}
	}

	positionData = pos;
	indexData = indices;

	normalData = new Vector3[numVertices];
	tangentData = new Vector3[numVertices];
	bitangentData = new Vector3[numVertices];

}

void Sphere::constructGeometryData()
{
	numVertices = (segmentsW + 1) * (segmentsH + 1);
	numTriangles = 2 * segmentsW * segmentsH;

	Vector3 *pos = new Vector3[numVertices];
	Vector2 *uv = new Vector2[numVertices];
	DWORD* indices = new DWORD[3*numTriangles];

	float deltaTheta = 2*PI / segmentsW;
	float deltaPhi = PI / segmentsH;

	for(int i = 0; i <= segmentsH; ++i)
	{
		// normalParis pair the verts with the same pos and normal, can be used to calculate smooth normal 
		normalPairs[i*(segmentsW+1)] = i*(segmentsW+1) + segmentsW;
		normalPairs[i*(segmentsW+1) + segmentsW] = i*(segmentsW+1);

		float circleHeight = - radius * cos(i*deltaPhi);
		float circleRadius = sqrt(radius * radius - circleHeight * circleHeight);
		for(int j = 0; j <= segmentsW; ++j)
		{
			float x = circleRadius * cos(j*deltaTheta);
			float z = circleRadius * sin(j*deltaTheta);

			float u = (float)j / segmentsW;
			float v = 1 - (float)i / segmentsH;

			pos[i*(segmentsW+1) + j] = Vector3(x, circleHeight, z);
			uv[i*(segmentsW+1) + j] = Vector2(u, v);
		}
	}

	for(int i = 0; i < segmentsH; ++i)
	{
		for(int j = 0; j < segmentsW; ++j)
		{
			indices[6*(segmentsW*i + j) + 0] = i*(segmentsW+1) + j;
			indices[6*(segmentsW*i + j) + 1] = (i+1)*(segmentsW+1) + j;
			indices[6*(segmentsW*i + j) + 2] = (i+1)*(segmentsW+1) + j+1;

			indices[6*(segmentsW*i + j) + 3] = i*(segmentsW+1) + j;
			indices[6*(segmentsW*i + j) + 4] = (i+1)*(segmentsW+1) + j+1;
			indices[6*(segmentsW*i + j) + 5] = i*(segmentsW+1) + j+1;
		}
	}

	positionData = pos;
	uvData = uv;
	indexData = indices;

	normalData = new Vector3[numVertices];
	tangentData = new Vector3[numVertices];
	bitangentData = new Vector3[numVertices];
}