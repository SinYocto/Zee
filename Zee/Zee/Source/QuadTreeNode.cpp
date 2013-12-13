#include "QuadTreeNode.h"
#include "Camera.h"

QuadTreeNode::QuadTreeNode(float centerX, float centerZ, float halfSize, int depth)
:mCenterX(centerX)
,mCenterZ(centerZ)
,mHalfSize(halfSize)
,mDepth(depth)
,mMinY(-100.0f)
,mMaxY(100.0f)
{
	CalculateBoundingBox(mMinY, mMaxY);		// 初始化的quadtreeNode的Y轴范围
	
	if(mDepth > 0)
	{
		mIsLeaf = false;
		mLeftTop = New QuadTreeNode(mCenterX - mHalfSize / 2.0f, mCenterZ + mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mRightTop = New QuadTreeNode(mCenterX + mHalfSize / 2.0f, mCenterZ + mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mLeftBottom = New QuadTreeNode(mCenterX - mHalfSize / 2.0f, mCenterZ - mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mRightBottom = New QuadTreeNode(mCenterX + mHalfSize / 2.0f, mCenterZ - mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
	}
	else
	{
		mIsLeaf = true;
		mLeftTop = NULL;
		mRightTop = NULL;
		mLeftBottom = NULL;
		mRightBottom = NULL;
	}
}

int QuadTreeNode::GetDepth()
{
	return mDepth;
}

Vector2 QuadTreeNode::GetCenter()
{
	return Vector2(mCenterX, mCenterZ);
}

float QuadTreeNode::GetHalfSize()
{
	return mHalfSize;
}

QuadTreeNode* QuadTreeNode::GetLeftTop()
{
	return mLeftTop;
}

QuadTreeNode* QuadTreeNode::GetRightTop()
{
	return mRightTop;
}

QuadTreeNode* QuadTreeNode::GetLeftBottom()
{
	return mLeftBottom;
}

QuadTreeNode* QuadTreeNode::GetRightBottom()
{
	return mRightBottom;
}

void QuadTreeNode::CalculateBoundingBox(float minY, float maxY)
{
	mMinY = minY;
	mMaxY = maxY;
	mAABBox = AABBox(Vector3(mCenterX, (maxY + minY) / 2.0f, mCenterZ), 2.0f * mHalfSize, maxY - minY, 2.0f * mHalfSize);
}

void QuadTreeNode::EvaluateVisibility(Camera* camera)
{
	_Assert(camera);
	mIsInFrustum = camera->IsVisible(mAABBox);

	if(!mIsInFrustum)
	{
		setVisible(false);
	}
	else
	{
		if(mLeftTop)
			mLeftTop->EvaluateVisibility(camera);
		if(mRightTop)
			mRightTop->EvaluateVisibility(camera);
		if(mLeftBottom)
			mLeftBottom->EvaluateVisibility(camera);
		if(mRightBottom)
			mRightBottom->EvaluateVisibility(camera);
	}
}

void QuadTreeNode::setVisible(bool isVisible)
{
	mIsInFrustum = isVisible;

	if(!mIsInFrustum && !mIsLeaf)	// invisible时需要设置子node也为invisible
	{
		mLeftTop->setVisible(false);
		mRightTop->setVisible(false);
		mLeftBottom->setVisible(false);
		mRightBottom->setVisible(false);
	}
}

bool QuadTreeNode::IsInFrustum()
{
	return mIsInFrustum;
}

AABBox QuadTreeNode::GetAABBox()
{
	return mAABBox;
}
