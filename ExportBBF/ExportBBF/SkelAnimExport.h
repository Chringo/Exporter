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
    void IterateAnimations();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode, int parentIndex, int currentIndex);

	void addToFilePath(string &filePath);

	void writeJointData();

private:

    /*Function that converts a MMatrix to a float[16] array.*/
    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);
	unsigned int nrOfAnimLayers = 0;

	std::vector<LayerIdHeader> animIdList;
	std::vector<JointHeader> jointList;

	//std::fstream* outFile;

	string m_filePath;
};

#endif 
