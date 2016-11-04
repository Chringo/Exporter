#include "maya_includes.h"

using namespace std;

void fLoadSkinDeformer(MObject obj)
{

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
            if (dependIt.item().hasFn(MFn::kSkin))
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