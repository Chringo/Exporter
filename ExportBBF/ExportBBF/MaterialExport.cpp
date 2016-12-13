#include "MaterialExport.h"

MaterialExport::MaterialExport()
{
}

bool bbfExist(const std::string& fileName)
{
	struct stat buf;
	if (stat(fileName.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}
MaterialExport::MaterialExport(string &filePath)
{
	//this->outFile = outFile;
	//size_t f = filePath.rfind(".", filePath.length());
	//this->filePath = filePath.substr(0, f - 1);
	//this->filePath += ".mat";
	this->filePath = filePath;
}



MaterialExport::~MaterialExport()
{
	delete outFile;
}

void MaterialExport::MaterialExtraction()
{
	QWidget *control = MQtUtil::findControl("progressBar");
	QProgressBar *pBar = (QProgressBar*)control;

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

				/*setting the filename to the material name*/
				this->filePath += string(MFnDependencyNode(srcNode).name().asChar()) + ".mat";

				//mHeader.textureIDs[0] = fnSet.name().length();
#pragma region textureColor
				MPlug texture = MFnDependencyNode(srcNode).findPlug("TEX_color_map", &stat);
				MItDependencyGraph dgItt(texture, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItt.disablePruningOnFilter();

				memcpy(tHeader.shaderName, fnSet.name().asChar(), fnSet.name().length());
				tHeader.shaderName[fnSet.name().length()] = '\0';


				MObject textureNode = dgItt.thisNode();
				MPlug filenamePlugn = MFnDependencyNode(textureNode).findPlug("fileTextureName");
				MString texName;

				filenamePlugn.getValue(texName);

			//	mHeader.textureNameLength = texName.length();
				
				int test = 0;
				string ctex = texName.asChar();
				if (!ctex.empty())
				{
					string path;
					path = ExportingTex(ctex);
					memcpy(tHeader.textureName, path.c_str(), path.length());
					//tHeader.textureName = path;
					tHeader.textureName[path.length()] = '\0';
					//mHeader.textureIDs[0] = path.length();
					mHeader.textureIDs[0] = path.length()+1;
				}
				//cerr << "1: " << test<<endl
				//mHeader.textureIDs[0] = texName.length();
				pBar->setValue(pBar->value() + 1);

#pragma endregion
#pragma region Normal
				MPlug texNormal = MFnDependencyNode(srcNode).findPlug("TEX_normal_map", &stat);
				MItDependencyGraph dgItn(texNormal, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItn.disablePruningOnFilter();

				MObject normalNode = dgItn.thisNode();
				MPlug filenameTexPlugn = MFnDependencyNode(normalNode).findPlug("fileTextureName");
				MString textureName;

				filenameTexPlugn.getValue(textureName);
	//			mHeader.normalNameLength = textureName.length();
				//mHeader.textureIDs[3] = textureName.length();
	

				string ntex = textureName.asChar();
				if (!ntex.empty())
				{
					string path;
					path = ExportingTex(ntex);
					
					memcpy(tHeader.normalName, path.c_str(), path.length());
					tHeader.normalName[path.length()] = '\0';
					mHeader.textureIDs[3] = path.length()+1;
				}

				//cerr << "2: " << test << endl;
				pBar->setValue(pBar->value() + 1);
#pragma endregion

#pragma region Metallic
				MPlug texmetall = MFnDependencyNode(srcNode).findPlug("TEX_metallic_map", &stat);
				MItDependencyGraph dgIt(texmetall, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIt.disablePruningOnFilter();

				MObject metallNode = dgIt.thisNode();
				MPlug filenamePlugm = MFnDependencyNode(metallNode).findPlug("fileTextureName");
				MString textureNamem;


				filenamePlugm.getValue(textureNamem);
				//mHeader.metallicNameLength = textureNamem.length();
				//mHeader.textureIDs[1] = textureNamem.length();
				string mtex = textureNamem.asChar();
				if (!mtex.empty())
				{
					string path;
					path = ExportingTex(mtex);
					memcpy(tHeader.metallicName, path.c_str(), path.length());
					tHeader.metallicName[path.length()] = '\0';
					mHeader.textureIDs[1] = path.length()+1;
				}
				//cerr << "3: " << test << endl;
				pBar->setValue(pBar->value() + 1);


#pragma endregion
#pragma region Roughness
				MPlug texRogugh = MFnDependencyNode(srcNode).findPlug("TEX_roughness_map", &stat);
				MItDependencyGraph dgItr(texRogugh, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgItr.disablePruningOnFilter();

				MObject roughNode = dgItr.thisNode();
				MPlug filenamePlugr = MFnDependencyNode(roughNode).findPlug("fileTextureName");
				MString textureNamer;



				filenamePlugr.getValue(textureNamer);
				//mHeader.roughNameLength = textureNamer.length();
			

				string rtex = textureNamer.asChar();
				if (!rtex.empty())
				{
					string path;
					
					path = ExportingTex(rtex);
					memcpy(tHeader.roughName, path.c_str(), path.length());
					tHeader.roughName[path.length()] = '\0';
					mHeader.textureIDs[2] = path.length()+1;
				}

				//cerr << "4: " << test << endl;
				pBar->setValue(pBar->value() + 1);

#pragma endregion 

#pragma region AO
				MPlug texAo = MFnDependencyNode(srcNode).findPlug("TEX_ao_map", &stat);
				MItDependencyGraph dgIta(texAo, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &stat);
				dgIta.disablePruningOnFilter();

				MObject aoNode = dgIta.thisNode();
				MPlug filenamePluga = MFnDependencyNode(aoNode).findPlug("fileTextureName");
				MString textureNamea;

				filenamePluga.getValue(textureNamea);
				

				//cerr << "5: " << test << endl;
				pBar->setValue(pBar->value() + 1);
				string atex = textureNamea.asChar();
				if (!atex.empty())
				{
					string path;
					path = ExportingTex(atex);
					
					memcpy(tHeader.aoName, path.c_str(), path.length());
					tHeader.aoName[path.length()] = '\0';
					mHeader.textureIDs[4] = path.length()+1;
					
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
	if (bbfExist(this->filePath))
	{
		size_t f = filePath.rfind("/", filePath.length());
		string pAth = filePath.substr(f + 1, filePath.length());

		string materialName = ("Overwrite " + pAth + "?");
		std::wstring stemp = std::wstring(materialName.begin(), materialName.end());
		LPCWSTR sw = stemp.c_str();
		MainHeader s_Head;
		s_Head.id = (unsigned int)std::hash<std::string>{}(filePath);
		this->m_UID = s_Head.id;
		if (MessageBox(NULL, sw, TEXT(".mat Already Exists "), MB_YESNO) == IDYES)
		{
			outFile = new fstream(this->filePath, std::fstream::out | std::fstream::binary);
			s_Head.type = (int)Resources::ResourceType::RES_MATERIAL;

			outFile->write((char*)&s_Head, sizeof(MainHeader));

	/*		outFile->write((char*)&this->tHeader.shaderName, mHeader.textureIDs[0]);
			outFile->write((char*)&this->tHeader.textureName, mHeader.textureIDs[1]);
			outFile->write((char*)&this->tHeader.normalName, mHeader.textureIDs[2]);
			outFile->write((char*)&this->tHeader.metallicName, mHeader.textureIDs[3]);
			outFile->write((char*)&this->tHeader.roughName, mHeader.textureIDs[4]);
			outFile->write((char*)&this->tHeader.aoName, mHeader.textureIDs[5]);*/
			//this->mHeader.textureIDs[0] = (char)this->tHeader.shaderName;
			/*this->mHeader.textureIDs[0] = (unsigned int)this->tHeader.textureName;
			this->mHeader.textureIDs[3] = (char)this->tHeader.normalName;
			this->mHeader.textureIDs[1] = (char)this->tHeader.metallicName;
			this->mHeader.textureIDs[2] = (char)this->tHeader.roughName;
			this->mHeader.textureIDs[4] = (char)this->tHeader.aoName;*/

			outFile->write((char*)&this->mHeader, sizeof(MaterialHeader));

			outFile->write((char*)&this->tHeader.textureName, this->mHeader.textureIDs[0]);
			outFile->write((char*)&this->tHeader.metallicName, this->mHeader.textureIDs[1]);
			outFile->write((char*)&this->tHeader.roughName, this->mHeader.textureIDs[2]);
			outFile->write((char*)&this->tHeader.normalName, this->mHeader.textureIDs[3]);
			outFile->write((char*)&this->tHeader.aoName, this->mHeader.textureIDs[4]);
		
		}
		else
		{
			overWrite = false;
		}
	}
	else
	{
		outFile = new fstream(this->filePath, std::fstream::out | std::fstream::binary);

		MainHeader s_Head;
		s_Head.type = (int)Resources::ResourceType::RES_MATERIAL;
		s_Head.id = (unsigned int)std::hash<std::string>{}(filePath);
		this->m_UID = s_Head.id;

		outFile->write((char*)&s_Head, sizeof(MainHeader));
		//this->mHeader.textureIDs[0] = (char)this->tHeader.shaderName;
		/*this->mHeader.textureIDs[0] = (char)this->tHeader.textureName;
		this->mHeader.textureIDs[1] = (char)this->tHeader.normalName;
		this->mHeader.textureIDs[2] = (char)this->tHeader.metallicName;
		this->mHeader.textureIDs[3] = (char)this->tHeader.roughName;
		this->mHeader.textureIDs[4] = (char)this->tHeader.aoName;*/

		outFile->write((char*)&this->mHeader, sizeof(MaterialHeader));

		//outFile->write((char*)&this->tHeader)
		//outFile->write((char*)&this->tHeader.shaderName, mHeader.shaderNameLength);
		//outFile->write((char*)&this->tHeader.textureName, mHeader.textureNameLength);
		//outFile->write((char*)&this->tHeader.normalName, mHeader.normalNameLength);
		//outFile->write((char*)&this->tHeader.metallicName, mHeader.metallicNameLength);
		//outFile->write((char*)&this->tHeader.roughName, mHeader.roughNameLength);
		//outFile->write((char*)&this->tHeader.aoName, mHeader.aoNameLength);
		outFile->write((char*)&this->tHeader.textureName, this->mHeader.textureIDs[0]);
		outFile->write((char*)&this->tHeader.metallicName, this->mHeader.textureIDs[1]);
		outFile->write((char*)&this->tHeader.roughName, this->mHeader.textureIDs[2]);
		outFile->write((char*)&this->tHeader.normalName, this->mHeader.textureIDs[3]);
		outFile->write((char*)&this->tHeader.aoName, this->mHeader.textureIDs[4]);
	}
	

	
	if (overWrite)
		outFile->close();
}

string MaterialExport::ExportingTex(string file)
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

	string path(newpath_wstr.begin(),newpath_wstr.end());
	return path;
	//return i;

}


