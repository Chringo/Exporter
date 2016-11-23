#pragma once
#include "HeaderStructs.h"
#include "maya_includes.h"

using namespace std;
class MaterialExport
{
public:
	MaterialExport();
	MaterialExport(fstream  *outFile);
	~MaterialExport();
	//MaterialHeader mHeader;
private:

	fstream * outFile;
public:

	void GetNumMats();
	void MaterialExtraction();
	void ExportingMats_Tex();
	
};

