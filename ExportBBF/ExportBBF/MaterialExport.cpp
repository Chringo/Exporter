#include "MaterialExport.h"

MaterialExport::MaterialExport()
{
}

MaterialExport::MaterialExport(fstream * outFile)
{
	this->outFile = outFile;
}


MaterialExport::~MaterialExport()
{
}

void MaterialExport::GetNumMats()
{
	MStatus stat;
	MItDag dagIter(MItDag::kBreadthFirst, MFn::kInvalid, &stat);
	int amountOfMats = 0;
	for (; !dagIter.isDone(); dagIter.next())
	{
		MDagPath dagPath;
		stat = dagIter.getPath(dagPath);

		if (stat)
		{
			MFnDagNode dagNode(dagPath, &stat);

			if (dagNode.isIntermediateObject())continue;
			if (!dagPath.hasFn(MFn::kMesh))continue;
			if (dagPath.hasFn(MFn::kTransform))continue;

			MFnMesh fnMesh(dagPath);

			unsigned instanceNumber = dagPath.instanceNumber();
			MObjectArray sets;
			MObjectArray comps;

			amountOfMats += 1;

			
			fnMesh.getConnectedSetsAndMembers(instanceNumber, sets, comps, true);

			for (unsigned i = 0; i < sets.length(); i++)
			{
				

				MObject set = sets[i];
				MObject comp = comps[i];

				MFnSet fnSet(set);

				MFnDependencyNode dnSet(set);
				MObject ssattr = dnSet.attribute(MString("surfaceShader"));

				MPlug sPlug(set, ssattr);

				MPlugArray srcplugarray;

				sPlug.connectedTo(srcplugarray, true, false);

				if (srcplugarray.length() == 0) continue;

				MObject srcNode = srcplugarray[0].node();
			
			}
		}
	}
	cerr << "BAJSERS: " << amountOfMats << endl;

	MaterialExtraction();
}
void MaterialExport::MaterialExtraction()
{
	MStatus stat;
	MItDag dagIter(MItDag::kBreadthFirst, MFn::kInvalid, &stat);
	for (; !dagIter.isDone(); dagIter.next())
	{
		MDagPath dagPath;
		stat = dagIter.getPath(dagPath);

		if (stat)
		{
			MFnDagNode dagNode(dagPath, &stat);

			if (dagNode.isIntermediateObject())continue;
			if (!dagPath.hasFn(MFn::kMesh))continue;
			if (dagPath.hasFn(MFn::kTransform))continue;

			MFnMesh fnMesh(dagPath);

			unsigned instanceNumber = dagPath.instanceNumber();
			MObjectArray sets;
			MObjectArray comps;
			fnMesh.getConnectedSetsAndMembers(instanceNumber, sets, comps, true);

			for (unsigned i = 0; i < sets.length(); i++)
			{


				MObject set = sets[i];
				MObject comp = comps[i];

				MFnSet fnSet(set);

				MFnDependencyNode dnSet(set);
				MObject ssattr = dnSet.attribute(MString("surfaceShader"));

				MPlug sPlug(set, ssattr);

				MPlugArray srcplugarray;

				sPlug.connectedTo(srcplugarray, true, false);

				if (srcplugarray.length() == 0) continue;

				MObject srcNode = srcplugarray[0].node();

				MaterialHeader mHeader;
				cerr << "Set: " << fnSet.name() << endl; // Shader Group
				memcpy(mHeader.shaderName, fnSet.name().asChar(), fnSet.name().length());
				mHeader.shaderName[fnSet.name().length()] = '\0';

				MPlug texture = MFnDependencyNode(srcNode).findPlug("TEX_color_map", &stat);
				MItDependencyGraph dgItt(texture, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItt.disablePruningOnFilter();


				MObject textureNode = dgItt.thisNode();
				MPlug filenamePlugn = MFnDependencyNode(textureNode).findPlug("fileTextureName");
				MString texName;

				filenamePlugn.getValue(texName);
				cerr << "texture MAP!!" << endl;
				//cerr << "Set: " << fnSet.name() << endl;
				cerr << "Texture Node Name: " << MFnDependencyNode(textureNode).name() << endl;
				memcpy(mHeader.textureMap, texName.asChar(), texName.length());
				mHeader.metallicMap[texName.length()] = '\0';
				cerr << "Texture File Name: " << texName.asChar() << endl;

#pragma region Normal
				MPlug texNormal = MFnDependencyNode(srcNode).findPlug("TEX_normal_map", &stat);
				MItDependencyGraph dgItn(texNormal, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItn.disablePruningOnFilter();

				MObject normalNode = dgItn.thisNode();
				MPlug filenameTexPlugn = MFnDependencyNode(normalNode).findPlug("fileTextureName");
				MString textureName;

				filenameTexPlugn.getValue(textureName);
				cerr << "normal MAP!!" << endl;
				//cerr << "Set: " << fnSet.name() << endl;
				cerr << "Texture Node Name: " << MFnDependencyNode(normalNode).name() << endl;
				memcpy(mHeader.normalMap, textureName.asChar(), textureName.length());
				mHeader.normalMap[textureName.length()] = '\0';
				cerr << "Texture File Name: " << textureName.asChar() << endl;
#pragma endregion

#pragma region Metallic
				MPlug texmetall = MFnDependencyNode(srcNode).findPlug("TEX_metallic_map", &stat);
				MItDependencyGraph dgIt(texmetall, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIt.disablePruningOnFilter();

				MObject metallNode = dgIt.thisNode();
				MPlug filenamePlugm = MFnDependencyNode(metallNode).findPlug("fileTextureName");
				MString textureNamem;

				filenamePlugm.getValue(textureNamem);
				cerr << "MetalMap!!" << endl;
				//cerr << "Set: " << fnSet.name() << endl;
				cerr << "Texture Node Name: " << MFnDependencyNode(metallNode).name() << endl;
				memcpy(mHeader.metallicMap, textureNamem.asChar(), textureNamem.length());
				mHeader.metallicMap[textureNamem.length()] = '\0';
				cerr << "Texture File Name: " << textureNamem.asChar() << endl;
#pragma endregion
#pragma region Roughness
				MPlug texRogugh = MFnDependencyNode(srcNode).findPlug("TEX_roughness_map", &stat);
				MItDependencyGraph dgItr(texRogugh, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItr.disablePruningOnFilter();

				MObject roughNode = dgItr.thisNode();
				MPlug filenamePlugr = MFnDependencyNode(roughNode).findPlug("fileTextureName");
				MString textureNamer;

				filenamePlugr.getValue(textureNamer);
				cerr << "Roughness MAP!!" << endl;
				cerr << "Texture Node Name: " << MFnDependencyNode(roughNode).name() << endl;
				memcpy(mHeader.roughnessMap, textureNamer.asChar(), textureNamer.length());
				mHeader.normalMap[textureName.length()] = '\0';
				cerr << "Texture File Name: " << textureNamer.asChar() << endl;
#pragma endregion 

#pragma region AO
				MPlug texAo = MFnDependencyNode(srcNode).findPlug("TEX_ao_map", &stat);
				MItDependencyGraph dgIta(texAo, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIta.disablePruningOnFilter();

				MObject aoNode = dgIta.thisNode();
				MPlug filenamePluga = MFnDependencyNode(aoNode).findPlug("fileTextureName");
				MString textureNamea;

				filenamePluga.getValue(textureNamea);
				cerr << "AO MAP!!" << endl;
				//cerr << "Set: " << fnSet.name() << endl;
				cerr << "Texture Node Name: " << MFnDependencyNode(aoNode).name() << endl;
				memcpy(mHeader.aoMap, textureNamea.asChar(), textureNamea.length());
				mHeader.aoMap[textureNamea.length()] = '\0';
				cerr << "Texture File Name: " << textureNamea.asChar() << endl;
#pragma endregion 
				//ExtractingTextures(srcNode, stat);
			//	MaterialHeader mHeader;
				MPlug metalness = MFnDependencyNode(srcNode).findPlug("metallic", &stat);
				float value;
				metalness.getValue(value);
				mHeader.metallness = value;

				MPlug roughness = MFnDependencyNode(srcNode).findPlug("roughness", &stat);
				float roughValue;
				roughness.getValue(roughValue);
				mHeader.roughness = roughValue;

				MPlug Emissive = MFnDependencyNode(srcNode).findPlug("emissive", &stat);
				float emissiveValue;
				Emissive.getValue(emissiveValue);
				mHeader.emissive = emissiveValue;

				ExportingMats_Tex();
			}
		}
	}

}

void MaterialExport::ExtractingTextures(MObject &srcNode, MStatus &stat)
{
	
}

void MaterialExport::ExtractingAttributes(MObject & srcNode)
{
	

}

void MaterialExport::ExportingMats_Tex()
{
	MaterialHeader mHead;

	outFile->write((char*)&mHead, sizeof(MaterialHeader));
}


