#ifndef MESH_EXPORT_H
#define MESH_EXPORT_H
#include "HeaderStructs.h"
#include "maya_includes.h"
#include <QtWidgets\qprogressbar.h>

using namespace std;
class MeshExport
{
private:
	vector<Vertex> * vertices = nullptr;
	vector<unsigned int> * newIndex = nullptr;
	vector<SkinData> * skinList = nullptr;

	unsigned int m_UID = 0;

	unsigned int jointCount;
	bool overWrite = true;

	fstream * outFile;
public:
	MeshExport();
	MeshExport(string &filePath);
	MeshExport(string & filePath, vector<SkinData> * skinList);
	~MeshExport();
	void exportMesh(MObject & mNode);
	static int getProgressBarValue();

	unsigned int getUID(){ return this->m_UID; }
private:
	/*export a mesh with a skeleton*/
	void exportDynamic(MFnMesh & mMesh, MFnTransform & mTran);

	/*ecport a mesh without any skeleton*/
	void exportStatic(MFnMesh & mMesh, MFnTransform & mTran);
};
#endif 

