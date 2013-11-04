#ifndef TREE_H
#define TREE_H

#include "Common.h"
#include "Math.h"
#include "D3DUtility.h"
#include "Geometry.h"

class Camera;

struct LevelContext
{
	LevelContext()
		:splitError(0)
	{

	}

	float splitError;
};

struct TreeLevelParams
{
	TreeLevelParams(int lv = 0)
		:level(lv)
		,segSegsW(8)
		,segSegsH(4)
		,segSplits(0)
		,splitAngle(0)
		,splitAngleV(0)
		,branches(0)
		,downAngle(0)
		,downAngleV(0)
		,rotateAngle(0)
		,rotateAngleV(0)
		,length(1.0f)
		,lengthV(0)
		,taper(1.0f)
		,curveRes(3)
		,curve(0)
		,curveV(0)
		,curveBack(0)
	{

	}

	int level;

	int segSegsW;
	int segSegsH;

	float segSplits;
	float splitAngle;
	float splitAngleV;

	int branches;
	float downAngle;
	float downAngleV;
	float rotateAngle;
	float rotateAngleV;

	float length;			// 相对parent的百分比
	float lengthV;
	float taper;

	int curveRes;
	float curve;
	float curveV;
	float curveBack;
};

struct TreeGeneralParams
{
	TreeGeneralParams()
		:levels(3)
		,shape(0)
		,scale(10.0f)
		,scaleV(0)
		,baseSplits(0)
		,baseSize(0)
		,radiusRatio(0.04f)
		,ratioPower(1.0f)
	{

	}

	int levels;
	int shape;

	float scale;
	float scaleV;

	int baseSplits;
	float baseSize;
	float radiusRatio;
	float ratioPower;
};

class TreeSegGeo : public Geometry
{
public:
	TreeSegGeo(float length, int segmentsW, int segmentsH, std::vector<float> radius, Quaternion orient, 
		bool topPlane, bool bottomPlane);

	void Render(D3DXMATRIX matWorld, Camera* camera);

private:
	void constructGeometryData(Quaternion orient, bool topPlane, bool bottomPlane);

private:
	float mLength;
	std::vector<float> mRadius;

	int mSegmentsW;
	int mSegmentsH;
};

class TreeSegment
{
	friend class TreeStem;
public:
	TreeSegment();
	~TreeSegment();

	void OnLostDevice();
	void OnResetDevice();

	void Draw(const D3DXMATRIX& matParent, Camera* camera);

private:
	Vector3 mPos;
	Quaternion mOrient;
	Quaternion mOrientWorld;

	int mSegIndex;		

	TreeStem* mStem;
	TreeSegment* mParent;
	std::vector<TreeSegment*> mChildren;
	std::vector<TreeStem*> mBranches;

	TreeSegGeo* mGeo;
};

class Tree;
class TreeStem
{
public:
	TreeStem(Tree* tree, int level);
	~TreeStem();

	void OnLostDevice();
	void OnResetDevice();

	float GetStemRadius(float offset);
	void Build(TreeGeneralParams generalParams, TreeLevelParams levelParams, LevelContext& context);

	void Draw(Camera* camera);

private:
	void buildSegment(TreeGeneralParams generalParams, TreeSegment* prevSeg, int segIndex, 
		float splitAngle, float divergeAngle, float rotateYAngle, LevelContext& context);

private:
	Tree* mTree;
	TreeLevelParams mLevelParams;

	float mBaseRadius;
	float mLength;

	TreeSegment* mFirstSeg;

	Vector3 mPos;
	Quaternion mOrient;
};

class Tree
{
public:
	Tree();

	void OnLostDevice();
	void OnResetDevice();

	void Generate();

	TreeGeneralParams GetGeneralParams();
	TreeLevelParams GetLevelParams(int level);
	void SetGeneralParamsLevels(int levels);
	void SetGeneralParams(TreeGeneralParams generalParams);
	void SetLevelParams(int level, TreeLevelParams levelParams);

	void Draw(Camera* camera);

private:
	void OnParamsLevelsChanged();

private:
	TreeGeneralParams mGeneralParams;
	std::vector<TreeLevelParams> mLevelParams;

	TreeStem* mTrunk;

	std::vector<float> mSplitError;
};


#endif