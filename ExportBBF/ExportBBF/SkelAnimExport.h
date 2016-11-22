#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include "HeaderStructs.h"
#include <vector>



class SkelAnimExport
{
public:

    SkelAnimExport();
    ~SkelAnimExport();

    std::vector<SkinData> skinList;
    std::vector<JointData> jointList;

    std::vector<MMatrix> invBindPoseList;

    void IterateSkinClusters();
    void IterateJoints();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode);

private:

    void ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16]);
};

#endif 
