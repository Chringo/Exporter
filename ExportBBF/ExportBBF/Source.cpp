#include "maya_includes.h"
#include "SkelAnimExport.h"
#include <maya/MFnPlugin.h>
#include <vector>

EXPORT MStatus initializePlugin(MObject obj)
{
    SkelAnimExport cSkelAnim;

	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

	MGlobal::displayInfo("Maya plugin loaded!");

    /*Iterate all skin clusters in scene.*/
    cSkelAnim.IterateSkinClusters();
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