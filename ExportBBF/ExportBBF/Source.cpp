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
#include "ModelExport.h"


using namespace std;

MCallbackIdArray myCallbackArray;

void setProcessBarSize(bool mesh, bool skel, bool mats, bool anims)
{
	QWidget *control = MQtUtil::findControl("progressBar");
	QProgressBar *pBar = (QProgressBar*)control;
	int progressSize = 0;
	if (mesh)
		progressSize += MeshExport::getProgressBarValue();
	if (mats)
		progressSize += 5;
	if (skel && mesh)
		progressSize += 3;
	else if (skel)
		progressSize += 2;
	if (anims)
		progressSize += 1;

	pBar->setMaximum(progressSize);
	pBar->setValue(0);
}

/*function that starts exporting everything chosen*/
void exportStart(bool mesh, bool skel, bool mats, bool anims, bool model, string filePath)
{
	if (mesh || skel || mats || anims)
	{
		MStatus res = MS::kSuccess;
		QWidget *bar = MQtUtil::findControl("progressBar");
		QProgressBar *pBar = (QProgressBar*)bar;
		setProcessBarSize(mesh, skel, mats, anims);

		//fstream outFile;

		/*writing a temporary mainheader*/
		//MainHeader tempHead{ 1 };
		//outFile.write((char*)&tempHead, sizeof(MainHeader));
		//size_t f = filePath.rfind("/", filePath.length());
		//string newPath = filePath.substr(0, f + 1);
		

		SkelAnimExport cSkelAnim(filePath + "/Skeletons/"); //check this <---------------------------------------------

		
		if (skel || anims)
		{
			/*Iterate all skin clusters in scene.*/
			/*cSkelAnim.IterateSkinClusters();
			pBar->setValue(pBar->value() + 1);*/


			MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
			for (; !meshIt.isDone(); meshIt.next())
			{
				MFnTransform trans = meshIt.currentItem();
				if (trans.child(0).hasFn(MFn::kMesh))
				{
					/*SAVING THE MESH NAME FOR THE SKELETON, AS AN IDENTIFIER*/
					cSkelAnim.setMeshName((string)trans.name().asChar());

					if (mesh && skel)
					{

						cSkelAnim.IterateSkinClusters();
						pBar->setValue(pBar->value() + 1);

						MeshExport newMesh((filePath + "/Meshes/" + (string)trans.name().asChar() + ".bbf"), &cSkelAnim.skinList);
						newMesh.exportMesh(meshIt.currentItem());
					}
				}

			}
		}
		else
		{
			if (mesh)
			{
				MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
				for (; !meshIt.isDone(); meshIt.next())
				{
					MFnTransform trans = meshIt.currentItem();
					if (trans.child(0).hasFn(MFn::kMesh))
					{

						//Createmesh(meshIt.currentItem(), cSkelAnim);
						MeshExport newMesh((filePath + "/Meshes/" + (string)trans.name().asChar() + ".bbf"));
						newMesh.exportMesh(meshIt.currentItem());
					}
				}
			}
		}

		if (skel && !anims)
		{
			/*Iterate all joints in scene.*/
			cSkelAnim.IterateJoints();
			pBar->setValue(pBar->value() + 1);

			/*Iterate all animations in the skeleton.*/
			//cSkelAnim.setFilePath(filePath + "/Animations/");
			cSkelAnim.IterateAnimations(anims);
			//pBar->setValue(pBar->value() + 1);

			cSkelAnim.setFilePath(filePath + "/Skeletons/");
			cSkelAnim.writeJointData();
			pBar->setValue(pBar->value() + 1);

		}
		else if (!skel && anims)
		{
			cSkelAnim.setFilePath(filePath + "/Animations/");
			cSkelAnim.IterateAnimations(anims);
			pBar->setValue(pBar->value() + 1);
		}
		else if (skel && anims)
		{
			/*Iterate all joints in scene.*/
			cSkelAnim.IterateJoints();
			pBar->setValue(pBar->value() + 1);

			/*Iterate all animations in the skeleton.*/
			cSkelAnim.setFilePath(filePath + "/Animations/");
			cSkelAnim.IterateAnimations(anims);
			pBar->setValue(pBar->value() + 1);

			cSkelAnim.setFilePath(filePath + "/Skeletons/");
			cSkelAnim.writeJointData();
			pBar->setValue(pBar->value() + 1);
		}
		if (mats)
		{
			MaterialExport newMat(filePath + "/Materials/");
			newMat.MaterialExtraction();

		}
		if (model)
		{

		}
		/*making the buttons clickable again and closing the file*/
		//outFile.close();
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

	QString fileName = QFileDialog::getExistingDirectory(cb->topLevelWidget(), "Choose directory", "//DESKTOP-BOKNO6D/server/Assets/bbf files");

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

	control = MQtUtil::findControl("animBox");
	bool anims = ((QCheckBox*)control)->checkState();

	control = MQtUtil::findControl("modelBox");
	bool model = ((QCheckBox*)control)->checkState();

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
		exportStart(mesh, skel, mats, anims, model, fName);
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
	/*getting the path to the directory*/
	string myDir = MYDIR;
	replace(myDir.begin(), myDir.end(), '\\', '/');

	MString uiDir = "string $uiD = \"";
	uiDir += myDir.c_str();
	uiDir += "mainwindow.ui\"";

	/*initializing the ui*/
	MGlobal::executeCommand(uiDir);
	MGlobal::executeCommand("string $dialog = `loadUI -uiFile $uiD`");
	MGlobal::executeCommand("showWindow $dialog");
	QWidget * control = MQtUtil::findControl("exportButton");
	QPushButton *cb = (QPushButton*)control;
	
	/*connecting the export button to the exportClicked function*/
	QObject::connect(cb, &QPushButton::clicked, [] {exportClicked(); });
	
	control = MQtUtil::findControl("editButton");
	QPushButton *eb = (QPushButton*)control;
	QObject::connect(eb, &QPushButton::clicked, [] {editClicked(); });

	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}
	
	MGlobal::displayInfo("Maya plugin loaded!");
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