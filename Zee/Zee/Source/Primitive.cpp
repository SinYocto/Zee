#include"Primitive.h"

void Cube::constructGeometryData()
{
	// positionData
	float halfSize = 0.5f * mSize;

	Vector3 pos[8];
	pos[0] = Vector3(-halfSize, halfSize,  halfSize);
	pos[1] = Vector3( halfSize, halfSize,  halfSize);
	pos[2] = Vector3(-halfSize, halfSize, -halfSize);
	pos[3] = Vector3( halfSize, halfSize, -halfSize);

	pos[4] = Vector3(-halfSize, -halfSize,  halfSize);
	pos[5] = Vector3( halfSize, -halfSize,  halfSize);
	pos[6] = Vector3(-halfSize, -halfSize, -halfSize);
	pos[7] = Vector3( halfSize, -halfSize, -halfSize);

	for(int i = 0; i < 8; ++i)
		mPositionData.push_back(pos[i]);

	// uvData
	Vector2 uv[4];
	uv[0] = Vector2(0, 0);
	uv[1] = Vector2(1, 0);
	uv[2] = Vector2(0, 1);
	uv[3] = Vector2(1, 1);

	for(int i = 0; i < 4; ++i)
		mUVData.push_back(uv[i]);

	// verts
	Geometry::Vert geoVerts[24];
	geoVerts[0] = Vert(0, 0);
	geoVerts[1] = Vert(1, 1);
	geoVerts[2] = Vert(2, 2);
	geoVerts[3] = Vert(3, 3);

	geoVerts[4] = Vert(5, 0);
	geoVerts[5] = Vert(4, 1);
	geoVerts[6] = Vert(7, 2);
	geoVerts[7] = Vert(6, 3);

	geoVerts[8] = Vert(2, 0);
	geoVerts[9] = Vert(3, 1);
	geoVerts[10] = Vert(6, 2);
	geoVerts[11] = Vert(7, 3);

	geoVerts[12] = Vert(1, 0);
	geoVerts[13] = Vert(0, 1);
	geoVerts[14] = Vert(5, 2);
	geoVerts[15] = Vert(4, 3);

	geoVerts[16] = Vert(0, 0);
	geoVerts[17] = Vert(2, 1);
	geoVerts[18] = Vert(4, 2);
	geoVerts[19] = Vert(6, 3);

	geoVerts[20] = Vert(3, 0);
	geoVerts[21] = Vert(1, 1);
	geoVerts[22] = Vert(7, 2);
	geoVerts[23] = Vert(5, 3);

	for(int i = 0; i < 24; ++i)
		mVerts.push_back(geoVerts[i]);

	// triangles
	Geometry::Triangle triangles[12];
	triangles[0] = Triangle(0, 1, 2);
	triangles[1] = Triangle(2, 1, 3);

	triangles[2] = Triangle(4, 5, 6);
	triangles[3] = Triangle(6, 5, 7);

	triangles[4] = Triangle(8, 9, 10);
	triangles[5] = Triangle(10, 9, 11);

	triangles[6] = Triangle(12, 13, 14);
	triangles[7] = Triangle(14, 13, 15);

	triangles[8] = Triangle(16, 17, 18);
	triangles[9] = Triangle(18, 17, 19);

	triangles[10] = Triangle(20, 21, 22);
	triangles[11] = Triangle(22, 21, 23);

	for(int i = 0; i < 12; ++i)
		mTriangles.push_back(triangles[i]);
}

