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

	unsigned int m_UID = 0;
public:

	unsigned int getUID() { return this->m_UID; }

	void MaterialExtraction();
	void ExportingMats_Tex();
	void ExportingTex(string file);
	
};

