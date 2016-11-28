#include "MeshExport.h"



MeshExport::MeshExport()
{
}


MeshExport::MeshExport(std::fstream * outFile)
{
	this->outFile = outFile;
	this->jointCount = 0;
}

MeshExport::MeshExport(fstream * outFile, vector<SkinData>* skinList, unsigned int jointCount)
{
	this->outFile = outFile;
	this->skinList = skinList;

	if (jointCount != NULL)
		this->jointCount = jointCount;
}

MeshExport::~MeshExport()
{
	//vertices->clear();
	//newIndex->clear();
	//delete vertices;
	//delete newIndex;
}

void MeshExport::exportMesh(MObject & mNode)
{
	/*extracting the nodes from the MObject*/
	MFnMesh mMesh(MFnTransform(mNode).child(0), NULL);
	MFnTransform mTran = mNode;

	/*forcing the quadsplit to be right handed*/
	MString quadSplit = "setAttr """;
	quadSplit += mMesh.name();
	quadSplit += ".quadSplit"" 0;";
	MGlobal::executeCommandStringResult(quadSplit);

	/*checking if the mesh has a skeleton*/
	if (skinList != nullptr)
	{
		MStatus res;
		MFnDependencyNode skinDepNode = mMesh.object();
		MPlug skinCluster = skinDepNode.findPlug("inMesh", &res);
		MPlugArray skinClusterConnection;
		skinCluster.connectedTo(skinClusterConnection, true, false, &res);
		MFnSkinCluster skinClusterObject(skinClusterConnection[0].node(), &res);

		/*Checking to see if the mesh has a skeleton*/
		if (res)
		{
			exportDynamic(mMesh, mTran);
		}
	}
	else
	{
		exportStatic(mMesh, mTran);
	}
}

int MeshExport::getProgressBarValue()
{
	MIntArray offset, indexList;
	MItDag meshIt(MItDag::kBreadthFirst, MFn::kMesh, NULL);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnMesh(meshIt.currentItem()).getTriangles(offset, indexList);
	}
	return indexList.length();
}

void MeshExport::exportDynamic(MFnMesh & mMesh, MFnTransform & mTran)
{
	/*Declaring variables to be used*/
	MIntArray indexList, offsetIdList, normalCount, uvCount, uvIds, normalIdList;
	MFloatPointArray points;
	MFloatArray u, v;
	MFloatVectorArray tangents;
	QWidget *control = MQtUtil::findControl("progressBar");
	QProgressBar *pBar = (QProgressBar*)control;

	/*getting the index list of the vertex positions*/
	mMesh.getTriangles(offsetIdList, indexList);
	mMesh.getTriangleOffsets(uvCount, offsetIdList);
	mMesh.getAssignedUVs(uvCount, uvIds);
	mMesh.getNormalIds(normalCount, normalIdList);
	mMesh.getTangents(tangents, MSpace::kObject);

	/*getting the data for the vertices*/
	const float * postitions = mMesh.getRawPoints(NULL);
	const float * normalsPos = mMesh.getRawNormals(NULL);
	mMesh.getUVs(u, v);

	/*extracting all the information from maya and putting them in a array of vertices*/
	vector<unsigned int> * newIndex = new vector<unsigned int>[indexList.length()];
	vector<SkelVertex> * sVertices = new vector<SkelVertex>[indexList.length()];
	SkelVertex tempVertex;
	MeshHeader hHead;

	hHead.hasSkeleton = true;

	//Recalculating the vertices using only the unique vertices based on individual normals
	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		tempVertex.position.x = postitions[indexList[i] * 3];
		tempVertex.position.y = postitions[indexList[i] * 3 + 1];
		tempVertex.position.z = postitions[indexList[i] * 3 + 2];

		tempVertex.normal.x = normalsPos[normalIdList[offsetIdList[i]] * 3];
		tempVertex.normal.y = normalsPos[normalIdList[offsetIdList[i]] * 3 + 1];
		tempVertex.normal.z = normalsPos[normalIdList[offsetIdList[i]] * 3 + 2];

		tempVertex.tangent.x = tangents[normalIdList[offsetIdList[i]]].x;
		tempVertex.tangent.y = tangents[normalIdList[offsetIdList[i]]].y;
		tempVertex.tangent.z = tangents[normalIdList[offsetIdList[i]]].z;

		tempVertex.UV.u = u[uvIds[offsetIdList[i]]];
		tempVertex.UV.v = v[uvIds[offsetIdList[i]]];

		/*for loop that controls the weight. If the weight is to low,
		the influence will be set to -1. Which means that the weight will
		not be calculated in the engine.*/
		for (int k = 0; k < 4; ++k)
		{
			tempVertex.weights[k] = skinList->at(indexList[i]).weights[k];
			if (tempVertex.weights[k] < FLT_EPSILON)
			{
				tempVertex.influence[k] = -1;
			}
			else
			{
				tempVertex.influence[k] = skinList->at(indexList[i]).boneInfluences[k];
			}
		}

		bool exists = false;

		for (int j = 0; j < sVertices->size(); ++j)
		{
			if (memcmp(&tempVertex, &sVertices->at(j), sizeof(Vertex)) == 0)
			{
				exists = true;
				newIndex->push_back(j);
				break;
			}
		}
		if (!exists)
		{
			newIndex->push_back((unsigned int)sVertices->size());
			sVertices->push_back(tempVertex);
		}
		pBar->setValue(pBar->value() + 1);
	}
	//sVertices->shrink_to_fit(); //kanske sedundant
	//newIndex->shrink_to_fit();

	/*creating the mesh header and setting the length of the vertices and indices*/

	hHead.indexLength = (unsigned int)newIndex->size();
	hHead.vertices = (unsigned int)sVertices->size();
	hHead.jointCount = jointCount;

	/*Getting the transformation matrix*/
	//MFnDependencyNode depNode = mMesh.parent(0);
	//MFnMatrixData parentMatrix = depNode.findPlug("pm").elementByLogicalIndex(0).asMObject();
	//hHead.transMatrix = mTran.transformationMatrix()*parentMatrix.matrix();

	/*writing the information to the binary file*/
	outFile->write((char*)&hHead, sizeof(MeshHeader));
	outFile->write((char*)sVertices->data(), sizeof(SkelVertex)*sVertices->size());
	outFile->write((char*)newIndex->data(), sizeof(unsigned int)*newIndex->size());

	/*clearing the variables*/
	sVertices->clear();
	newIndex->clear();
}

