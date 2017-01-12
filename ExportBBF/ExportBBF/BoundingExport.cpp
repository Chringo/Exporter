#include "BoundingExport.h"

BoundingExport::BoundingExport()
{

}

BoundingExport::BoundingExport(std::string & filePath)
{

}


BoundingExport::~BoundingExport()
{
}

void BoundingExport::exportBoundingBox(MObject & mNode)
{
	MFnMesh mMesh(MFnTransform(mNode).child(0), NULL);
	MFnTransform mTrans = mNode;


	exportOBB(mMesh, mTrans);
}

int BoundingExport::getProgressBar()
{
	return 0;
}

void BoundingExport::exportOBB(MFnMesh & mMesh, MFnTransform & mTran)
{
	MIntArray indexList, offsetIdList;

	const float* positions = mMesh.getRawPoints(NULL);
	mMesh.getTriangles(offsetIdList, indexList);

	Vector3 m_positions = { 0,0,0 };
	MVector center;
	//MMatrix m_center;
	Vector3 min = { 0,0,0 };
	Vector3 max = { 0,0,0 };

	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		m_positions.x = positions[indexList[i] * 3];
		m_positions.y = positions[indexList[i] * 3 + 1];
		m_positions.z = positions[indexList[i] * 3 + 2];

		if (min.x > m_positions.x)
			min.x = m_positions.x;
		if (min.y > m_positions.y)
			min.y = m_positions.y;
		if (min.z > m_positions.z)
			min.z = m_positions.z;

		if (max.x < m_positions.x)
			max.x = m_positions.x;
		if (max.y < m_positions.y)
			max.y = m_positions.y;
		if (max.z < m_positions.z)
			max.z = m_positions.z;
	
	}
	MVector pivPos = mTran.rotatePivot(MSpace::kTransform, NULL);
	//MVector piver = mTran.rotatePivotTranslation(MSpace::kWorld, NULL);
	obbHead.position = pivPos;
	center.x = max.x - min.x;
	center.y = max.y - min.y;
	center.z = max.z - min.z;
	//center.normalize();

	obbHead.extension[0] = max.x;
	obbHead.extension[1] = max.y;
	obbHead.extension[2] = max.z;

	obbHead.extensionDir[0].x = 1;
	obbHead.extensionDir[0].y = 0;
	obbHead.extensionDir[0].z = 0;

	obbHead.extensionDir[1].x = 0;
	obbHead.extensionDir[1].y = 1;
	obbHead.extensionDir[1].z = 0;

	obbHead.extensionDir[2].x = 0;
	obbHead.extensionDir[2].y = 0;
	obbHead.extensionDir[2].z = 1;

	/*obbHead.extensionDir.x = center.x;
	obbHead.extensionDir.y = center.y;
	obbHead.extensionDir.z = center.y;
	obbHead.extension =	center.length();*/
	//obbHead.position = m_positions;
	//obbHead.position = center;
	/*obbHead.position.y = m_positions.y;
	obbHead.position.z = m_positions.z;*/

}