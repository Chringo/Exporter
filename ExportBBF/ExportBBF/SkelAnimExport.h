#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include "HeaderStructs.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define PI 3.14159265

class SkelAnimExport
{
public:
	SkelAnimExport();
	SkelAnimExport(string &filePath);
    ~SkelAnimExport();

    std::vector<SkinData> skinList;
    
    void IterateSkinClusters();
    void IterateJoints();
    void IterateAnimations(bool anims);

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode, int parentIndex, int currentIndex);

	void setFilePath(string &filePath);
	void setMeshName(string &meshName);

	void writeJointData();

	unsigned int getUID() { return this->m_UID; }

private:

    /*Function that converts a MMatrix to a float[16] array.*/
    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);
	unsigned int nrOfAnimLayers = 0;

	std::vector<LayerIdHeader> animIdList;
	std::vector<JointHeader> jointList;

	unsigned int m_UID = 0;

	string m_filePath;
	string m_meshName;
};

#endif 
