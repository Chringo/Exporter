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
	MIntArray indexList;

	const float* positions = mMesh.getRawPoints(NULL);

	Vector3 m_positions = { 0,0,0 };
	MVector center;
	Vector3 min = { 0,0,0 };
	Vector3 max = { 0,0,0 };

	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		m_positions.x = positions[indexList[i] * 3];
		m_positions.y = positions[indexList[i] * 3 + 1];
		m_positions.z = positions[indexList[i] * 3 + 2];

		if (min.x <= m_positions.x)
			min.x = m_positions.x;
		if (min.x <= m_positions.x)
			min.x = m_positions.x;
		if (min.x <= m_positions.x)
			min.x = m_positions.x;
		if (max.x <= m_positions.x)
			max.x = m_positions.x;
		if (max.y <= m_positions.x)
			max.y = m_positions.x;
		if (max.z <= m_positions.x)
			max.z = m_positions.x;
	}

	center.x = max.x - min.x;
	center.y = max.y - min.y;
	center.z = max.z - min.z;

	center.normalize();
	obbHead.extensionDir = center;
	obbHead.extension = center.length();
	//obbHead.position = m_positions;
	obbHead.position.x = m_positions.x;
	obbHead.position.y = m_positions.y;
	obbHead.position.z = m_positions.z;

}