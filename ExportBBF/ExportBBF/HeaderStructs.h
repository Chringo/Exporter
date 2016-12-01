#pragma once
#pragma region utilStructs
#include "maya_includes.h"
struct Vector2
{
	float u, v;
};
struct Vector3
{
	float x, y, z;
	/*put overload function here*/
	Vector3& operator=(const MFloatVector other)
	{
		x = (float)other.x;
		y = (float)other.y;
		z = (float)other.z;
		return *this;
	}
};
struct Vector4
{
	float x, y, z, w;
};
struct SkinData
{
	float weights[4];
	unsigned int boneInfluences[4];
};

struct JointAnimHeader
{
	unsigned int jointCount;
};

struct JointKeyFrameHeader
{
	unsigned int numKeys;
};

struct KeyframeHeader
{
	float timeValue;
	float translation[3];
	float quaternion[4];
	float scale[3];
};



//struct KeyframeHolder
//{
//	std::vector<KeyframeHeader> keyframes;
//};
//
//struct AnimationJointHeader
//{
//	std::vector<KeyframeHolder> animationData;
//	std::vector<AnimationStateHeader> animationCount;
//};

#pragma endregion
#pragma region header structs
struct SkeletonHeader
{
	unsigned int jointCount;
	//int skeletonId;
};

struct JointHeader
{
	float bindPose[16];
	int jointIndex;
	int parentIndex;
};
struct MainHeader
{
	int type;
	unsigned int id;
};
struct MeshHeader
{
	unsigned int vertices;
	unsigned int indexLength;
	//MMatrix transMatrix;
	//unsigned int jointCount;
	bool hasSkeleton;
};
struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 UV;
};
struct SkelVertex
{
	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 UV;
	float weights[4];
	int influence[4];
};
struct MaterialHeader
{
	//int numOfMats;
	//char shaderName[256];
	float roughness;
	float metallness;
	float emissive;
	unsigned int shaderNameLength;
	unsigned int textureNameLength;
	unsigned int normalNameLength;
	unsigned int metallicNameLength;
	unsigned int woofNameLength;
	unsigned int aoNameLength;

};

namespace Resources
{
	enum ResourceType
	{
		RES_UNKOWN = -1,
		RES_TEXTURE = 0,
		RES_MATERIAL = 1,
		RES_MESH = 2,
		RES_MODEL = 3,
		RES_SKELETON = 4,
		RES_SOUND = 5,
		RES_LIGHT = 6,
		RES_ANIMATION = 7,
		RES_UI = 8
	};
}
//struct matsHead
//{
//	float roughness;
//	float metallness;
//	float emissive;
//	unsigned int shaderNameLength;
//	unsigned int textureNameLength;
//	unsigned int normalNameLength;
//	unsigned int metallicNameLength;
//	unsigned int woofNameLength;
//	unsigned int aoNameLength;
//};
/*
1: fyll matsHead
2: skriv in shadergrejer i följd
*/
#pragma endregion