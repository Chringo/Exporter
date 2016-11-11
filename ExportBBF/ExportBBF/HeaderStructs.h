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
#pragma endregion
#pragma region header structs
struct MainHeader
{
	unsigned int meshes;
};
struct MeshHeader
{
	unsigned int vertices;
	unsigned int indexLength;
	MMatrix transMatrix;
};
struct Vertex
{
	Vector3 position;
	Vector3 normal;
	//Vector3 tangent;
	Vector2 UV;
};
#pragma endregion