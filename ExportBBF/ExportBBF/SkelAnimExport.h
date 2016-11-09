#ifndef SKELANIMEXPORT_H
#define SKELANIMEXPORT_H
 
#include "maya_includes.h"
#include <vector>

struct hSkinData
{
    float weights[4];
    unsigned int boneInfluences[4];
};

class SkelAnimExport
{
public:

    SkelAnimExport();
    ~SkelAnimExport();

    std::vector<hSkinData> skinList;

    void IterateSkinClusters();
    void IterateJoints();

    void LoadSkinData(MObject skinNode);
    void LoadJointData(MObject jointNode);

private:

};

#endif 
