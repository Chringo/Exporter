#include <Windows.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcheckbox.h>
#include <QtWidgets\qmainwindow.h>
#include "maya_includes.h"
#include "SkelAnimExport.h"
#include "HeaderStructs.h"
#include "MeshExport.h"
#include <maya/MFnPlugin.h>


using namespace std;

MCallbackIdArray myCallbackArray;
//fstream outFile("//DESKTOP-BOKNO6D/server/knulla.BBF", std::fstream::out | std::fstream::binary);
fstream outFile("pillar.BBF", std::fstream::out | std::fstream::binary);
bool filePath = false;

/*function that starts exporting everything chosen*/
void exportStart(bool skel, bool mats, bool light)
{
	if (skel || mats || light)
	{
		MStatus res = MS::kSuccess;

		/*writing a temporary mainheader*/
		MainHeader tempHead{ 1 };
		outFile.write((char*)&tempHead, sizeof(MainHeader));

		SkelAnimExport cSkelAnim;
		if (skel)
		{
			/*Iterate all skin clusters in scene.*/
			cSkelAnim.IterateSkinClusters();

			/*Iterate all joints in scene.*/
			cSkelAnim.IterateJoints();
		}
		if (true) //furute mesh
		{
			MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
			for (; !meshIt.isDone(); meshIt.next())
			{
				MFnTransform trans = meshIt.currentItem();
				if (trans.child(0).hasFn(MFn::kMesh))
				{
					//Createmesh(meshIt.currentItem(), cSkelAnim);
					MeshExport newMesh(&outFile, &cSkelAnim.skinList);
					newMesh.exportMesh(meshIt.currentItem());
				}
			}
		}
		if (mats)
		{

		}
		if (light)
		{

		}
		if (!skel && !mats && !light)
		{
			MGlobal::displayInfo("ERROR: 0xfded; Nothing checked for export.");
			return;
		}
	}
}

void editClicked()
{

}

/*Function thats called when the export button is pressed*/
void exportClicked()
{
	/*getting the export button from the ui*/
	QWidget * control = MQtUtil::findControl("exportButton");
	QPushButton *cb = (QPushButton*)control;

	/*disabling the export button to ensure no "accidental" double clicks*/
	cb->setDisabled(true);

	/*getting the rest of the ui variables for export info*/
	control = MQtUtil::findControl("skelBox");
	bool skel = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("matBox");
	bool mats = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("lightBox");
	bool light = ((QCheckBox*)control)->checkState();



	MGlobal::displayInfo("in export");

}



EXPORT MStatus initializePlugin(MObject obj)
{
	/*initializing the ui*/
	MGlobal::executeCommand("string $dialog = `loadUI - uiFile ""mainwindow.ui""`");
	MGlobal::executeCommand("showWindow $dialog");
	QWidget * control = MQtUtil::findControl("exportButton");
	QPushButton *cb = (QPushButton*)control;
	
	/*connecting the export button to the exportClicked function*/
	QObject::connect(cb, &QPushButton::clicked, [] {exportClicked(); });
	
	control = MQtUtil::findControl("editButton");
	QPushButton *eb = (QPushButton*)control;
	QObject::connect(eb, &QPushButton::clicked, [] {editClicked(); });

    SkelAnimExport cSkelAnim;
	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	//ofstream outFile("test", std::ofstream::binary);
	if (!outFile.is_open())
	{
		MGlobal::displayError("ERROR: the binary file is not open");
		
	}

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}
	
	MGlobal::displayInfo("Maya plugin loaded!");

    /*Iterate all skin clusters in scene.*/
    cSkelAnim.IterateSkinClusters();

    /*Iterate all joints in scene.*/
    cSkelAnim.IterateJoints();
	
	/*writing a temporary mainheader for one mesh*/
	MainHeader tempHead{ 1 };
	outFile.write((char*)&tempHead, sizeof(MainHeader));

	MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnTransform trans = meshIt.currentItem();
		if (trans.child(0).hasFn(MFn::kMesh))
		{
			//Createmesh(meshIt.currentItem(), cSkelAnim);
			MeshExport newMesh(&outFile, &cSkelAnim.skinList);
			newMesh.exportMesh(meshIt.currentItem());
		}
	}

	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	MGlobal::executeCommand("deleteUI -window $dialog");
	//need to force close the ui

	//QObject::disconnect(cb, exportClicked());
	
	//delete cb;

	outFile.close();
	// if any resources have been allocated, release and free here before
	// returning...
	MMessage::removeCallbacks(myCallbackArray);


	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}