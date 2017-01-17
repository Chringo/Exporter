#ifndef MESH_EXPORT_H
#define MESH_EXPORT_H
#include "HeaderStructs.h"
#include "maya_includes.h"
#include <QtWidgets\qprogressbar.h>
#include "BoundingExport.h"

using namespace std;
class MeshExport
{
private:
	vector<Vertex> * vertices = nullptr;
	vector<unsigned int> * newIndex = nullptr;
	vector<SkinData> * skinList = nullptr;
	BoundingExport newBox;
	

	unsigned int m_UID = 0;

	std::string filePath;
	unsigned int jointCount;
	bool overWrite = true;

	fstream * outFile;
public:
	BoundingBoxHeader obbHead;
	BoundingBoxHeader* getObbHead() { return &obbHead; }
	MeshExport();
	MeshExport(string &filePath);
	MeshExport(string & filePath, vector<SkinData> * skinList);
	~MeshExport();
	void exportMesh(MObject & mNode,bool customObb);
	static int getProgressBarValue();
	void GenerateID(std::string *filePath = nullptr);

	unsigned int getUID(){ return this->m_UID; }

	void exportCustomObb(MStatus &res);
private:
	/*export a mesh with a skeleton*/
	void exportDynamic(MFnMesh & mMesh, MFnTransform & mTran);

	/*ecport a mesh without any skeleton*/
	void exportStatic(MFnMesh & mMesh, MFnTransform & mTran,bool customObb);
};
#endif 

