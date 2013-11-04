#include "Tree.h"
#include "MaterialManager.h"
#include "Camera.h"

TreeSegment::TreeSegment()
:mPos(Vector3::Zero)
,mOrient(Quaternion(1, 0, 0, 0))
,mOrientWorld(Quaternion(1, 0, 0, 0))
,mSegIndex(0)
,mStem(NULL)
,mParent(NULL)
,mGeo(NULL)
{

}

TreeSegment::~TreeSegment()
{
	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}

	SAFE_DELETE(mGeo);
}

void TreeSegment::Draw(const D3DXMATRIX& matParent, Camera* camera)
{
	D3DXMATRIX matRot, matTrans, matWorld;
	matRot = mOrient.Matrix();
	D3DXMatrixTranslation(&matTrans, mPos.x, mPos.y, mPos.z);
	matWorld = matRot * matTrans * matParent; 

	mGeo->Render(matWorld, camera);

	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->Draw(matWorld, camera);
	}
}

void TreeSegment::OnLostDevice()
{
	if(mGeo)
		mGeo->OnLostDevice();

	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void TreeSegment::OnResetDevice()
{
	if(mGeo)
		mGeo->OnResetDevice();

	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}

TreeStem::TreeStem(Tree* tree, int level)
:mPos(Vector3::Zero)
,mOrient(Quaternion(1, 0, 0, 0))
,mLength(0)
,mBaseRadius(0)
,mLevelParams(level)
,mFirstSeg(NULL)
,mTree(tree)
{

}

void TreeStem::Build(TreeGeneralParams generalParams, TreeLevelParams levelParams, LevelContext& context)
{
	SAFE_DELETE(mFirstSeg);

	mPos = Vector3::Zero;
	mOrient = Quaternion(DEGREE_TO_RAD(RandomVariation(levelParams.downAngle, levelParams.downAngleV)), 
		DEGREE_TO_RAD(RandomVariation(levelParams.rotateAngle, levelParams.rotateAngleV)), 0);

	mLength = RandomVariation(generalParams.scale, generalParams.scaleV) * 
		RandomVariation(levelParams.length, levelParams.lengthV);

	mBaseRadius = mLength * generalParams.radiusRatio;

	mLevelParams = levelParams;

	buildSegment(generalParams, NULL, 0, 0, 0, 0, context);
}

void TreeStem::buildSegment(TreeGeneralParams generalParams, TreeSegment* prevSeg, int segIndex, 
							float splitAngle, float divergeAngle, float rotateYAngle, LevelContext& context)
{
	_Assert(segIndex < mLevelParams.curveRes);

	int numSegs = mLevelParams.curveRes;
	float segLength = mLength / numSegs;

	TreeSegment* seg = new TreeSegment;

	float curveDelta = 0;
	if(segIndex != 0)
	{
		if(mLevelParams.curveBack == 0)
		{
			curveDelta = mLevelParams.curve / mLevelParams.curveRes;
		}
		else
		{
			if(segIndex < (mLevelParams.curveRes + 1) / 2)
				curveDelta = 2 * mLevelParams.curve / mLevelParams.curveRes;
			else
				curveDelta = -2 * mLevelParams.curveBack / mLevelParams.curveRes;
		}

		curveDelta += RandomVariation(0, mLevelParams.curveV) / mLevelParams.curveRes;
	}

	Vector3 basePos;
	if(prevSeg == NULL)
		basePos = Vector3::Zero;
	else
		basePos = Vector3(0, segLength, 0);

	seg->mPos = basePos;
	seg->mOrient = Quaternion(0, DEGREE_TO_RAD(divergeAngle), DEGREE_TO_RAD(curveDelta + splitAngle));
	//seg->mOrient = Quaternion(0, 0, DEGREE_TO_RAD(curveDelta + splitAngle));
	seg->mSegIndex = segIndex;
	seg->mParent = prevSeg;
	seg->mStem = this;

	Quaternion parentOrientWorld;
	if(prevSeg)
	{
		prevSeg->mChildren.push_back(seg);
		parentOrientWorld = prevSeg->mOrientWorld;
	}
	else
	{
		mFirstSeg = seg;
		parentOrientWorld = mOrient;
	}

	// divergeAngle是绕世界的y轴旋转
	//seg->mOrientWorld = seg->mOrient * parentOrientWorld;
	//seg->mOrient = WorldRotationToLocal(Quaternion(0, DEGREE_TO_RAD(divergeAngle), 0), seg->mOrientWorld) * seg->mOrient;
	//seg->mOrientWorld = seg->mOrient * parentOrientWorld;


	//seg->mOrientWorld = Quaternion(0, DEGREE_TO_RAD(rotateYAngle), 0) * (seg->mOrient * parentOrientWorld);
	//seg->mOrient = WorldRotationToLocal(parentOrientWorld.Difference(seg->mOrientWorld), parentOrientWorld);
	//seg->mOrientWorld = (seg->mOrient * parentOrientWorld);

	Vector3 euler0 = seg->mOrient.EulerAngle();
	Vector3 up0 = Vector3(0, 1.0f, 0) * seg->mOrient;
	Vector3 right = Vector3(1, 0, 0) * seg->mOrient;
	Vector3 forward = Vector3(0, 0, 1) * seg->mOrient;

	seg->mOrientWorld = Quaternion(0, DEGREE_TO_RAD(rotateYAngle), 0) * (parentOrientWorld * seg->mOrient);
	seg->mOrient = WorldRotationToLocal(parentOrientWorld.Difference(seg->mOrientWorld), parentOrientWorld);

	Vector3 up = Vector3(0, 1.0f, 0) * seg->mOrientWorld;
	Vector3 euler = seg->mOrientWorld.EulerAngle();

	// create seg vb ...
	std::vector<float> segRadius;
	float offset = (float)segIndex / numSegs;
	float offsetDelta = 1.0f / numSegs / mLevelParams.segSegsH;
	for(int i = 0; i <= mLevelParams.segSegsH; ++i)
	{
		segRadius.push_back(GetStemRadius(offset));
		offset += offsetDelta;
	}

	bool closeTop = true;
	bool closeBottom = true;
	//if(segIndex == 0)
	//	closeBottom = true;
	//if(segIndex == numSegs - 1)
	//	closeTop = true;

	seg->mGeo = new TreeSegGeo(segLength, mLevelParams.segSegsW, mLevelParams.segSegsH, segRadius, 
 		Quaternion(0, 0, DEGREE_TO_RAD(curveDelta + splitAngle)), closeTop, closeBottom);
	seg->mGeo->CalculateNormals();
	seg->mGeo->BuildGeometry(XYZ_N);

	if(segIndex == numSegs - 1)
		return;

	int numSplits = 0;
	if(mLevelParams.level == 0 && segIndex == 0)
	{
		numSplits = generalParams.baseSplits;
	}
	else
	{
		numSplits = (int)(mLevelParams.segSplits + context.splitError);
		context.splitError = mLevelParams.segSplits + context.splitError - numSplits;
	}

	if(numSplits < 1)		// no splits
	{
		buildSegment(generalParams, seg, segIndex + 1, 0, 0, 0, context);
	}
	else
	{			
		Vector3 vecUp = Vector3(0, 1, 0) * seg->mOrient;
		float declination = VectorAngle(Vector3(0, 1, 0), vecUp);

		float splitAngleFactor = 1.0f;		// 调整此算法来使splitAngle随枝干的水平角度加成变化
		//splitAngleFactor = fabs(declination - PI/2) / (PI/2);

		float childSplitAngle = max(0, splitAngleFactor * RandomVariation(mLevelParams.splitAngle, mLevelParams.splitAngleV));	

		for(int i = 0; i <= numSplits; ++i)
		{
			float childDivergeAngle = 0;
			float childRotateYAngle = 0;

			childDivergeAngle = ((float)i / (numSplits + 1)) * 360;
			if(mLevelParams.level == 0 && segIndex == 0)
			{
				childRotateYAngle = 0;
			}
			else
			{
				float rf = RandomFloat(0, 1.0f);
				int s = sign(RandomFloat(-1.0f, 1.0f));
				childRotateYAngle = s * (20 + 0.75f * (30 + fabs(declination - 90)) * pow(rf, 2));
			}

			buildSegment(generalParams, seg, segIndex + 1, childSplitAngle, childDivergeAngle, childRotateYAngle, context);
		}
	}
}

float TreeStem::GetStemRadius(float offset)
{
	float Z = min(offset, 1.0f);

	float taper = mLevelParams.taper;
	float unitTaper = 0;

	if(taper <= 1) 
		unitTaper = taper;
	else if(taper <=2) 
		unitTaper = 2 - taper;
	else
		unitTaper = 0;

	float radius = mBaseRadius * (1 - unitTaper * Z);

	float depth = 0;
	if(taper > 1)
	{
		float Z2 = (1 - Z) * mLength;
		if(taper < 2 || Z2 < radius)
			depth = 1;
		else
			depth = taper - 2;

		float Z3 = 0;
		if(taper < 2)
			Z3 = Z2;
		else
			Z3 = fabs(Z2 - 2 * radius * (int)(Z2 / 2 / radius + 0.5));

		if(taper > 2 || Z3 < radius)
			radius = (1 - depth) * radius + depth * sqrt(radius * radius - (Z3 - radius) * (Z3 - radius));
	}

	return radius;
}

TreeStem::~TreeStem()
{
	SAFE_DELETE(mFirstSeg);
}

void TreeStem::Draw(Camera* camera)
{
	if(mFirstSeg)
	{
		D3DXMATRIX matRot, matTrans, matWorld;
		matRot = mOrient.Matrix();
		D3DXMatrixTranslation(&matTrans, mPos.x, mPos.y, mPos.z);
		matWorld = matRot * matTrans;

		mFirstSeg->Draw(matWorld, camera);
	}
}

void TreeStem::OnLostDevice()
{
	if(mFirstSeg)
		mFirstSeg->OnLostDevice();
}

void TreeStem::OnResetDevice()
{
	if(mFirstSeg)
		mFirstSeg->OnResetDevice();
}

TreeSegGeo::TreeSegGeo(float length, int segmentsW, int segmentsH, std::vector<float> radius, Quaternion orient,
					   bool topPlane, bool bottomPlane)
:Geometry(L"TreeSegGeo")
,mLength(length)
,mRadius(radius)
,mSegmentsW(segmentsW)
,mSegmentsH(segmentsH)
{
	_Assert(mRadius.size() == mSegmentsH + 1);

	constructGeometryData(orient, topPlane, bottomPlane);
}

void TreeSegGeo::constructGeometryData(Quaternion orient, bool topPlane, bool bottomPlane)
{
	int numVerts = mSegmentsW * (mSegmentsH + 1) + 2;

	// positionData
	mPositionData.push_back(Vector3::Zero);
	mPositionData.push_back(Vector3(0, mLength, 0));

	float heightDelta = mLength / mSegmentsH;
	float deltaTheta = 2*PI / mSegmentsW;

	for(int i = 0; i <= mSegmentsH; ++i)
	{
		float circleHeight = i * heightDelta;
		float circleRadius = mRadius[i];
		for(int j = 0; j < mSegmentsW; ++j)
		{
			float x = circleRadius * cos(j*deltaTheta);
			float z = circleRadius * sin(j*deltaTheta);

			Vector3 vertPos(x, circleHeight, z);
			if(i == 0)
				vertPos = vertPos * orient.Conjugate();

			mPositionData.push_back(vertPos);
		}
	}

	// verts
	for(size_t i = 0; i < mPositionData.size(); ++i)
	{
		Geometry::Vert vert(i);
		mVerts.push_back(vert);
	}

	// triangles
	if(bottomPlane)
	{
		for(int i = 0; i < mSegmentsW; ++i)		// 底圆面
		{
			// 顶点索引为2 ~ sw+1
			Geometry::Triangle triangle(0, 2+i, 2 + (i + 1) % mSegmentsW);
			mTriangles.push_back(triangle);
		}
	}

	if(topPlane)
	{
		for(int i = 0; i < mSegmentsW; ++i)		// 顶圆面
		{
			// 顶点索引为sw*sh + 2 ~ sw*(sh + 1) + 1
			Geometry::Triangle triangle(1, mSegmentsW*mSegmentsH + 2 + (i + 1) % mSegmentsW, mSegmentsW*mSegmentsH + 2 + i);
			mTriangles.push_back(triangle);
		}
	}

	for(int i = 0; i < mSegmentsH; ++i)		// 柱面
	{
		for(int j = 0; j < mSegmentsW; ++j)		
		{
			Geometry::Triangle tri1;
			Geometry::Triangle tri2;

			tri1.vertexIndex[0] = mSegmentsW * i + j + 2;
			tri1.vertexIndex[1] = mSegmentsW * (i + 1) + j + 2;
			tri1.vertexIndex[2] = mSegmentsW * i + (j + 1)%mSegmentsW + 2;

			tri2.vertexIndex[0] = mSegmentsW * (i + 1) + j + 2;
			tri2.vertexIndex[1] = mSegmentsW * (i + 1) + (j + 1)%mSegmentsW + 2;
			tri2.vertexIndex[2] = mSegmentsW * i + (j + 1)%mSegmentsW + 2;

			_Assert(tri1.vertexIndex[0] < mSegmentsW * (mSegmentsH + 1) + 2);
			_Assert(tri1.vertexIndex[1] < mSegmentsW * (mSegmentsH + 1) + 2);
			_Assert(tri1.vertexIndex[2] < mSegmentsW * (mSegmentsH + 1) + 2);

			_Assert(tri2.vertexIndex[0] < mSegmentsW * (mSegmentsH + 1) + 2);
			_Assert(tri2.vertexIndex[1] < mSegmentsW * (mSegmentsH + 1) + 2);
			_Assert(tri2.vertexIndex[2] < mSegmentsW * (mSegmentsH + 1) + 2);

			mTriangles.push_back(tri1);
			mTriangles.push_back(tri2);
		}
	}
}

void TreeSegGeo::Render(D3DXMATRIX matWorld, Camera* camera)
{
	_Assert(NULL != MaterialManager::viewMtl);
	MaterialManager::viewMtl->Render(matWorld, this, camera);
}

Tree::Tree()
:mTrunk(NULL)
{
	for(int i = 0; i < mGeneralParams.levels; ++i)
	{
		mLevelParams.push_back(TreeLevelParams(i));
	}
}

void Tree::OnParamsLevelsChanged()
{
	int levels = mGeneralParams.levels;
	int curLevels = mLevelParams.size();

	_Assert(levels > 0 && levels < 5);

	if(levels == curLevels)
		return;

	if(levels < curLevels)
	{
		for(int i = curLevels; i > levels; --i)
			mLevelParams.pop_back();
	}
	else
	{
		for(int i = curLevels; i < levels; ++i)
			mLevelParams.push_back(TreeLevelParams(i));
	}
}

void Tree::SetGeneralParams(TreeGeneralParams generalParams)
{
	int curLevels = mGeneralParams.levels;
	mGeneralParams = generalParams;

	if(mGeneralParams.levels != curLevels)
		OnParamsLevelsChanged();
}

void Tree::SetLevelParams(int level, TreeLevelParams levelParams)
{
	_Assert(level >= 0 && level < (int)mLevelParams.size());

	mLevelParams[level] = levelParams;
}

void Tree::Generate()
{
	SAFE_DELETE(mTrunk);

	mTrunk = new TreeStem(this, 0);
	mTrunk->Build(mGeneralParams, mLevelParams[0], LevelContext());
}

void Tree::OnLostDevice()
{
	if(mTrunk)
		mTrunk->OnLostDevice();
}

void Tree::OnResetDevice()
{
	if(mTrunk)
		mTrunk->OnResetDevice();
}

void Tree::Draw(Camera* camera)
{
	if(mTrunk)
		mTrunk->Draw(camera);
}

TreeGeneralParams Tree::GetGeneralParams()
{
	return mGeneralParams;
}

TreeLevelParams Tree::GetLevelParams(int level)
{
	_Assert(level >= 0 && level < (int)mLevelParams.size());
	return mLevelParams[level];
}

void Tree::SetGeneralParamsLevels(int levels)
{
	int curLevels = mGeneralParams.levels;
	mGeneralParams.levels = levels;

	if(mGeneralParams.levels != curLevels)
		OnParamsLevelsChanged();
}
