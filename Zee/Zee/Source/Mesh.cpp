#include"Mesh.h"
#include<fstream>

void Mesh::createVertexBuffer()
{
	CreateVB(gD3DDevice, &vertexBuffer, vertexData, numVertices, vertexType);
}

void Mesh::createIndexBuffer(int numIndices)
{
	CreateIB(gD3DDevice, &indexBuffer, indexData, numIndices);
}

void Mesh::BuildGeometry(VertexType type)
{
	vertexType = type;

	switch(vertexType)
	{
		case XYZ:
			{
				vertexData = positionData;
				break;
			}
		case XYZ_UV:
			{
				vertexData = new VertexUV[numVertices];
				for(int i = 0; i < numVertices; ++i)
				{
					VertexUV vert = VertexUV(positionData[i].x, positionData[i].y, positionData[i].z, 
						uvData[i].x, uvData[i].y);

					((VertexUV*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_N:
			{
				vertexData = new VertexN[numVertices];
				for(int i = 0; i < numVertices; ++i)
				{
					VertexN vert = VertexN(positionData[i].x, positionData[i].y, positionData[i].z, 
						normalData[i].x, normalData[i].y, normalData[i].z);

					((VertexN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_N:
			{
				vertexData = new VertexUVN[numVertices];
				for(int i =0; i < numVertices; ++i)
				{
					VertexUVN vert = VertexUVN(positionData[i].x, positionData[i].y, positionData[i].z, 
						uvData[i].x, uvData[i].y, normalData[i].x, normalData[i].y, normalData[i].z);

					((VertexUVN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_TBN:
			{
				vertexData = new VertexUVTBN[numVertices];
				for(int i =0; i < numVertices; ++i)
				{
					VertexUVTBN vert = VertexUVTBN(positionData[i].x, positionData[i].y, positionData[i].z, 
						uvData[i].x, uvData[i].y, tangentData[i].x, tangentData[i].y, tangentData[i].z,
						bitangentData[i].x, bitangentData[i].y, bitangentData[i].z,
						normalData[i].x, normalData[i].y, normalData[i].z);

					((VertexUVTBN*)vertexData)[i] = vert;
				}
				break;
			}
	}

	createVertexBuffer();
	createIndexBuffer(3*numTriangles);

	createVertexDeclaration();

	calculateBoundingBox();
}

void Mesh::createVertexDeclaration()
{
	switch(vertexType)
	{
	case XYZ:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_UV:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_N:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},   
				{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},        
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_UV_N:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},    
				{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_UV_TBN:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},    
				{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},     
				{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},     
				{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	}
}

void Mesh::SetVertexDeclaration()
{
	gD3DDevice->SetVertexDeclaration(vertexDecl);
}

void Mesh::Draw()
{
	gD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, numTriangles);
}

void Mesh::SetVertexStream()
{	
	gD3DDevice->SetStreamSource(0, vertexBuffer, 0, SizeofVertex(vertexType));
	gD3DDevice->SetIndices(indexBuffer);
}

void Mesh::CalculateNormals()
{
	for(int i = 0; i < numVertices; ++i)
	{
		normalData[i] = Vector3::Zero;
	}

	for(int i = 0; i < numTriangles; ++i)
	{
		Vector3 v0 = positionData[indexData[3*i + 0]];
		Vector3 v1 = positionData[indexData[3*i + 1]];
		Vector3 v2 = positionData[indexData[3*i + 2]];

		Vector3 v0v1 = v1 - v0;
		Vector3 v0v2 = v2 - v0;

		Vector3 normal = v0v1.Cross(v0v2);
		normal.Normalize();

		for(int k = 0; k < 3; ++k)
		{
			normalData[indexData[3*i + k]] += normal;

			// using normalPairs to find the the vert with the same normal
			std::map<int, int>::iterator iter = normalPairs.find(indexData[3*i + k]);
			if(iter != normalPairs.end())
				normalData[iter->second] += normal;
		}
	}

	for(int i = 0; i < numVertices; ++i)
	{
		normalData[i].Normalize();
	}
}

void Mesh::CalculateTangents()
{
	for(int i = 0; i < numVertices; ++i)
	{
		tangentData[i] = Vector3::Zero;
	}

	for(int i = 0; i < numTriangles; ++i){
		Vector3 v0 = positionData[indexData[3*i + 0]];
		Vector3 v1 = positionData[indexData[3*i + 1]];
		Vector3 v2 = positionData[indexData[3*i + 2]];

		Vector2 uv0 = uvData[indexData[3*i + 0]];
		Vector2 uv1 = uvData[indexData[3*i + 1]];
		Vector2 uv2 = uvData[indexData[3*i + 2]];

		Vector3 v0v1 = v1 - v0;
		Vector3 v1v2 = v2 - v1;

		Vector3 tangent;
		if(uv1.y == uv2.y)
			tangent = (uv1.x < uv2.x)?v1v2:(-1*v1v2);
		else{
			tangent = v0v1 + ((uv1.y - uv0.y)/(uv1.y - uv2.y))*v1v2;

			if(uv1.y > uv2.y)
				tangent = -1.0f * tangent;
		}

		tangent.Normalize();

		tangentData[indexData[3*i + 0]] = tangent;
		tangentData[indexData[3*i + 1]] = tangent;
		tangentData[indexData[3*i + 2]] = tangent;
	}
}

void Mesh::CalculateBitangents()
{
	for(int i = 0; i < numTriangles; ++i){
		Vector3 normal = normalData[indexData[3*i + 0]];
		Vector3 tangent = tangentData[indexData[3*i + 0]];
		Vector3 bitangent = normal.Cross(tangent);

		bitangent.Normalize();
		
		bitangentData[indexData[3*i + 0]] = bitangent;
		bitangentData[indexData[3*i + 1]] = bitangent;
		bitangentData[indexData[3*i + 2]] = bitangent;
	}
}

void Mesh::CalculateUVs(UVMappingMode uvMode)
{
	switch(uvMode)
	{
	case SphereUV:
		{
			for(int i = 0; i < numVertices; ++i)
			{
				Vector3 posDir = positionData[i].Normalized();

				float u = (atan2(posDir.z, posDir.x) + PI) / (2*PI);
				float v = -0.5f * (posDir.y - 1.0f);

				uvData[i] = Vector2(u, v);
			}
			break;
		}
	}
}

void Mesh::calculateBoundingBox()
{
	boundingBox.max = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	boundingBox.min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for(int i = 0; i < numVertices; ++i)
	{
		if(positionData[i].x > boundingBox.max.x)
			boundingBox.max.x = positionData[i].x;
		if(positionData[i].x < boundingBox.min.x)
			boundingBox.min.x = positionData[i].x;


		if(positionData[i].y > boundingBox.max.y)
			boundingBox.max.y = positionData[i].y;
		if(positionData[i].y < boundingBox.min.y)
			boundingBox.min.y = positionData[i].y;

		if(positionData[i].z > boundingBox.max.z)
			boundingBox.max.z = positionData[i].z;
		if(positionData[i].z < boundingBox.min.z)
			boundingBox.min.z = positionData[i].z;
	}
}


void Mesh::LoadDataFromFile(std::string filename, MeshFileFormat format)
{
	std::fstream fin(filename.c_str());

	if(!fin)
		log("load file \"%s\" failed!\n", filename.c_str());

	switch(format)
	{
	case OBJ:
		{
			std::vector<Vector3> filePosData;
			std::vector<DWORD> fileIndexData;

			char line[100];
			while(!fin.eof())
			{
				fin.getline(line, 100);
				OBJParseLine(line, filePosData, fileIndexData);
			}

			numVertices = filePosData.size();
			numTriangles = fileIndexData.size() / 3;

			positionData = new Vector3[numVertices];
			uvData = new Vector2[numVertices];
			normalData = new Vector3[numVertices];
			tangentData = new Vector3[numVertices];
			bitangentData = new Vector3[numVertices];
			indexData = new DWORD[3*numTriangles];

			for(int i = 0; i < numVertices; ++i)
			{
				positionData[i] = filePosData[i];
			}

			for(int i = 0; i < 3*numTriangles; ++i)
			{
				indexData[i] = fileIndexData[i];
			}

			break;
		}
	}
}


void Mesh::OBJParseLine(char *line, std::vector<Vector3> &filePosData, std::vector<DWORD> &fileIndexData)
{

	char *lineWordPtr;
	lineWordPtr = strtok_s(line, " ", NULL);
	if(!lineWordPtr)
		return;

	switch(lineWordPtr[0])
	{
	case '#':
		return;
		break;
	case 'v':
		{
			Vector3 pos;

			lineWordPtr = strtok_s(NULL, " ", NULL);
			pos.x = (float)atof(lineWordPtr);
			lineWordPtr = strtok_s(NULL, " ", NULL);
			pos.y = (float)atof(lineWordPtr);
			lineWordPtr = strtok_s(NULL, " ", NULL);
			pos.z = (float)atof(lineWordPtr);

			filePosData.push_back(pos);
			break;
		}
	case 'f':
		{
			lineWordPtr = strtok_s(NULL, " ", NULL);
			fileIndexData.push_back(atoi(lineWordPtr) - 1);
			lineWordPtr = strtok_s(NULL, " ", NULL);
			fileIndexData.push_back(atoi(lineWordPtr) - 1);
			lineWordPtr = strtok_s(NULL, " ", NULL);
			fileIndexData.push_back(atoi(lineWordPtr) - 1);
			break;
		}
	}
}

void Mesh::CalculateTBN()
{
	CalculateNormals();
	CalculateTangents();
	CalculateBitangents();
}
