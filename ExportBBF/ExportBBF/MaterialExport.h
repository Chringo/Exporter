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
public:

	void MaterialExtraction();
	void ExportingMats_Tex();
	void ExportingTex(string file);
	
};

