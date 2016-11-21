#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include <vector>
#include <map>

#define PI 3.14159265

struct hSkinData
{
    float weights[4];
    unsigned int boneInfluences[4];
};


struct hKeyData
{
    float timeValue;
    float translation[3];
    float quaternion[4];
    float scale[3];
};

struct hAnimationStateData
{
    std::vector<hKeyData>keyFrames;
};

struct hJointData
{
    float bindPose[16];
    int parentIndex;
    int jointIndex;

    std::vector<hAnimationStateData>animationStates;
};

class SkelAnimExport
{
public:

    SkelAnimExport();
    ~SkelAnimExport();

    std::vector<hSkinData> skinList;
    std::vector<hJointData> jointList;

    void IterateSkinClusters();
    void IterateJoints();
    void IterateAnimations();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode, int parentIndex, int currentIndex);

private:
    /*Function that converts a MMatrix to a float[16] array.*/
    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);
};

#endif 
