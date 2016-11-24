#include "MaterialExport.h"

MaterialExport::MaterialExport()
{
}

MaterialExport::MaterialExport(fstream * outFile,string filePath)
{
	this->outFile = outFile;
	this->filePath = filePath;
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
				amountOfMats += 1;
			}
		}
	}
	MainHeader mHead;
	//mHead.numOfMats = amountOfMats; //<-----------------------------------------------------------------------
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
				//TextureHeader tHeader;

				mHeader.shaderNameLength = fnSet.name().length();
				#pragma region textureColor
				MPlug texture = MFnDependencyNode(srcNode).findPlug("TEX_color_map", &stat);
				MItDependencyGraph dgItt(texture, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItt.disablePruningOnFilter();


				MObject textureNode = dgItt.thisNode();
				MPlug filenamePlugn = MFnDependencyNode(textureNode).findPlug("fileTextureName");
				MString texName;

				filenamePlugn.getValue(texName);
			
				mHeader.textureNameLength = texName.length();
				int test = 0;
				string ctex = texName.asChar();
				if (!ctex.empty())
				{
					ExportingTex(ctex);
				}
				cerr << "1: " << test<<endl;

#pragma endregion
				#pragma region Normal
				MPlug texNormal = MFnDependencyNode(srcNode).findPlug("TEX_normal_map", &stat);
				MItDependencyGraph dgItn(texNormal, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItn.disablePruningOnFilter();

				MObject normalNode = dgItn.thisNode();
				MPlug filenameTexPlugn = MFnDependencyNode(normalNode).findPlug("fileTextureName");
				MString textureName;

				filenameTexPlugn.getValue(textureName);
				mHeader.normalNameLength = textureName.length();
			
				string ntex = textureName.asChar();
				if (!ntex.empty())
				{
					ExportingTex(ntex);
				}
				
				cerr << "2: " << test << endl;
				#pragma endregion

				#pragma region Metallic
				MPlug texmetall = MFnDependencyNode(srcNode).findPlug("TEX_metallic_map", &stat);
				MItDependencyGraph dgIt(texmetall, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIt.disablePruningOnFilter();

				MObject metallNode = dgIt.thisNode();
				MPlug filenamePlugm = MFnDependencyNode(metallNode).findPlug("fileTextureName");
				MString textureNamem;

			
				filenamePlugm.getValue(textureNamem);
				mHeader.metallicNameLength = textureNamem.length();
				string mtex = textureNamem.asChar();
				if (!mtex.empty())
				{
					ExportingTex(mtex);
				}
				cerr << "3: " << test << endl;

				
				#pragma endregion
				#pragma region Roughness
				MPlug texRogugh = MFnDependencyNode(srcNode).findPlug("TEX_roughness_map", &stat);
				MItDependencyGraph dgItr(texRogugh, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItr.disablePruningOnFilter();

				MObject roughNode = dgItr.thisNode();
				MPlug filenamePlugr = MFnDependencyNode(roughNode).findPlug("fileTextureName");
				MString textureNamer;

			
			
				filenamePlugr.getValue(textureNamer);
				mHeader.woofNameLength = textureNamer.length();
				string rtex = textureNamer.asChar();
				if (!rtex.empty())
				{
					ExportingTex(rtex);
				}

				cerr << "4: " << test << endl;
				
				#pragma endregion 

				#pragma region AO
				MPlug texAo = MFnDependencyNode(srcNode).findPlug("TEX_ao_map", &stat);
				MItDependencyGraph dgIta(texAo, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIta.disablePruningOnFilter();

				MObject aoNode = dgIta.thisNode();
				MPlug filenamePluga = MFnDependencyNode(aoNode).findPlug("fileTextureName");
				MString textureNamea;

				filenamePluga.getValue(textureNamea);
				mHeader.aoNameLength = textureNamea.length();

				cerr << "5: " << test << endl;
				string atex = textureNamea.asChar();
				if (!atex.empty())
				{
					ExportingTex(atex);
				}

				#pragma endregion 
				#pragma region Attributes
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
				#pragma endregion
			}
		}
	}

	ExportingMats_Tex();
}

void MaterialExport::ExportingMats_Tex()
{
	MaterialHeader mHead;
	outFile->write((char*)&mHead, sizeof(MaterialHeader));
	
}

void MaterialExport::ExportingTex(string file)
{
	int i = 0;
	int j = 0;
	size_t f = file.rfind("/", file.length());
	string pAth = file.substr(f + 1, file.length() - f);

	std::string str = filePath;
	std::string res = str.substr(str.find_last_of("/") + 1);

	str.erase(str.find_last_of("/"));
	str += "/" + pAth;
	wstring newpath_wstr(str.begin(), str.end());
	wstring path_wstr(file.begin(), file.end());

	CopyFile(path_wstr.c_str(), newpath_wstr.c_str(), FALSE);

	//return i;

}


