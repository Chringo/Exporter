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
	MVector value;
	//MMatrix m_center;
	Vector3 min = { 0,0,0 };
	Vector3 max = { 0,0,0 };

	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		m_positions.x = positions[indexList[i] * 3];
		m_positions.y = positions[indexList[i] * 3 + 1];
		m_positions.z = positions[indexList[i] * 3 + 2];

		if (min.x <= m_positions.x)
			min.x = m_positions.x;
		if (min.y <= m_positions.y)
			min.y = m_positions.y;
		if (min.z <= m_positions.z)
			min.z = m_positions.z;
		if (max.x <= m_positions.x)
			max.x = m_positions.x;
		if (max.y <= m_positions.x)
			max.y = m_positions.x;
		if (max.z <= m_positions.x)
			max.z = m_positions.x;
	
	}

	//Center Mitt punkten utav Boundingboxen. 
	value.x = max.x - min.x;
	value.y = max.y - min.y;
	value.z = max.z - min.z;


	value.normalize();
	//ExtensionDir = matrix
	
	obbHead.extensionDir[0].x   = value.x;
	obbHead.extensionDir[0].y   = value.y;
	obbHead.extensionDir[0].z   = value.y;
	obbHead.extensionDir[1].x   = value.x;
	obbHead.extensionDir[1].y ;
	obbHead.extensionDir[1].z;
	obbHead.extensionDir[2].x;
	obbHead.extensionDir[2].y;
	obbHead.extensionDir[2].z;

	//Extension = Längd på extensionDir
	obbHead.extension[0]	   = value.x;
	obbHead.extension[1]	   = value.y;
	obbHead.extension[2]	   = value.z;
	
	//Position = Punktens position. 
	obbHead.position.x		   =	m_positions.x - value.x;
	obbHead.position.y		   =	m_positions.y - value.y;
	obbHead.position.z		   =	m_positions.z - value.z;

}