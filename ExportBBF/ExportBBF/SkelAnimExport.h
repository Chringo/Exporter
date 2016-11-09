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

    void LoadSkinData(MObject skinNode);

private:

};

#endif 
