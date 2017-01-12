#pragma once
#include "HeaderStructs.h"
#include "maya_includes.h"
#include <QtWidgets\qprogressbar.h>

using namespace std;
class MaterialExport
{
public:
	MaterialExport();
	MaterialExport(string &filePath);
	~MaterialExport();
	//MaterialHeader mHeader;
private:
	string filePath;
	fstream * outFile;
	bool overWrite = true;
	MaterialHeader mHeader;
	TextureHeader tHeader;
	unsigned int textureNameLength[5] = { 0,0,0,0,0 };

	unsigned int m_UID = 0;
public:

	unsigned int getUID() { return this->m_UID; }

	void MaterialExtraction(MFnMesh &fnmMesh);
	void ExportingMats_Tex();
	string ExportingTex(string file);
	
};