void MeshExport::exportStatic(MFnMesh & mMesh, MFnTransform & mTran)
{
	/*Declaring variables to be used*/
	MIntArray indexList, offsetIdList, normalCount, uvCount, uvIds, normalIdList;
	MFloatPointArray points;
	MFloatArray u, v;
	MFloatVectorArray tangents;
	MeshHeader hHead;
	QWidget *control = MQtUtil::findControl("progressBar");
	QProgressBar *pBar = (QProgressBar*)control;

	/*getting the index list of the vertex positions*/
	mMesh.getTriangles(offsetIdList, indexList);
	mMesh.getTriangleOffsets(uvCount, offsetIdList);
	mMesh.getAssignedUVs(uvCount, uvIds);
	mMesh.getNormalIds(normalCount, normalIdList);
	mMesh.getTangents(tangents, MSpace::kObject);

	/*getting the data for the vertices*/
	const float * postitions = mMesh.getRawPoints(NULL);
	const float * normalsPos = mMesh.getRawNormals(NULL);
	mMesh.getUVs(u, v);

	/*extracting all the information from maya and putting them in a array of vertices*/
	newIndex = new vector<unsigned int>[indexList.length()];
	vertices = new vector<Vertex>[indexList.length()];
	hHead.hasSkeleton = false;

	Vertex tempVertex;

	//Recalculating the vertices using only the unique vertices based on individual normals
	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		tempVertex.position.x = postitions[indexList[i] * 3];
		tempVertex.position.y = postitions[indexList[i] * 3 + 1];
		tempVertex.position.z = postitions[indexList[i] * 3 + 2];

		tempVertex.normal.x = normalsPos[normalIdList[offsetIdList[i]] * 3];
		tempVertex.normal.y = normalsPos[normalIdList[offsetIdList[i]] * 3 + 1];
		tempVertex.normal.z = normalsPos[normalIdList[offsetIdList[i]] * 3 + 2];

		tempVertex.tangent.x = tangents[normalIdList[offsetIdList[i]]].x;
		tempVertex.tangent.y = tangents[normalIdList[offsetIdList[i]]].y;
		tempVertex.tangent.z = tangents[normalIdList[offsetIdList[i]]].z;

		tempVertex.UV.u = u[uvIds[offsetIdList[i]]];
		tempVertex.UV.v = 1.0 - v[uvIds[offsetIdList[i]]];

		bool exists = false;

		for (int j = 0; j < vertices->size(); ++j)
		{
			if (memcmp(&tempVertex, &vertices->at(j), sizeof(Vertex)) == 0)
			{
				exists = true;
				newIndex->push_back(j);
				break;
			}
		}
		if (!exists)
		{
			newIndex->push_back((unsigned int)vertices->size());
			vertices->push_back(tempVertex);
		}
		pBar->setValue(pBar->value() + 1);
	}

	//vertices->shrink_to_fit();
	//newIndex->shrink_to_fit();

	/*creating the mesh header and setting the length of the vertices and indices*/
	hHead.indexLength = (unsigned int)newIndex->size();
	hHead.vertices = (unsigned int)vertices->size();
	hHead.jointCount = 0;

	/*Getting the transformation matrix*/
	//MFnDependencyNode depNode = mMesh.parent(0);
	//MFnMatrixData parentMatrix = depNode.findPlug("pm").elementByLogicalIndex(0).asMObject();
	//hHead.transMatrix = mTran.transformationMatrix()*parentMatrix.matrix();

	/*writing the information to the binary file*/
	outFile->write((char*)&hHead, sizeof(MeshHeader));
	outFile->write((char*)vertices->data(), sizeof(Vertex)*hHead.vertices);
	outFile->write((char*)newIndex->data(), sizeof(unsigned int)*hHead.indexLength);

	/*deleting allocated variables*/
	vertices->clear();
	newIndex->clear();
}
