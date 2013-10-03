#include "QuadTreeNode.h"
#include "Camera.h"

QuadTreeNode::QuadTreeNode(float centerX, float centerZ, float halfSize, int depth)
:mCenterX(centerX)
,mCenterZ(centerZ)
,mHalfSize(halfSize)
,mDepth(depth)
,mMinY(-FLT_MAX)
,mMaxY(FLT_MAX)
 {
	if(mDepth > 0)
	{
		mIsLeaf = false;
		mLeftTop = new QuadTreeNode(mCenterX - mHalfSize / 2.0f, mCenterZ + mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mRightTop = new QuadTreeNode(mCenterX + mHalfSize / 2.0f, mCenterZ + mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mLeftBottom = new QuadTreeNode(mCenterX - mHalfSize / 2.0f, mCenterZ - mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
		mRightBottom = new QuadTreeNode(mCenterX + mHalfSize / 2.0f, mCenterZ - mHalfSize / 2.0f, mHalfSize / 2.0f, mDepth - 1);
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
	mAABBox = AABBox(Vector3(mCenterX, (maxY + minY) / 2.0f, mCenterZ), 2.0f * mHalfSize, mMaxY - mMinY, 2.0f * mHalfSize);
}

void QuadTreeNode::EvaluateVisibility(Camera* camera)
{
	_Assert(camera);
	mIsInFrustum = camera->IsVisible(mAABBox);

	if(!mIsInFrustum)
	{
		SetVisible(false);
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

void QuadTreeNode::SetVisible(bool isVisible)
{
	mIsInFrustum = isVisible;

	if(!mIsInFrustum && mIsLeaf)
	{
		mLeftTop->SetVisible(false);
		mRightTop->SetVisible(false);
		mLeftBottom->SetVisible(false);
		mRightBottom->SetVisible(false);
	}
}