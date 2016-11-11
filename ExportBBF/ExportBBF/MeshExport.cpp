#include "MeshExport.h"



MeshExport::MeshExport()
{
}


MeshExport::MeshExport(std::fstream * outFile)
{
	this->outFile = outFile;
}

MeshExport::~MeshExport()
{
	vertices->clear();
	newIndex->clear();
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

	/*Declaring variables to be used*/
	MIntArray indexList, offsetIdList, normalCount, uvCount, uvIds, normalIdList;
	MFloatPointArray points;
	MFloatArray u, v;
	MFloatVectorArray tangents;

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
	vertices = new vector<Vertex>[indexList.length()];
	newIndex = new vector<unsigned int>[indexList.length()];
	Vertex tempVertex;
	/*for (unsigned int i = 0; i < indexList.length(); ++i)
	{
	MString info;
	info += indexList[i];
	MGlobal::displayInfo(info);
	}*/

	//Recalculating the vertices using only the unique vertices based on individual normals
	//unsigned int index = 0;
	for (unsigned int i = 0; i < indexList.length(); ++i)
	{
		tempVertex.position.x = postitions[indexList[i] * 3];
		tempVertex.position.y = postitions[indexList[i] * 3 + 1];
		tempVertex.position.z = postitions[indexList[i] * 3 + 2];

		tempVertex.normal.x = normalsPos[normalIdList[offsetIdList[i]] * 3];
		tempVertex.normal.y = normalsPos[normalIdList[offsetIdList[i]] * 3 + 1];
		tempVertex.normal.z = normalsPos[normalIdList[offsetIdList[i]] * 3 + 2];

		tempVertex.UV.u = u[uvIds[offsetIdList[i]]];
		tempVertex.UV.v = v[uvIds[offsetIdList[i]]];

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
	}

	for (int i = 1; i < newIndex->size(); i += 3)
	{
		unsigned int temp = newIndex->at(i);
		newIndex->at(i) = newIndex->at(i + 1);
		newIndex->at(i + 1) = temp;
	}

	vertices->shrink_to_fit();
	newIndex->shrink_to_fit();

	/*creating the mesh header and setting the length of the vertices and indices*/
	MeshHeader hHead;
	hHead.indexLength = newIndex->size();
	hHead.vertices = vertices->size();

	/*Getting the transformation matrix*/
	MFnDependencyNode depNode = mMesh.parent(0);
	MFnMatrixData parentMatrix = depNode.findPlug("pm").elementByLogicalIndex(0).asMObject();
	hHead.transMatrix = mTran.transformationMatrix()*parentMatrix.matrix();

	/*writing the information to the binary file*/
	outFile->write((char*)&hHead, sizeof(MeshHeader));
	outFile->write((char*)vertices->data(), sizeof(Vertex)*vertices->size());
	outFile->write((char*)newIndex->data(), sizeof(unsigned int)*newIndex->size());

}
