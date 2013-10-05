#ifndef QUADTREE_NODE_H
#define QUADTREE_NODE_H

#include "Common.h"
#include "Bound.h"

class Camera;

class QuadTreeNode
{
public:
	QuadTreeNode(float centerX, float centerZ, float halfSize, int depth);
	~QuadTreeNode()
	{
		SAFE_DELETE(mLeftTop);
		SAFE_DELETE(mRightTop);
		SAFE_DELETE(mLeftBottom);
		SAFE_DELETE(mRightBottom);
	}

	void EvaluateVisibility(Camera* camera);

	int GetDepth();
	Vector2 GetCenter();
	float GetHalfSize();

	QuadTreeNode* GetLeftTop();
	QuadTreeNode* GetRightTop();
	QuadTreeNode* GetLeftBottom();
	QuadTreeNode* GetRightBottom();

public:
	void CalculateBoundingBox(float minY, float maxY);
	bool IsInFrustum();
	AABBox GetAABBox();

private:
	void setVisible(bool isVisible);

private:
	float mCenterX;
	float mCenterZ;
	float mHalfSize;

	float mMinY;
	float mMaxY;

	int mDepth;
	bool mIsLeaf;
	bool mIsInFrustum;

	QuadTreeNode* mLeftTop;
	QuadTreeNode* mRightTop;
	QuadTreeNode* mLeftBottom;
	QuadTreeNode* mRightBottom;

	AABBox mAABBox;
};

#endif