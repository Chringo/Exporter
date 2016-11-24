#pragma once
#include "HeaderStructs.h"
#include "maya_includes.h"

using namespace std;
class MaterialExport
{
public:
	MaterialExport();
	MaterialExport(fstream  *outFile,string filePath);
	~MaterialExport();
	//MaterialHeader mHeader;
private:
	string filePath;
	fstream * outFile;
public:

	void GetNumMats();
	void MaterialExtraction();
	void ExportingMats_Tex();
	void ExportingTex(string file);
	
};

