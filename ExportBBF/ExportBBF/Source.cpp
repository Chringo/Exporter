#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qprogressbar.h>
#include <QtWidgets\qcheckbox.h>
#include <QtWidgets\qmainwindow.h>
#include <QtWidgets\qfiledialog.h>
#include <QtWidgets\qlineedit.h>
#include "maya_includes.h"
#include "SkelAnimExport.h"
#include "HeaderStructs.h"
#include "MeshExport.h"
#include <maya/MFnPlugin.h>
#include <string.h>
#include "MaterialExport.h"


using namespace std;

MCallbackIdArray myCallbackArray;

void setProcessBarSize()
{
	QWidget *control = MQtUtil::findControl("progressBar");
	QProgressBar *pBar = (QProgressBar*)control;

	//textur = 5

	pBar->setMaximum(5);
	pBar->setValue(0);
}

/*function that starts exporting everything chosen*/
void exportStart(bool mesh, bool skel, bool mats, bool light, string filePath)
{
	if (mesh || skel || mats || light)
	{
		MStatus res = MS::kSuccess;

		fstream outFile(filePath, std::fstream::out | std::fstream::binary);

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

			/*Iterate all animations in the skeleton.*/
			cSkelAnim.IterateAnimations();
		}
		if (mesh)
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
			MaterialExport newMat(&outFile,filePath);
			newMat.MaterialExtraction();
			
			

		}
		if (light)
		{
			
		}

		/*making the buttons clickable again and closing the file*/
		outFile.close();
		MGlobal::displayInfo("Done with the export!");
		QWidget * control = MQtUtil::findControl("exportButton");
		QPushButton *cb = (QPushButton*)control;
		cb->setDisabled(false);
		control = MQtUtil::findControl("editButton");
		cb = (QPushButton*)control;
		cb->setDisabled(false);
		return;
	}
	else
	{
		MGlobal::displayError(" 0xfded; Nothing checked for export.");

		/*making the buttons clickable again*/
		QWidget * control = MQtUtil::findControl("exportButton");
		QPushButton *cb = (QPushButton*)control;
		cb->setDisabled(false);
		control = MQtUtil::findControl("editButton");
		cb = (QPushButton*)control;
		cb->setDisabled(false);
	}
}

/*Function thats called when the export button is pressed*/
void editClicked()
{
	/*getting the export button from the ui*/

	/*disabling the export button and the editbutton to ensure no "accidental" double clicks*/
	QWidget *control = MQtUtil::findControl("editButton");
	QPushButton *cb = (QPushButton*)control;
	//cb->setDisabled(true);


	QString fileName = QFileDialog::getSaveFileName(cb->topLevelWidget(), "Choose directory", "//DESKTOP-BOKNO6D/server/Assets/bbf files", "*.bbf");

	control = MQtUtil::findControl("lineEdit");
	QLineEdit * lE = (QLineEdit*)control;
	lE->setText(fileName);
}

/*Function thats called when the export button is pressed*/
void exportClicked()
{






	/*getting the export button from the ui*/
	QWidget * control = MQtUtil::findControl("exportButton");
	QPushButton *cb = (QPushButton*)control;

	/*disabling the export button and the editbutton to ensure no "accidental" double clicks*/
	cb->setDisabled(true);
	control = MQtUtil::findControl("editButton");
	cb = (QPushButton*)control;
	cb->setDisabled(true);

	/*getting the rest of the ui variables for export info*/
	control = MQtUtil::findControl("meshBox");
	bool mesh = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("skelBox");
	bool skel = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("matBox");
	bool mats = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("lightBox");
	bool light = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("lineEdit");
	QString fileName = ((QLineEdit*)control)->text();

	/*if there's a file path chosen, the program will start exporting*/
	if (!fileName.isEmpty())
	{
		string fName = "";
		for (int i = 0; i < fileName.size(); ++i)
		{
			fName += fileName[i].unicode();
		}
		exportStart(mesh, skel, mats, light, fName);
		//MGlobal::displayInfo("in export");
	}
	else
	{
		MGlobal::displayError("no file path selected, please click the edit button and choose a file path");
		cb->setDisabled(false);
		control = MQtUtil::findControl("exportButton");
		cb = (QPushButton*)control;
		cb->setDisabled(false);
	}
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
	/*if (!outFile.is_open())
	{
		MGlobal::displayError("ERROR: the binary file is not open");
		
	}*/

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}
	
	MGlobal::displayInfo("Maya plugin loaded!");
    /*Iterate all skin clusters in scene.*/
 //   cSkelAnim.IterateSkinClusters();

 //   /*Iterate all joints in scene.*/
 //   cSkelAnim.IterateJoints();
	//
	///*writing a temporary mainheader for one mesh*/
	//MainHeader tempHead{ 1 };
	//outFile.write((char*)&tempHead, sizeof(MainHeader));

	//MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	//for (; !meshIt.isDone(); meshIt.next())
	//{
	//	MFnTransform trans = meshIt.currentItem();
	//	if (trans.child(0).hasFn(MFn::kMesh))
	//	{
	//		//Createmesh(meshIt.currentItem(), cSkelAnim);
	//		MeshExport newMesh(&outFile, &cSkelAnim.skinList);
	//		newMesh.exportMesh(meshIt.currentItem());
	//	}
	//}
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

	//outFile.close();
	// if any resources have been allocated, release and free here before
	// returning...
	MMessage::removeCallbacks(myCallbackArray);


	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}