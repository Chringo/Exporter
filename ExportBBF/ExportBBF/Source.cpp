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

//void exportClicked()
//{
//	MGlobal::displayInfo("juj");
//}

/*class to recieve signals from the qt ui*/
//class SignalWrapper : public QObject
//{
//	Q_OBJECT
//public:
//	SignalWrapper(QWidget * parent = 0);
//		
//
//		public slots:
//	void test() { MGlobal::displayInfo("hejsan"); }
//
//
//};
//SignalWrapper::SignalWrapper(QWidget * parent)
//{
//	QObject::connect(parent, SIGNAL(released()), this, SLOT(test()));
//}
//class SignalWrapper : public QMainWindow
//{
//	//Q_OBJECT;
//public:
//	explicit SignalWrapper(QWidget *parent, QPushButton* button);
//	
//	public slots:
//	void handleButton();
//private:
//	QPushButton *m_button;
//};
//SignalWrapper::SignalWrapper(QWidget *parent, QPushButton* button)
//	: QMainWindow(parent)
//{
//	// Create the button, make "this" the parent
//	m_button = button;
//	// set size and location of the button
//	//m_button->setGeometry(QRect(QPoint(100, 100),
//	//	QSize(200, 50)));
//
//	// Connect button signal to appropriate slot
//	//connect(button, SIGNAL(clicked()), this, SLOT(handleButton()));
//}
//void SignalWrapper::handleButton()
//{
//	MGlobal::displayInfo("hejsan");
//}
class leif : public QObject
{
	//Q_OBJECT

	public slots:
	void allan();

};
class SignalWrapper : public QObject
{
	//Q_OBJECT
public:
	SignalWrapper(QPushButton * sender, QWidget * parent = 0);
			

		public slots:
	void test();

};
void SignalWrapper::test()
{
	MGlobal::displayInfo("hejsan");
}
SignalWrapper::SignalWrapper(QPushButton * sender, QWidget * parent)
	: QObject(parent)
{
	QObject::connect(sender, SIGNAL(clicked()), this, SLOT(test()));
}
const char* gemig()
{
	MGlobal::displayInfo("kuk");
	const char *hej = nullptr;
	return hej;
}
void exportClicked()
{
	MGlobal::displayInfo("kuk");
}

QPushButton* cb;

EXPORT MStatus initializePlugin(MObject obj)
{
    SkelAnimExport cSkelAnim;
	MGlobal::executeCommand("string $dialog = `loadUI - uiFile ""mainwindow.ui""`");
	MGlobal::executeCommand("showWindow $dialog");
	QWidget * control = MQtUtil::findControl("exportButton");
	QWidget * box = MQtUtil::findControl("skelBox");
	QCheckBox* sb = (QCheckBox*)box;
	cb = (QPushButton*)control;

	QWidget * ex = cb->topLevelWidget();
	QString helll = ex->whatsThis();
	
	MGlobal::displayInfo("hejsan nu är det nu");
	QObject::connect(cb, &QPushButton::clicked, [] {exportClicked(); });
	//sb->setChecked(true);

	//SignalWrapper tw(cb);
	//QtPrivate::FunctionPointer<
	//QObject::connect(cb, (const void*)cb->clicked(), gemig());
	
	
	
	//cb->connect(cb, SIGNAL(cb->released()), cb->topLevelWidget(), SLOT(exportClicked()));
	//cb->clicked.connect(SLOT(exportClicked()));
	//cb->isChecked();
	//MString checkName;
	//checkName = cb->objectName().toStdString().c_str();
	//checkName += hej.c_str();
	/*if (cb->isChecked())
		MGlobal::displayInfo("is clicked");
	else
		MGlobal::displayInfo("is not clicked");*/
	//MString hejsan = MGlobal::executeCommandStringResult("int $status = `checkBox -q -v skelBox`");
	//MGlobal::executeCommand("int $status = `checkBox -q -v skelBox`");
	//MString hejsan = MGlobal::executeCommandStringResult("$status");

	//MGlobal::displayInfo(hejsan);

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

	//QObject::disconnect(cb, [] {exportClicked(); });


	outFile.close();
	// if any resources have been allocated, release and free here before
	// returning...
	MMessage::removeCallbacks(myCallbackArray);


	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}