#include "maya_includes.h"
#include <vector>

using namespace std;

struct hBlendData
{
	float weights[4];
	unsigned int influence[4];
};

std::vector<hBlendData> blendList;

void fLoadSkinDeformer(MObject obj)
{
	MStatus res;
	hBlendData blendData;

	MFnSkinCluster skinFn(obj, &res);
	if (res == MStatus::kSuccess)
	{
		MDagPathArray influences;

		MGlobal::displayInfo("I'M A SKIN CLUSTER! Name: " + skinFn.name());

		unsigned int numGeoms = skinFn.numOutputConnections(&res);

		for (unsigned int geomIndex = 0; geomIndex < numGeoms; geomIndex++)
		{
			unsigned int index = skinFn.indexForOutputConnection(geomIndex, &res);

			MDagPathArray jointPathArray;
			skinFn.influenceObjects(jointPathArray, &res);
			unsigned int jointArrayLength = jointPathArray.length();

			for (unsigned jointIndex = 0; jointIndex < jointArrayLength; jointIndex++)
				MGlobal::displayInfo(jointPathArray[jointIndex].partialPathName().asChar());

			MDagPath skinPath;
			if (skinFn.getPathAtIndex(index, skinPath))
			{
				MItGeometry geometryIter(skinPath, &res);
				while (!geometryIter.isDone())
				{
					MObject component = geometryIter.component(&res);

					/*Populate the influence index array, which we use to get the weights of each vertex.*/
					MIntArray inflIndexArray;
					MIntArray allIndexArray;
				
					for (unsigned jointIndex = 0; jointIndex < jointArrayLength; jointIndex++)
					{
						allIndexArray.append(skinFn.indexForInfluenceObject(jointPathArray[jointIndex]));
					}

					//if (pathArray.length() > 0)
					//{
					//	for (unsigned int j = 0; j < pathArray.length(); j++)
					//	{
					//		unsigned int index = skinFn.indexForInfluenceObject(pathArray[j], &res);

					//		for (unsigned int k = 0; allIndexArray.length(); k++)
					//		{
					//			if ((int)index == allIndexArray[k])
					//			{
					//				inflIndexArray.append(k);
					//			}
					//		}
					//	}
					//}

					//else
					//{
					//	for (unsigned inflIndex = 0; inflIndex < allIndexArray.length(); inflIndex++)
					//	{
					//		inflIndexArray.append(inflIndex);
					//	}
					//}

					///*Obtain the weights for the affected vertex.*/
					//MDoubleArray weights;

					//if (skinFn.getWeights(skinPath, component, inflIndexArray, weights))
					//{
					//	MGlobal::displayInfo(MString("Weights: \n") +
					//		MString(" x: ") + weights[0] + 
					//		MString(" y: ") + weights[1] +
					//		MString(" z: ") + weights[2] +
					//		MString(" w: ") + weights[3]);

					//	blendData.weights[0] = float(weights[0]);
					//	blendData.weights[1] = float(weights[1]);
					//	blendData.weights[2] = float(weights[2]);
					//	blendData.weights[3] = float(weights[3]);

					//	blendList.push_back(blendData);
					//}

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