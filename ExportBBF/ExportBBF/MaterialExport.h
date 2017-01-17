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

	void generateID(string *filePath = nullptr);
	//MaterialHeader mHeader;
private:
	string filePath;
	string matname1;
	fstream * outFile;
	bool overWrite = true;
	MaterialHeader mHeader;
	TextureHeader tHeader;
	unsigned int textureNameLength[5] = { 0,0,0,0,0 };

	unsigned int m_UID = 0;
public:

	unsigned int getUID() { return this->m_UID; }

	void MaterialExtraction(bool customObb);
	void ExportingMats_Tex();
	string ExportingTex(string file);
	
};

