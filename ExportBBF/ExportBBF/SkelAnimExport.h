#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include <vector>

struct hSkinData
{
    float weights[4];
    unsigned int boneInfluences[4];
};

struct hJointData
{
    float bindPose[16];
    int parentIndex;
    int jointIndex;
};

class SkelAnimExport
{
public:

    SkelAnimExport();
    ~SkelAnimExport();

    std::vector<hSkinData> skinList;
    std::vector<hJointData> jointList;

    std::vector<MMatrix> invBindPoseList;

    void IterateSkinClusters();
    void IterateJoints();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode, int parentIndex, int currentIndex);

private:
    /*Function that converts a MMatrix to a float[16] array.*/
    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);
};

#endif 
