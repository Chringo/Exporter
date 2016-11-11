#include "SkelAnimExport.h"

SkelAnimExport::SkelAnimExport()
{
}

SkelAnimExport::~SkelAnimExport()
{
}

void SkelAnimExport::IterateSkinClusters()
{
    MStatus res;

    MItDependencyNodes skinIter(MFn::kSkinClusterFilter, &res);
    if (res == MStatus::kSuccess)
    {
        while (!skinIter.isDone())
        {
            LoadSkinData(skinIter.item());

            skinIter.next();
        }
    }
}

void SkelAnimExport::IterateJoints()
{
    MStatus res;

    MItDag dependIter(MItDag::kDepthFirst, MFn::kJoint, &res);

    if (res == MStatus::kSuccess)
    {
        LoadJointData(dependIter.item(), -1, 0);
    }
}

void SkelAnimExport::LoadSkinData(MObject skinNode)
{
    MStatus res;

    float weightSum = 0;

    int weightsCounter = 0;
    int influenceCounter = 0;

    MFnSkinCluster skinFn(skinNode, &res);
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
                skinList.resize(controlVerticesCount);

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

                            skinList[cvIndex].weights[weightsCounter] = weights[i];
                            skinList[cvIndex].boneInfluences[influenceCounter] = inflIndexArray[i];

                            weightsCounter++, influenceCounter++;

                            weightSum += weights[i];
                        }
                    }

                    MGlobal::displayInfo(MString("Weight sum: ") + weightSum + "\n");
                    weightSum = 0;
                    weightsCounter = 0, influenceCounter = 0;

                    geometryIter.next();
                }
            }
        }
    }
}

void SkelAnimExport::LoadJointData(MObject jointNode, int parentIndex, int currentIndex)
 {
    MStatus res;

    hJointData jointData;

    MFnIkJoint jointFn(jointNode, &res);
    if (res == MStatus::kSuccess)
    {
        /*Obtain the plug for every joint's bindpose matrix.*/
        MPlug bindPosePlug = jointFn.findPlug("bindPose", &res);
        if (res == MStatus::kSuccess)
        {
           MString jointName = jointFn.name();
           MGlobal::displayInfo("Current joint: " + jointName);

           /*Get the matrix as a MObject.*/
           MObject bpNode;
           bindPosePlug.getValue(bpNode);

           /*Retrieve the matrix data from the bindpose MObject.*/
           MFnMatrixData bindPoseFn(bpNode, &res);
           
           /*The actual bindpose matrix is obtained here from every joint.*/
           MMatrix bindPose = bindPoseFn.matrix(&res);
           if (res == MStatus::kSuccess)
           {
               float inverseBindPose[16];
               /*Convert MMatrix bindpose to a float[16] array.*/
               ConvertMMatrixToFloatArray(bindPose, inverseBindPose);
               memcpy(jointData.bindPose, &inverseBindPose, sizeof(float) * 16);

               /*Assign both current joint ID and it's parent ID.*/
               jointData.parentIndex = parentIndex;
               jointData.jointIndex = currentIndex;

               currentIndex++;

               jointList.push_back(jointData);
           }
        }
    }

    /*This is where the recursive happens. Basically for a "CHEST" joint, both the l_arm and r_arm,
    will know that both are parents to "CHEST" joint.*/
    if (jointFn.childCount() > 0)
    {
        for (int childIndex = 0; childIndex < jointFn.childCount(); childIndex++)
        {
            /*If there is a child in the skeleton hierarchy, decrement the currentIndex, which is now parentIndex.*/
            LoadJointData(jointFn.child(childIndex), currentIndex - 1, jointList.size());
        }
    }
}

void SkelAnimExport::ConvertMMatrixToFloatArray(MMatrix inputMatrix, float outputMatrix[16])
{
    int matrixCounter = 0;

    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            outputMatrix[matrixCounter] = inputMatrix[row][column];

            matrixCounter++;
        }
    }
}
