#include "maya_includes.h"
#include "HeaderStructs.h"
#include "MaterialExport.h"


using namespace std;

MCallbackIdArray myCallbackArray;
fstream outFile("knulla.BBF", std::fstream::out | std::fstream::binary);

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
	vector<Vertex> * vertices = new vector<Vertex>[indexList.length()];
	vector<unsigned int> * newIndex = new vector<unsigned int>[indexList.length()];
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
	
	for (int i = 1; i < newIndex->size(); i+=3)
	{
		unsigned int temp = newIndex->at(i);
		newIndex->at(i) = newIndex->at(i + 1);
		newIndex->at(i + 1) = temp;
	}

	vertices->shrink_to_fit();
	newIndex->shrink_to_fit();

	/*creating the mesh header and setting the length of the vertices and indices*/
	MeshHeader mHead;
	mHead.indexLength = newIndex->size();
	mHead.vertices = vertices->size();

	/*Getting the transformation matrix*/
	MFnDependencyNode depNode = mMesh.parent(0);
	MFnMatrixData parentMatrix = depNode.findPlug("pm").elementByLogicalIndex(0).asMObject();
	mHead.transMatrix = mTran.transformationMatrix()*parentMatrix.matrix();

	/*writing the information to the binary file*/
	outFile.write((char*)&mHead, sizeof(MeshHeader));
	outFile.write((char*)vertices->data(), sizeof(Vertex)*vertices->size());
	outFile.write((char*)newIndex->data(), sizeof(unsigned int)*newIndex->size());

	/*deleting allocated variables*/
	vertices->clear();
	newIndex->clear();
}
void skeletonHandler(MObject & mNode)
{
	MFnSkinCluster mSkel = mNode;
	MGlobal::displayInfo(mSkel.name());
	
}

