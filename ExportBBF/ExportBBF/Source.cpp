#include "maya_includes.h"
#include "HeaderStructs.h"
#include <vector>

using namespace std;

MCallbackIdArray myCallbackArray;

void Createmesh(MObject & mNode)
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
	MFloatVectorArray normals;
	
	/*getting the index list of the vertex positions*/
	mMesh.getTriangles(offsetIdList, indexList);
	mMesh.getTriangleOffsets(uvCount, offsetIdList);
	mMesh.getAssignedUVs(uvCount, uvIds);
	mMesh.getNormalIds(normalCount, normalIdList);

	/*getting the data for the vertices*/
	const float * postitions = mMesh.getRawPoints(NULL);
	const float * normalsPos = mMesh.getRawNormals(NULL);
	mMesh.getUVs(u, v);

	/*extracting all the information from maya and putting them in a array of vertices*/
	vector<Vertex> * vertices = new vector<Vertex>[indexList.length()];
	vector<unsigned int> * newIndex = new vector<unsigned int>[indexList.length()];
	Vertex tempVertex;
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
			newIndex->push_back(i);
			vertices->push_back(tempVertex);
		}
	}
	vertices->shrink_to_fit();
	newIndex->shrink_to_fit();

}

EXPORT MStatus initializePlugin(MObject obj)
{
	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

	MGlobal::displayInfo("Maya plugin loaded!");
	
	MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnTransform trans = meshIt.currentItem();
		if (trans.child(0).hasFn(MFn::kMesh))
		{
			Createmesh(meshIt.currentItem());
		}
	}
	
	
	// if res == kSuccess then the plugin has been loaded,
	// otherwise is has not.
	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	// if any resources have been allocated, release and free here before
	// returning...

	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}