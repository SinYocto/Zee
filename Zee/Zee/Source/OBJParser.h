#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "Math.h"
#include "Geometry.h"
#include "Material.h"
#include "YString.h"
#include "YFile.h"

#include <vector>

class Model;

class OBJParser
{
private:
	enum OBJ_SPECIFIER
	{
		OBJ_COMMENT,
		MTLLIB,
		VERT_POS,
		VERT_UV,
		VERT_NORMAL,
		GROUP,
		MESH_MTL,
		FACE,
		OBJ_OTEHR
	};

	enum MTL_SPECIFIER
	{
		MTL_COMMENT,
		NEW_MTL,
		AMBIENT_COLOR,
		DIFFUSE_COLOR,
		SPEC_COLOR,
		SPEC_GLOSS,
		AMBIENT_TEX,
		DIFFUSE_TEX,
		SPEC_TEX,
		BUMP_TEX,
		MTL_OTHER
	};

	enum DATA_CONTENT
	{
		POS_DATA = 1,
		UV_DATA = 1 << 1,
		NORMAL_DATA = 1 << 2
	};

	struct MaterialContent
	{
		D3DXCOLOR ambientColor;
		D3DXCOLOR diffuseColor;
		D3DXCOLOR specColor;
		float gloss;

		wchar_t ambientTexFilePath[MAX_PATH_LEN];
		wchar_t diffuseTexFilePath[MAX_PATH_LEN];
		wchar_t specTexFilePath[MAX_PATH_LEN];
		wchar_t bumpTexFilePath[MAX_PATH_LEN];

		MaterialContent()
			:ambientColor(0, 0, 0, 1)
			,diffuseColor(1, 1, 1, 1)
			,specColor(1, 1, 1, 1)
			,gloss(50.0f)
		{
			YString::Empty(ambientTexFilePath);
			YString::Empty(diffuseTexFilePath);
			YString::Empty(specTexFilePath);
			YString::Empty(bumpTexFilePath);
		}
	};

public:
	static bool Parse(const wchar_t* filePath, Model** result);

private:
	static void clear();
	static void parseLine(YFile* file, const wchar_t* lineContent);

	static void parseTrianglesBlock(const std::vector<std::wstring>& blockContent);
	static void parseTrianglesBlockLine(const wchar_t* lineContent, Geometry** curGeo, std::map<int, int>& posIndexMap,
		std::map<int, int>& uvIndexMap, std::map<int, int>& normalIndexMap);

	static void parseMtl(const wchar_t* mtlFilePath);
	static void parseMtlBlock(const std::vector<std::wstring>& blockContent);
	static void parseMtlLine(const wchar_t* lineContent, Material** curMaterial, MaterialContent* materialContent);

	static bool getMaterial(const wchar_t* mtlName, Material** material);

	static bool getOBJSpecifier(const wchar_t* lineContent, OBJ_SPECIFIER* specifier);
	static bool getMTLSpecifier(const wchar_t* lineContent, MTL_SPECIFIER* specifier);

	static void determineDataContentType();

private:
	// 解析过程中存的数据和状态
	static DWORD mDataContentType;
	static std::vector<Vector3> mPosData;
	static std::vector<Vector2> uvData;
	static std::vector<Vector3> normalData;

	static std::vector<Geometry*> mGeoList;
	static std::vector<Material*> mMtlList;

	static Model* mResultModel;

};


#endif