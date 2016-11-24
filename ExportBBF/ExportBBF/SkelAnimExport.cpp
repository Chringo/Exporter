#include "SkelAnimExport.h"



SkelAnimExport::SkelAnimExport(std::fstream * outFile)
{
	this->outFile = outFile;
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

    MItDag jointIter(MItDag::kDepthFirst, MFn::kJoint, &res);

    if (res == MStatus::kSuccess)
    {
        LoadJointData(jointIter.item(), -1, 0);
    }
}

void SkelAnimExport::IterateAnimations()
{
    MStatus res;

    MFnIkJoint jointFn;

    /*Iterates all animation layers and setting their weight value to 0.
    Later we want this because processing every layer requires all others
    to be set to 0, otherwise probably we can't get the keyframe values.*/
    MPlugArray layerWeights;

    MItDependencyNodes layerWeightIter(MFn::kAnimLayer, &res);
    if (res == MStatus::kSuccess)
    {
        while (!layerWeightIter.isDone())
        {
            MFnDependencyNode animLayerFn(layerWeightIter.item(), &res);

            MPlug weightLayerPlug = animLayerFn.findPlug("weight", &res);
            MPlug soloPlug = animLayerFn.findPlug("solo", &res);
            MPlug mutePlug = animLayerFn.findPlug("parentMute", &res);

            weightLayerPlug.setDouble(0);
            soloPlug.setBool(0);
            mutePlug.setBool(0);

            layerWeights.append(weightLayerPlug);

            layerWeightIter.next();
        }
    }

    int plugWeightCounter = 0;

    MItDependencyNodes animLayerIter(MFn::kAnimLayer, &res);
    if (res == MStatus::kSuccess)
    {
        while (!animLayerIter.isDone())
        {
            int jointCounter = 0;

            MFnDependencyNode animLayerFn(animLayerIter.item(), &res);
            if (animLayerFn.name() == "BaseAnimation")
            {
                animLayerIter.next();
                layerWeights[plugWeightCounter].setDouble(1);
                plugWeightCounter++;
                continue;
            }

            MString animLayerName = animLayerFn.name();
            
            layerWeights[plugWeightCounter].setDouble(1);

            MPlug blendNodePlug = animLayerFn.findPlug("blendNodes", &res);
            if (res == MStatus::kSuccess)
            {
                MItDependencyGraph blendIter(animLayerIter.item(), MFn::kBlendNodeAdditiveRotation,
                    MItDependencyGraph::Direction::kDownstream, MItDependencyGraph::Traversal::kBreadthFirst,
                    MItDependencyGraph::Level::kNodeLevel, &res);

                blendIter.enablePruningOnFilter();

                while (!blendIter.isDone())
                {
					if (jointCounter >= jointList.size())
						break;

                   AnimationStateHeader animStateData;

				   MFnDependencyNode blendFn(blendIter.currentItem(), &res);

                   MPlug outputPlug = MFnDependencyNode(blendIter.currentItem()).findPlug("rotateOrder", &res);
                   if (res == MStatus::kSuccess)
                   {
                       MString plugName = outputPlug.name();
                       MPlugArray outputConnection;
                       outputPlug.connectedTo(outputConnection, true, false, &res);
                       if (outputConnection.length())
                       {
                           jointFn.setObject(outputConnection[0].node());
                       }
                   }

				   MPlug inputBPlug = blendFn.findPlug("inputB", &res);

                   if (res == MStatus::kSuccess)
                   {
					   MObjectArray objArray;
                      
                       MAnimUtil::findAnimation(inputBPlug.child(0), objArray, &res);
       
                       if (objArray.length())
                       {
                           MFnAnimCurve animCurveFn(objArray[0], &res);

                           int numKeys = animCurveFn.numKeyframes(&res);
                           for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
                           {
                               KeyframeHeader keyData;

                               MTime keyTime = animCurveFn.time(keyIndex);
                               keyData.timeValue = keyTime.as(MTime::kSeconds);

                               MAnimControl::setCurrentTime(keyTime);

                               double quaternion[4];
                               jointFn.getRotationQuaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3], MSpace::kTransform);
                               std::copy(quaternion, quaternion + 4, keyData.quaternion);

                               MVector transVec = jointFn.getTranslation(MSpace::kTransform, &res);
                               double translation[3];
                               transVec.get(translation);
                               std::copy(translation, translation + 3, keyData.translation);

                               double scale[3];
                               jointFn.getScale(scale);
                               std::copy(scale, scale + 3, keyData.scale);

                               animStateData.keyFrames.push_back(keyData);
                           }

						   animStateData.keyFrameCount = animStateData.keyFrames.size();

                           jointList[jointCounter].animationStates.push_back(animStateData);
						   jointList[jointCounter].animStateCount = jointList[jointCounter].animationStates.size();
                           jointCounter++;
                       }
                   }
                   
                   blendIter.next();
                }
            }

            plugWeightCounter++;
            animLayerIter.next();
        }
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
                    /*Obtain each control vertex componoment object.*/
                    MObject component = geometryIter.component(&res);

                    int cvIndex = geometryIter.index();

                    MIntArray inflIndexArray;

                    /*Append each joint's dagpath index to the array.*/
                    for (unsigned jointIndex = 0; jointIndex < jointArrayLength; jointIndex++)
                    {
                        inflIndexArray.append(skinFn.indexForInfluenceObject(jointPathArray[jointIndex]));
                    }

                    MDoubleArray weights;
                    skinFn.getWeights(skinPath, component, inflIndexArray, weights);

                    int weightsLength = weights.length();

                    for (int i = 0; i < weightsLength; i++)
                    {
						/*If a joint has zero weight value, skip it for this control vertex.
						There is always 4 joints influencing a control vertex.*/
						if (weights[i] != 0)
                        {
                            skinList[cvIndex].weights[weightsCounter] = weights[i];
                            skinList[cvIndex].boneInfluences[influenceCounter] = inflIndexArray[i];

                            weightsCounter++, influenceCounter++;

                            weightSum += weights[i];
                        }
                    }

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

    JointHeader jointData;

    MFnIkJoint jointFn(jointNode, &res);
    if (res == MStatus::kSuccess)
    {
        /*Obtain the plug for every joint's bindpose matrix.*/
        MPlug bindPosePlug = jointFn.findPlug("bindPose", &res);
        if (res == MStatus::kSuccess)
        {
           MString jointName = jointFn.name();

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

void SkelAnimExport::ExportSkelAnimData()
{
	JointHeader mJointHead;

	const int jointCount = jointList.size();

	outFile->write((const char*)jointList.data(), sizeof(JointHeader) * jointCount);

	for (int jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		const int animStateCount = jointList[jointIndex].animationStates.size();

		outFile->write((const char*)jointList[jointIndex].animationStates.data(), sizeof(AnimationStateHeader) * animStateCount);

		for (int animIndex = 0; animIndex < animStateCount; animIndex++)
		{
			const int keyFrameCount = jointList[jointIndex].animationStates[animIndex].keyFrames.size();

			outFile->write((const char*)jointList[jointIndex].animationStates[animIndex].keyFrames.data(), sizeof(KeyframeHeader) * keyFrameCount);
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
