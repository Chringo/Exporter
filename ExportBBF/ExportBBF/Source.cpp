#include "maya_includes.h"
#include <vector>

using namespace std;

struct hBlendData
{
    std::vector<float> weights;
    std::vector<unsigned int> jointInfluence;
};

static std::vector<hBlendData> blendList;

void fLoadSkinDeformer(MObject obj)
{
	MStatus res;
	hBlendData blendData;
    float weightSum = 0;

	MFnSkinCluster skinFn(obj, &res);
	if (res == MStatus::kSuccess)
	{
		MDagPathArray influences;

		MGlobal::displayInfo("I'M A SKIN CLUSTER! Name: " + skinFn.name());

        /*Find the number of geometries connecting to the skinCluster, in this case it's always 1 mesh connected.*/
		unsigned int numGeoms = skinFn.numOutputConnections(&res);
		for (unsigned int geomIndex = 0; geomIndex < numGeoms; geomIndex++)
		{
            /*Obtain the index for the connected mesh.*/
			unsigned int index = skinFn.indexForOutputConnection(geomIndex, &res);

            /*Obtain influence count, which is the joints controlling the binded skin mesh.*/
			MDagPathArray jointPathArray;
			skinFn.influenceObjects(jointPathArray, &res);
			unsigned int jointArrayLength = jointPathArray.length();

            /*Find the path to the connected mesh, and iterate it's control vertices. */
			MDagPath skinPath;
			if (skinFn.getPathAtIndex(index, skinPath))
			{
                /*Iterator for the path to the binded skin mesh, iterates all control vertices.*/
				MItGeometry geometryIter(skinPath, &res);

                /*Total count of all control points in Mesh, need to be readjusted for indices.*/
                int controlVerticesCount = geometryIter.count(&res);

                /*Resize the blend data list to be the size of control vertices count.*/
                blendList.resize(controlVerticesCount);

                while (!geometryIter.isDone())
                {
                    MGlobal::displayInfo(MString("CV index: ") + geometryIter.index());
                    /*Obtain each control vertex componoment object.*/
                    MObject component = geometryIter.component(&res);

                    int cvIndex = geometryIter.index();

                    MIntArray inflIndexArray;

                    /*Append each joint's dagpath index to the array.*/
                    for (unsigned jointIndex = 0; jointIndex < jointArrayLength; jointIndex++)
                    {
                        inflIndexArray.append(skinFn.indexForInfluenceObject(jointPathArray[jointIndex]));

                        //MGlobal::displayInfo(MString("Influence object index: ") + inflIndexArray[jointIndex]);
                    }

                    MDoubleArray weights;
                    unsigned int inflCount;
                    skinFn.getWeights(skinPath, component, inflIndexArray, weights);

                    int weightsLength = weights.length();

                    for (int i = 0; i < weightsLength; i++)
                    {
                        if (weights[i] == 0)
                        {
                            //MGlobal::displayInfo(jointPathArray[i].partialPathName().asChar());
                            //MGlobal::displayInfo("ZERO WEIGHT!");
                        }

                        else
                        {
                            MGlobal::displayInfo(jointPathArray[i].partialPathName().asChar());
                            MGlobal::displayInfo(MString() + weights[i]);

                            blendList[cvIndex].weights.push_back(weights[i]);
                            blendList[cvIndex].jointInfluence.push_back(inflIndexArray[i]);

                            weightSum += weights[i];
                        }
                    }

                    MGlobal::displayInfo(MString("Weight sum: ") + weightSum + "\n");
                    weightSum = 0;

                    geometryIter.next();
				}
			}
		}
	}
}

EXPORT MStatus initializePlugin(MObject obj)
{
	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

    MItDependencyNodes dependIt(MFn::kDependencyNode, &res);

    if (res == MStatus::kSuccess)
    {
        while (!dependIt.isDone())
        {
			if (dependIt.item().hasFn(MFn::kSkinClusterFilter))
			{
			
				fLoadSkinDeformer(dependIt.item());
			}

            dependIt.next();
        }
        
    }

	MGlobal::displayInfo("Maya plugin loaded!");
	// if res == kSuccess then the plugin has been loaded,
	// otherwise is has not.
	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	// if any resources have been allocated, release and free here before
	// returning...

	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}