//void extractingMaterials()
//{
//	MStatus stat;
//	MItDag dagIter(MItDag::kBreadthFirst, MFn::kInvalid, &stat);
//
//	for (; !dagIter.isDone(); dagIter.next())
//	{
//		MDagPath dagPath;
//		stat = dagIter.getPath(dagPath);
//
//		if (stat)
//		{
//			MFnDagNode dagNode(dagPath, &stat);
//
//			if (dagNode.isIntermediateObject())continue;
//			if (!dagPath.hasFn(MFn::kMesh))continue;
//			if (dagPath.hasFn(MFn::kTransform))continue;
//
//			MFnMesh fnMesh(dagPath);
//
//			unsigned instanceNumber = dagPath.instanceNumber();
//			MObjectArray sets;
//			MObjectArray comps;
//
//			fnMesh.getConnectedSetsAndMembers(instanceNumber, sets, comps, true);
//
//			for (unsigned i = 0; i < sets.length(); i++)
//			{
//				MObject set = sets[i];
//				MObject comp = comps[i];
//				
//				MFnSet fnSet(set);
//
//				MFnDependencyNode dnSet(set);
//				MObject ssattr = dnSet.attribute(MString("surfaceShader"));
//
//				MPlug sPlug(set, ssattr);
//
//				MPlugArray srcplugarray;
//
//				sPlug.connectedTo(srcplugarray, true, false);
//
//				if (srcplugarray.length() == 0) continue;
//
//				MObject srcNode = srcplugarray[0].node();
//
//				// Values for metal and roughness.
//
//				MaterialHeader mHeader;
//
//				
//#pragma region Attributes
//				MPlug metalness = MFnDependencyNode(srcNode).findPlug("metallic", &stat);
//				float value;
//				metalness.getValue(value);
//				mHeader.metallness = value;
//				cerr << "Metallic Value: " << value << endl;
//
//				MPlug roughness = MFnDependencyNode(srcNode).findPlug("roughness", &stat);
//				float roughValue;
//				roughness.getValue(roughValue);
//				mHeader.roughness = roughValue;
//				cerr << "RoughNess Value: " << roughValue << endl;
//
//				MPlug Emissive = MFnDependencyNode(srcNode).findPlug("emissive", &stat);
//				float emissiveValue; 
//				Emissive.getValue(emissiveValue);
//				mHeader.emissive = emissiveValue;
//				cerr << "Emissive Value: " << emissiveValue << endl;;
//#pragma endregion
//
//#pragma region texture
//				
//				#pragma region Texture
//				MPlug texture = MFnDependencyNode(srcNode).findPlug("TEX_color_map", &stat);
//				MItDependencyGraph dgItt(texture, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
//				dgItt.disablePruningOnFilter();
//
//				if (dgItt.isDone())
//				{
//					continue;
//				}
//				MObject textureNode = dgItt.thisNode();
//				MPlug filenamePlugn = MFnDependencyNode(textureNode).findPlug("fileTextureName");
//				MString texName;
//
//				filenamePlugn.getValue(texName);
//				cerr << "texture MAP!!" << endl;
//				cerr << "Set: " << fnSet.name() << endl;
//				cerr << "Texture Node Name: " << MFnDependencyNode(textureNode).name() << endl;
//				mHeader.textureMap = texName.asChar();
//				cerr << "Texture File Name: " << texName.asChar() << endl;
//#pragma endregion
//				#pragma region Normal
//				MPlug texNormal = MFnDependencyNode(srcNode).findPlug("TEX_normal_map", &stat);
//				MItDependencyGraph dgItn(texNormal, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
//				dgItn.disablePruningOnFilter();
//
//				if (dgItn.isDone())
//				{
//					continue;
//				}
//				MObject normalNode = dgItn.thisNode();
//				MPlug filenameTexPlugn = MFnDependencyNode(normalNode).findPlug("fileTextureName");
//				MString textureName;
//				
//				filenameTexPlugn.getValue(textureName);
//				cerr << "normal MAP!!" << endl;
//				cerr << "Set: " << fnSet.name() << endl;
//				cerr << "Texture Node Name: " << MFnDependencyNode(normalNode).name() << endl;
//				mHeader.normalMap = textureName.asChar();
//				cerr << "Texture File Name: " << textureName.asChar() << endl;
//				#pragma endregion
//				#pragma region Metallic
//				MPlug texmetall = MFnDependencyNode(srcNode).findPlug("TEX_metallic_map", &stat);
//				MItDependencyGraph dgIt(texmetall, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
//				dgIt.disablePruningOnFilter();
//
//				if (dgIt.isDone())
//				{
//					continue;
//				}
//				MObject metallNode = dgIt.thisNode();
//				MPlug filenamePlugm = MFnDependencyNode(metallNode).findPlug("fileTextureName");
//				MString textureNamem;
//
//				filenamePlugm.getValue(textureNamem);
//				cerr << "MetalMap!!" << endl;
//				cerr << "Set: " << fnSet.name() << endl;
//				cerr << "Texture Node Name: " << MFnDependencyNode(metallNode).name() << endl;
//				mHeader.metallicMap = textureNamem.asChar();
//				cerr << "Texture File Name: " << textureNamem.asChar() << endl;
//				#pragma endregion
//				#pragma region Roughness
//				MPlug texRogugh = MFnDependencyNode(srcNode).findPlug("TEX_roughness_map", &stat);
//				MItDependencyGraph dgItr(texRogugh, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
//				dgItr.disablePruningOnFilter();
//
//				if (dgItr.isDone())
//				{
//					continue;
//				}
//				MObject roughNode = dgItr.thisNode();
//				MPlug filenamePlugr = MFnDependencyNode(roughNode).findPlug("fileTextureName");
//				MString textureNamer;
//
//				filenamePlugr.getValue(textureNamer);
//				cerr << "Roughness MAP!!" << endl;
//				cerr << "Set: " << fnSet.name() << endl;
//				cerr << "Texture Node Name: " << MFnDependencyNode(roughNode).name() << endl;
//				mHeader.roughnessMap = textureNamer.asChar();
//				cerr << "Texture File Name: " << textureNamer.asChar() << endl;
//#pragma endregion 
//				#pragma region AO
//				MPlug texAo = MFnDependencyNode(srcNode).findPlug("TEX_ao_map", &stat);
//				MItDependencyGraph dgIta(texAo, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
//				dgIta.disablePruningOnFilter();
//
//				if (dgIta.isDone())
//				{
//					continue;
//				}
//				MObject aoNode = dgIta.thisNode();
//				MPlug filenamePluga = MFnDependencyNode(aoNode).findPlug("fileTextureName");
//				MString textureNamea;
//
//				filenamePluga.getValue(textureNamea);
//				cerr << "AO MAP!!" << endl;
//				cerr << "Set: " << fnSet.name() << endl;
//				cerr << "Texture Node Name: " << MFnDependencyNode(aoNode).name() << endl;
//				mHeader.aoMap = textureNamea.asChar();
//				cerr << "Texture File Name: " << textureNamea.asChar() << endl;
//#pragma endregion 
//				outFile.write((char*)&mHeader, sizeof(MaterialHeader));
//#pragma endregion
//			}
//		}
//	}
//
//
//}

void extractingLights()
{
	//Extract the lights
	MFnPointLight pointLight;
	MFnLight dirLight,spotLight;

	
	//Point 
	//Dir
	//Spot 
	//find attributes for each light soruce 
	//Save to Structs 
	//Write to file. 
}
EXPORT MStatus initializePlugin(MObject obj)
{
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
	
	/*writing a temporary mainheader for one mesh*/
	MainHeader tempHead{ 1 };
	outFile.write((char*)&tempHead, sizeof(MainHeader));
	
	MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnTransform trans = meshIt.currentItem();
		if (trans.child(0).hasFn(MFn::kMesh))
		{
			Createmesh(meshIt.currentItem());
			
		}
		
	}
	
	MaterialExport bajs;
	bajs.MaterialExtraction();

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