void Cylinder::constructGeometryData()
{
	int numVerts = segmentsW * (segmentsH + 1) + 2;

	// positionData
	mPositionData.push_back(Vector3::Zero);
	mPositionData.push_back(Vector3(0, height, 0));

	float heightDelta = height / segmentsH;
	float radiusDelta = (topRadius - bottomRadius) / segmentsH;
	float deltaTheta = 2*PI / segmentsW;

	for(int i = 0; i <= segmentsH; ++i)
	{
		float circleHeight = i * heightDelta;
		float circleRadius = bottomRadius + i*radiusDelta;
		for(int j = 0; j < segmentsW; ++j)
		{
			float x = circleRadius * cos(j*deltaTheta);
			float z = circleRadius * sin(j*deltaTheta);

			mPositionData.push_back(Vector3(x, circleHeight, z));
		}
	}

	// verts
	for(size_t i = 0; i < mPositionData.size(); ++i)
	{
		Geometry::Vert vert(i);
		mVerts.push_back(vert);
	}

	// triangles
	for(int i = 0; i < segmentsW; ++i)		// 底圆面
	{
		// 顶点索引为2 ~ sw+1
		Geometry::Triangle triangle(0, 2+i, 2 + (i + 1) % segmentsW);
		mTriangles.push_back(triangle);
	}

	for(int i = 0; i < segmentsW; ++i)		// 顶圆面
	{
		// 顶点索引为sw*sh + 2 ~ sw*(sh + 1) + 1
		Geometry::Triangle triangle(1, segmentsW*segmentsH + 2 + (i + 1) % segmentsW, segmentsW*segmentsH + 2 + i);
		mTriangles.push_back(triangle);
	}

	for(int i = 0; i < segmentsH; ++i)		// 柱面
	{
		for(int j = 0; j < segmentsW; ++j)		
		{
			Geometry::Triangle tri1;
			Geometry::Triangle tri2;

			tri1.vertexIndex[0] = segmentsW * i + j + 2;
			tri1.vertexIndex[1] = segmentsW * (i + 1) + j + 2;
			tri1.vertexIndex[2] = segmentsW * i + (j + 1)%segmentsW + 2;

			tri2.vertexIndex[0] = segmentsW * (i + 1) + j + 2;
			tri2.vertexIndex[1] = segmentsW * (i + 1) + (j + 1)%segmentsW + 2;
			tri2.vertexIndex[2] = segmentsW * i + (j + 1)%segmentsW + 2;

			_Assert(tri1.vertexIndex[0] < segmentsW * (segmentsH + 1) + 2);
			_Assert(tri1.vertexIndex[1] < segmentsW * (segmentsH + 1) + 2);
			_Assert(tri1.vertexIndex[2] < segmentsW * (segmentsH + 1) + 2);

			_Assert(tri2.vertexIndex[0] < segmentsW * (segmentsH + 1) + 2);
			_Assert(tri2.vertexIndex[1] < segmentsW * (segmentsH + 1) + 2);
			_Assert(tri2.vertexIndex[2] < segmentsW * (segmentsH + 1) + 2);

			mTriangles.push_back(tri1);
			mTriangles.push_back(tri2);
		}
	}
}
//
//void Sphere::constructGeometryData()
//{
//	//numVertices = (segmentsW + 1) * (segmentsH + 1);
//	//numTriangles = 2 * segmentsW * segmentsH;
//
//	//Vector3 *pos = new Vector3[numVertices];
//	//Vector2 *uv = new Vector2[numVertices];
//	//DWORD* indices = new DWORD[3*numTriangles];
//
//	//float deltaTheta = 2*PI / segmentsW;
//	//float deltaPhi = PI / segmentsH;
//
//	//for(int i = 0; i <= segmentsH; ++i)
//	//{
//	//	float circleHeight = - radius * cos(i*deltaPhi);
//	//	float circleRadius = sqrt(radius * radius - circleHeight * circleHeight);
//	//	for(int j = 0; j <= segmentsW; ++j)
//	//	{
//	//		float x = circleRadius * cos(j*deltaTheta);
//	//		float z = circleRadius * sin(j*deltaTheta);
//
//	//		float u = (float)j / segmentsW;
//	//		float v = 1 - (float)i / segmentsH;
//
//	//		pos[i*(segmentsW+1) + j] = Vector3(x, circleHeight, z);
//	//		uv[i*(segmentsW+1) + j] = Vector2(u, v);
//	//	}
//	//}
//
//	//for(int i = 0; i < segmentsH; ++i)
//	//{
//	//	for(int j = 0; j < segmentsW; ++j)
//	//	{
//	//		indices[6*(segmentsW*i + j) + 0] = i*(segmentsW+1) + j;
//	//		indices[6*(segmentsW*i + j) + 1] = (i+1)*(segmentsW+1) + j;
//	//		indices[6*(segmentsW*i + j) + 2] = (i+1)*(segmentsW+1) + j+1;
//
//	//		indices[6*(segmentsW*i + j) + 3] = i*(segmentsW+1) + j;
//	//		indices[6*(segmentsW*i + j) + 4] = (i+1)*(segmentsW+1) + j+1;
//	//		indices[6*(segmentsW*i + j) + 5] = i*(segmentsW+1) + j+1;
//	//	}
//	//}
//
//	//positionData = pos;
//	//uvData = uv;
//	//indexData = indices;
//
//	//normalData = new Vector3[numVertices];
//	//tangentData = new Vector3[numVertices];
//	//bitangentData = new Vector3[numVertices];
//}

void Torus::constructGeometryData()
{
	int numVerts = mSegmentsR * mSegmentsT;

	// positionData
	float deltaThetaR = 2 * PI / mSegmentsR;
	float deltaThetaT = 2 * PI / mSegmentsT;

	for(int i = 0; i < mSegmentsR; ++i)
	{
		float thetaR = i * deltaThetaR;
		for(int j = 0; j < mSegmentsT; ++j)
		{
			float thetaT = j * deltaThetaT;

			float x = (mRadius + mTubeRadius * cos(thetaT)) * cos(thetaR);
			float y = mTubeRadius * sin(thetaT);
			float z = (mRadius + mTubeRadius * cos(thetaT)) * sin(thetaR);

			mPositionData.push_back(Vector3(x, y, z));
		}
	}

	// verts
	for(size_t i = 0; i < mPositionData.size(); ++i)
	{
		Geometry::Vert vert(i);
		mVerts.push_back(vert);
	}

	// triangles
	for(int i = 0; i < mSegmentsR; ++i)
	{
		for(int j = 0; j < mSegmentsT; ++j)		
		{
			Geometry::Triangle tri1;
			Geometry::Triangle tri2;

			tri1.vertexIndex[0] = mSegmentsT * i + j;
			tri1.vertexIndex[1] = mSegmentsT * i + (j + 1) % mSegmentsT;
			tri1.vertexIndex[2] = mSegmentsT * ((i + 1) % mSegmentsR) + j ;

			tri2.vertexIndex[0] = mSegmentsT * ((i + 1) % mSegmentsR) + j;
			tri2.vertexIndex[1] = mSegmentsT * i + (j + 1) % mSegmentsT;
			tri2.vertexIndex[2] = mSegmentsT * ((i + 1) % mSegmentsR) + (j + 1) % mSegmentsT;

			_Assert(tri1.vertexIndex[0] < mSegmentsR * mSegmentsT);
			_Assert(tri1.vertexIndex[1] < mSegmentsR * mSegmentsT);
			_Assert(tri1.vertexIndex[2] < mSegmentsR * mSegmentsT);

			_Assert(tri2.vertexIndex[0] < mSegmentsR * mSegmentsT);
			_Assert(tri2.vertexIndex[1] < mSegmentsR * mSegmentsT);
			_Assert(tri2.vertexIndex[2] < mSegmentsR * mSegmentsT);

			mTriangles.push_back(tri1);
			mTriangles.push_back(tri2);
		}
	}
}
