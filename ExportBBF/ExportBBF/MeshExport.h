#ifndef MESH_EXPORT_H
#define MESH_EXPORT_H
#include "HeaderStructs.h"
#include "maya_includes.h"

class MeshExport
{
private:
	Vertex * vertices;
	unsigned int * index;
public:
	MeshExport();
	~MeshExport();
	void exportMesh(MObject & mNode);
};
#endif 

