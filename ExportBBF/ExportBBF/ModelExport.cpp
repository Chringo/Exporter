#include "ModelExport.h"



ModelExport::ModelExport()
{
}


ModelExport::~ModelExport()
{
}

ModelExport::ModelExport(std::string & filePath)
{
	this->m_filePath = filePath;
}

void ModelExport::setUID(std::string & name)
{
	this->m_UID = (unsigned int)std::hash<std::string>{}(name);
}
