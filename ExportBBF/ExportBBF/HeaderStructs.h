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

struct JointData
{
	float inverseBindPose[16];
	int parentIndex;
};
#pragma endregion
#pragma region header structs
struct MainHeader
{
	unsigned int meshes;
	unsigned int numOfMats;
};
struct MeshHeader
{
	unsigned int vertices;
	unsigned int indexLength;
	//MMatrix transMatrix;
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
	char shaderName[256];
	float roughness;
	float metallness;
	float emissive;
	char textureMap[256];
	char texFileName[256];
	char normalMap[256];
	char norFileName[256];
	char metallicMap[256];
	char metalFileName[256];
	char roughnessMap[256];
	char roughFileName[256];
	char aoMap[256];
	char aoFileName[256];
};
#pragma endregion