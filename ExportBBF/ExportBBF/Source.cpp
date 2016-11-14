#include "maya_includes.h"
#include "SkelAnimExport.h"
#include "HeaderStructs.h"
#include <maya/MFnPlugin.h>


using namespace std;

MCallbackIdArray myCallbackArray;
fstream outFile("knulla.BBF", std::fstream::out | std::fstream::binary);

void Createmesh(MObject & mNode, SkelAnimExport & cSkelAnim)
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
	vector<Vertex> * vertices = nullptr;
	vector<SkelVertex> *sVertices = nullptr;
	vector<unsigned int> * newIndex = new vector<unsigned int>[indexList.length()];

	/*checking if the mesh has a skeleton*/
	MStatus res;
	MFnDependencyNode skinDepNode = mMesh.object();
	MPlug skinCluster = skinDepNode.findPlug("inMesh", &res);
	MPlugArray skinClusterConnection;
	skinCluster.connectedTo(skinClusterConnection, true, false, &res);
	MFnSkinCluster skinClusterObject(skinClusterConnection[0].node(), &res);

	MeshHeader hHead;
	
	if (res)
	{
		sVertices = new vector<SkelVertex>[indexList.length()];
		SkelVertex tempVertex;
		hHead.hasSkeleton = true;

		vector<SkinData> * skinList = &cSkelAnim.skinList;

		//Recalculating the vertices using only the unique vertices based on individual normals
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
		}
	}
	else
	{
		vertices = new vector<Vertex>[indexList.length()];
		Vertex tempVertex;
		hHead.hasSkeleton = false;

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
		}
	}

	/*remaking the index to fit a righthanded system. Maya is lefthanded.*/
	/*for (int i = 1; i < newIndex->size(); i+=3)
	{
		unsigned int temp = newIndex->at(i);
		newIndex->at(i) = newIndex->at(i + 1);
		newIndex->at(i + 1) = temp;
	}*/

	if(hHead.hasSkeleton)
		sVertices->shrink_to_fit();
	else
		vertices->shrink_to_fit();
	newIndex->shrink_to_fit();

	/*creating the mesh header and setting the length of the vertices and indices*/
	
	hHead.indexLength = (unsigned int)newIndex->size();
	if (hHead.hasSkeleton)
		hHead.vertices = (unsigned int)sVertices->size();
	else
		hHead.vertices = (unsigned int)vertices->size();

	/*Getting the transformation matrix*/
	MFnDependencyNode depNode = mMesh.parent(0);
	MFnMatrixData parentMatrix = depNode.findPlug("pm").elementByLogicalIndex(0).asMObject();
	hHead.transMatrix = mTran.transformationMatrix()*parentMatrix.matrix();

	/*writing the information to the binary file*/
	outFile.write((char*)&hHead, sizeof(MeshHeader));
	outFile.write((char*)vertices->data(), sizeof(Vertex)*vertices->size());
	outFile.write((char*)newIndex->data(), sizeof(unsigned int)*newIndex->size());

	/*deleting allocated variables*/
	if (hHead.hasSkeleton)
		sVertices->clear();
	else
		vertices->clear();
	newIndex->clear();
}
void skeletonHandler(MObject & mNode)
{
	MFnSkinCluster mSkel = mNode;
	MGlobal::displayInfo(mSkel.name());
	
}

EXPORT MStatus initializePlugin(MObject obj)
{
    SkelAnimExport cSkelAnim;

	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	//ofstream outFile("test", std::ofstream::binary);
	if (!outFile.is_open())
	{
		MGlobal::displayError("ERROR: the binary file is not open");
		
	}

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

	MGlobal::displayInfo("Maya plugin loaded!");

    /*Iterate all skin clusters in scene.*/
    cSkelAnim.IterateSkinClusters();

    /*Iterate all joints in scene.*/
    cSkelAnim.IterateJoints();
	
	/*writing a temporary mainheader for one mesh*/
	MainHeader tempHead{ 1 };
	outFile.write((char*)&tempHead, sizeof(MainHeader));

	MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnTransform trans = meshIt.currentItem();
		if (trans.child(0).hasFn(MFn::kMesh))
		{
			Createmesh(meshIt.currentItem(), cSkelAnim);
		}
	}

	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	outFile.close();
	// if any resources have been allocated, release and free here before
	// returning...
	MMessage::removeCallbacks(myCallbackArray);


	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}