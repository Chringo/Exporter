#ifndef MESH_EXPORT_H
#define MESH_EXPORT_H
#include "HeaderStructs.h"
#include "maya_includes.h"

using namespace std;
class MeshExport
{
private:
	vector<Vertex> * vertices;
	vector<unsigned int> * newIndex;

	fstream * outFile;
public:
	MeshExport();
	MeshExport(fstream * outFile);
	~MeshExport();
	void exportMesh(MObject & mNode);
};
#endif 

