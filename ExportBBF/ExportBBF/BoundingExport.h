#pragma once
#include "maya_includes.h"
#include "HeaderStructs.h"
class BoundingExport
{
public:
	BoundingExport();
	BoundingExport(std::string &filePath);
	~BoundingExport();
	void exportBoundingBox(MObject &mNode);
	static int getProgressBar();

	//test
	BoundingBoxHeader* getObbHead() { return &obbHead; }

private:
	BoundingBoxHeader obbHead;
	std::fstream* outFile;
	void exportOBB(MFnMesh & mMesh, MFnTransform & mTran);
public:

};


