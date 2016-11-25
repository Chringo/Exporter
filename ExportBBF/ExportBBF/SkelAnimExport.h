#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include "HeaderStructs.h"
#include <vector>

#define PI 3.14159265

class SkelAnimExport
{
public:

    SkelAnimExport(std::fstream* outFile);
    ~SkelAnimExport();

    std::vector<SkinData> skinList;
    std::vector<JointHeader> jointList;

    void IterateSkinClusters();
    void IterateJoints();
    void IterateAnimations();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode, int parentIndex, int currentIndex);

	void ExportSkelAnimData();

private:

	struct AnimationPerJoint
	{
		std::vector<KeyframeHeader> keyframes;
	};

	struct tempJointStruct
	{
		std::vector<AnimationPerJoint> animationData; 
		std::vector<AnimationStateHeader> animationCount;
	};

	std::vector<tempJointStruct> tempAnimations;

    /*Function that converts a MMatrix to a float[16] array.*/
    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);

	std::fstream* outFile;
};

#endif 
