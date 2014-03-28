#include "Tree.h"
#include "Engine.h"
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
	for(std::vector<TreeStem*>::iterator iter = mBranches.begin(); iter != mBranches.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}

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

	for(std::vector<TreeStem*>::iterator iter = mBranches.begin(); iter != mBranches.end(); ++iter)
	{
		(*iter)->Draw(matWorld, camera);
	}

	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->Draw(matWorld, camera);
	}
}

void TreeSegment::OnLostDevice()
{
	if(mGeo)
		mGeo->OnLostDevice();

	for(std::vector<TreeStem*>::iterator iter = mBranches.begin(); iter != mBranches.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}

	for(std::vector<TreeSegment*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void TreeSegment::OnResetDevice()
{
	if(mGeo)
		mGeo->OnResetDevice();

	for(std::vector<TreeStem*>::iterator iter = mBranches.begin(); iter != mBranches.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}

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
,mNumBranches(0)
,mLevelParams(level)
,mParentSeg(NULL)
,mFirstSeg(NULL)
,mTree(tree)
{

}

float TreeStem::calcStemLength(TreeGeneralParams generalParams, float parentLength, float stemOffset)
{
	float stemLength = 0;
	if(mLevelParams.level == 0)
	{
		stemLength = RandomVariation(generalParams.scale, generalParams.scaleV) * 
			RandomVariation(mLevelParams.length, mLevelParams.lengthV);
	}
	else if(mLevelParams.level == 1)
	{
		float baseLength = generalParams.baseSize * parentLength;

		stemLength = RandomVariation(mLevelParams.length, mLevelParams.lengthV) * parentLength *
			ShapeRatio(generalParams.shape, (parentLength - stemOffset) / (parentLength - baseLength));
	}
	else
	{
		stemLength = RandomVariation(mLevelParams.length, mLevelParams.lengthV) * (parentLength - 0.6f * stemOffset);
	}

	return stemLength;
}

float TreeStem::calcStemBaseRadius(TreeGeneralParams generalParams, float parentLength, float stemOffset)
{
	float baseRadius = 0;
	if(mLevelParams.level == 0)
	{
		baseRadius = mLength * generalParams.radiusRatio;
	}
	else
	{
		baseRadius = mParentSeg->mStem->GetBaseRadius() * pow((mLength / parentLength), generalParams.ratioPower);

		float maxRadius = mParentSeg->mStem->GetStemRadius(stemOffset / parentLength);
		if(baseRadius > maxRadius)
			baseRadius = maxRadius;
	}

	return baseRadius;
}

void TreeStem::calcStemBranchInterval(float parentLength, float stemOffset, float baseLength,
									  int* numBranches, float* branchInterval)
{
	_Assert(NULL != numBranches);
	_Assert(NULL != branchInterval);

	if(mLevelParams.level == 0)
	{
		*numBranches = mLevelParams.branches;
	}
	else if(mLevelParams.level == 1)
	{
		float lengthChildMax = RandomVariation(mLevelParams.length, mLevelParams.lengthV) * parentLength;
		*numBranches = (int)(mLevelParams.branches * (0.2f + 0.8f * mLength / parentLength / lengthChildMax));
	}
	else
	{
		*numBranches = (int)(mLevelParams.branches * (1.0f - 0.5f * stemOffset / parentLength));
	}

	*branchInterval = (mLength - baseLength) / (*numBranches + 1);
}

void TreeStem::calcBranchAngle(float parentLength, float stemOffset, float baseLength, float curRotateAngle, 
							   float* downAngle, float* rotateAngle)
{
	_Assert(NULL != downAngle);
	_Assert(NULL != rotateAngle);

	if(mLevelParams.level == 0 || mLevelParams.downAngleV >= 0)
	{
		*downAngle = RandomVariation(mLevelParams.downAngle, mLevelParams.downAngleV);
	}
	else
	{
		*downAngle = RandomVariation(mLevelParams.downAngle, 
			mLevelParams.downAngleV * (1.0f - 2.0f * (ShapeRatio(0, (parentLength - stemOffset) / (parentLength - baseLength)))));
	}

	*rotateAngle = curRotateAngle + RandomVariation(mLevelParams.rotateAngle, mLevelParams.rotateAngleV);
}

void TreeStem::Build(TreeSegment* parentSeg, float offset, TreeGeneralParams generalParams, TreeLevelParams levelParams, 
					 LevelContext& context)
{
	static int dbNumStems[4];
	dbNumStems[levelParams.level]++;
	ConsolePrint(L"current numStems: (0-%d),(1-%d),(2-%d),(3-%d)\n", dbNumStems[0], dbNumStems[1], dbNumStems[2], dbNumStems[3]);

	SAFE_DELETE(mFirstSeg);
	mParentSeg = parentSeg;
	mLevelParams = levelParams;

	float parentLength = 0;
	float stemOffset = 0;

	if(parentSeg)
	{
		mPos = Vector3(0, offset, 0);		// offset: 当stem是parentSeg的一个枝条时, stem在parentSeg上长出的位置

		parentLength = mParentSeg->mStem->GetLength();
		stemOffset = mParentSeg->mSegIndex * mParentSeg->mStem->GetSegLength() + offset;
	}
	else
	{
		_Assert(mLevelParams.level == 0);	// 只有trunk不存在parentSeg
		mPos = Vector3::Zero;
	}

	mLength = calcStemLength(generalParams, parentLength, stemOffset);
	mBaseRadius = calcStemBaseRadius(generalParams, parentLength, stemOffset);

	float baseLength = 0;
	if(mLevelParams.level == 0)
	{
		baseLength = generalParams.baseSize * mLength;
	}
	calcStemBranchInterval(parentLength, stemOffset, baseLength, &mNumBranches, &mBranchInterval);

	float parentBaseLength = 0;
	if(mLevelParams.level == 1)
	{
		parentBaseLength = generalParams.baseSize * parentLength;
	}

	float downAngle = 0;
	float rotateAngle = 0;
	calcBranchAngle(parentLength, stemOffset, parentBaseLength, context.rotateAngle, &downAngle, &rotateAngle);

	context.rotateAngle = rotateAngle;

	mOrient = Quaternion(0, DEGREE_TO_RAD(rotateAngle), DEGREE_TO_RAD(downAngle));

	buildSegment(generalParams, NULL, 0, 0, 0, 0, LevelContext());
}

void TreeStem::buildSegment(TreeGeneralParams generalParams, TreeSegment* prevSeg, int segIndex, 
							float splitAngle, float divergeAngle, float rotateYAngle, LevelContext& context)
{
	_Assert(segIndex < mLevelParams.curveRes);

	static int dbNumSegs[4];
	dbNumSegs[mLevelParams.level]++;
	ConsolePrint(L"current numSegs: (0-%d),(1-%d),(2-%d),(3-%d)\n", dbNumSegs[0], dbNumSegs[1], dbNumSegs[2], dbNumSegs[3]);

	int numSegs = mLevelParams.curveRes;
	float segLength = mLength / numSegs;

	TreeSegment* seg = New TreeSegment;

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
	seg->mOrientWorld = Quaternion(0, DEGREE_TO_RAD(rotateYAngle), 0) * (parentOrientWorld * seg->mOrient);
	seg->mOrient = WorldRotationToLocal(parentOrientWorld.Difference(seg->mOrientWorld), parentOrientWorld);

	// create seg vb ...
	std::vector<float> segRadius;
	{
		float offset = (float)segIndex / numSegs;
		float offsetDelta = 1.0f / numSegs / mLevelParams.segSegsH;
		for(int i = 0; i <= mLevelParams.segSegsH; ++i)
		{
			segRadius.push_back(GetStemRadius(offset));
			offset += offsetDelta;
		}
	}

	bool closeTop = true;
	bool closeBottom = true;
	//if(segIndex == 0)
	//	closeBottom = true;
	//if(segIndex == numSegs - 1)
	//	closeTop = true;

	seg->mGeo = New TreeSegGeo(segLength, mLevelParams.segSegsW, mLevelParams.segSegsH, segRadius, 
 		Quaternion(0, 0, DEGREE_TO_RAD(curveDelta + splitAngle)), closeTop, closeBottom);
	seg->mGeo->CalculateNormals();
	seg->mGeo->BuildGeometry(XYZ_N);

	// branches
	if(seg->mStem->GetNumBranches() != 0 && mLevelParams.level < generalParams.levels - 1)
	{
		float baseOffset = 0;
		if(mLevelParams.level == 0)
		{
			float baseLength = generalParams.baseSize * mLength;
			baseOffset = baseLength - segIndex * segLength;
		}

		float offset = 0;
		if(baseOffset <= 0)
		{
			offset = mBranchInterval - context.branchDistError;
		}
		else if(baseOffset < segLength)
		{
			offset = baseOffset + mBranchInterval;
		}
		else
		{
			offset = segLength + mBranchInterval;
		}

		while(offset < segLength - 0.0001f)
		{
			TreeStem* branch = New TreeStem(mTree, mLevelParams.level + 1);
			branch->Build(seg, offset, generalParams, mTree->GetLevelParams(mLevelParams.level + 1), context);
			seg->mBranches.push_back(branch);

			offset += mBranchInterval;
		}

		context.branchDistError = segLength - (offset - mBranchInterval);
	}

	// splits
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
		Vector3 vecUp = Vector3(0, 1, 0) * seg->mOrientWorld;
		float declination = VectorAngle(Vector3(0, 1, 0), vecUp);

		float splitAngleFactor = 1.0f;		// 调整此算法来使splitAngle随枝干的水平角度加成变化
		splitAngleFactor = fabs(declination - PI/2) / (PI/2);

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

float TreeStem::GetLength()
{
	return mLength;
}

TreeStem::~TreeStem()
{
	SAFE_DELETE(mFirstSeg);
}

void TreeStem::Draw(D3DXMATRIX matParent, Camera* camera)
{
	if(mFirstSeg)
	{
		D3DXMATRIX matRot, matTrans, matWorld;
		matRot = mOrient.Matrix();
		D3DXMatrixTranslation(&matTrans, mPos.x, mPos.y, mPos.z);
		matWorld = matRot * matTrans * matParent;

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

float TreeStem::ShapeRatio(int shape, float ratio)
{
	float resultVal = 0;

	switch(shape)
	{
	case 0:		// conical
		resultVal = 0.2f + 0.8f * ratio;
		break;

	case 1:		// spherical
		resultVal = 0.2f + 0.8f * sin(PI * ratio);
		break;

	case 2:		// hemispherical
		resultVal = 0.2f + 0.8f * sin(0.5f * PI * ratio);
		break;

	case 3:		// cylindrical
		resultVal = 1.0f;
		break;

	case 4:		// taped cylindrical
		resultVal = 0.5f + 0.5f * ratio;
		break;

	case 5:		// flame
		if(ratio <= 0.7f)
			resultVal = ratio / 0.7f;
		else
			resultVal = (1.0f - ratio) / 0.3f;
		break;

	case 6:		// inverse conical
		resultVal = 1.0f - 0.8f * ratio;
		break;

	case 7:		// tend flame
		if(ratio <= 0.7f)
			resultVal = 0.5f + 0.5f * ratio / 0.7f;
		else
			resultVal = 0.5f + 0.5f * (1.0f - ratio) / 0.3f;
		break;

	default:
		_Assert(false);
		break;
	}

	return resultVal;
}

float TreeStem::GetSegLength()
{
	return mLength / mLevelParams.curveRes;
}

float TreeStem::GetBaseRadius()
{
	return mBaseRadius;
}

int TreeStem::GetNumBranches()
{
	return mNumBranches;
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
	mGeoData.posData.push_back(Vector3::Zero);
	mGeoData.posData.push_back(Vector3(0, mLength, 0));

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

			mGeoData.posData.push_back(vertPos);
		}
	}

	// verts
	for(size_t i = 0; i < mGeoData.posData.size(); ++i)
	{
		Vert vert(i);
		mGeoData.verts.push_back(vert);
	}

	// triangles
	if(bottomPlane)
	{
		for(int i = 0; i < mSegmentsW; ++i)		// 底圆面
		{
			// 顶点索引为2 ~ sw+1
			Triangle triangle(0, 2+i, 2 + (i + 1) % mSegmentsW);
			mGeoData.tris.push_back(triangle);
		}
	}

	if(topPlane)
	{
		for(int i = 0; i < mSegmentsW; ++i)		// 顶圆面
		{
			// 顶点索引为sw*sh + 2 ~ sw*(sh + 1) + 1
			Triangle triangle(1, mSegmentsW*mSegmentsH + 2 + (i + 1) % mSegmentsW, mSegmentsW*mSegmentsH + 2 + i);
			mGeoData.tris.push_back(triangle);
		}
	}

	for(int i = 0; i < mSegmentsH; ++i)		// 柱面
	{
		for(int j = 0; j < mSegmentsW; ++j)		
		{
			Triangle tri1;
			Triangle tri2;

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

			mGeoData.tris.push_back(tri1);
			mGeoData.tris.push_back(tri2);
		}
	}
}

void TreeSegGeo::Render(D3DXMATRIX matWorld, Camera* camera)
{
	gEngine->GetMaterialManager()->GetDefaultDiffMtl()->Render(matWorld, this, camera);
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

	mTrunk = New TreeStem(this, 0);
	mTrunk->Build(NULL, 0, mGeneralParams, mLevelParams[0], LevelContext());
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
		mTrunk->Draw(IDENTITY_MATRIX, camera);
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

void Tree::LoadParamsFromFile(const wchar_t* filePath)
{
	mLevelParams.clear();

	TreeGeneralParams generalParams;

	generalParams.levels = GetPrivateProfileInt(L"GeneralParams", L"Levels", 0, filePath);
	generalParams.shape = GetPrivateProfileInt(L"GeneralParams", L"Shape", 0, filePath);
	generalParams.baseSplits = GetPrivateProfileInt(L"GeneralParams", L"BaseSplits", 0, filePath);

	wchar_t strTemp[MAX_STR_LEN];

	GetPrivateProfileString(L"GeneralParams", L"Scale", L"10.0", strTemp, MAX_STR_LEN, filePath);
	generalParams.scale = (float)_wtof(strTemp);

	GetPrivateProfileString(L"GeneralParams", L"ScaleV", L"0", strTemp, MAX_STR_LEN, filePath);
	generalParams.scaleV = (float)_wtof(strTemp);

	GetPrivateProfileString(L"GeneralParams", L"BaseSize", L"0", strTemp, MAX_STR_LEN, filePath);
	generalParams.baseSize = (float)_wtof(strTemp);

	GetPrivateProfileString(L"GeneralParams", L"RadiusRatio", L"0.02", strTemp, MAX_STR_LEN, filePath);
	generalParams.radiusRatio = (float)_wtof(strTemp);

	GetPrivateProfileString(L"GeneralParams", L"RatioPower", L"1", strTemp, MAX_STR_LEN, filePath);
	generalParams.ratioPower = (float)_wtof(strTemp);

	mGeneralParams = generalParams;

	for(int i = 0; i < generalParams.levels; ++i)
	{
		TreeLevelParams levelParams(i);

		wchar_t strLevelSection[MAX_STR_LEN];
		YString::Format(strLevelSection, L"LevelParams%d", i);

		levelParams.segSegsW = GetPrivateProfileInt(strLevelSection, L"SegSegsW", 8, filePath);
		levelParams.segSegsH = GetPrivateProfileInt(strLevelSection, L"SegSegsH", 4, filePath);
		levelParams.branches = GetPrivateProfileInt(strLevelSection, L"Branches", 0, filePath);
		levelParams.curveRes = GetPrivateProfileInt(strLevelSection, L"CurveRes", 3, filePath);

		GetPrivateProfileString(strLevelSection, L"SegSplits", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.segSplits = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"SplitAngle", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.splitAngle = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"SplitAngleV", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.splitAngleV = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"DownAngle", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.downAngle = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"DownAngleV", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.downAngleV = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"RotateAngle", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.rotateAngle = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"RotateAngleV", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.rotateAngleV = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"Length", L"1", strTemp, MAX_STR_LEN, filePath);
		levelParams.length = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"LengthV", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.lengthV = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"Taper", L"1", strTemp, MAX_STR_LEN, filePath);
		levelParams.taper = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"Curve", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.curve = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"CurveV", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.curveV = (float)_wtof(strTemp);

		GetPrivateProfileString(strLevelSection, L"CurveBack", L"0", strTemp, MAX_STR_LEN, filePath);
		levelParams.curveBack = (float)_wtof(strTemp);

		mLevelParams.push_back(levelParams);
	}
}
