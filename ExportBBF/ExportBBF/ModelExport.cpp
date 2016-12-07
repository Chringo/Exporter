#include "ModelExport.h"



ModelExport::ModelExport()
{
}


ModelExport::~ModelExport()
{
}

bool bbfExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

ModelExport::ModelExport(std::string & filePath)
{
	this->m_filePath = filePath;
}

void ModelExport::setUID(std::string & name)
{
	this->m_UID = (unsigned int)std::hash<std::string>{}(name);
}

void ModelExport::exportModel()
{
	if (bbfExists(m_filePath))
	{
		size_t f = m_filePath.rfind("/", m_filePath.length());
		std::string pAth = m_filePath.substr(f + 1, m_filePath.length());

		std::string meshName = ("Overwrite " + pAth + "?");
		std::wstring stemp = std::wstring(meshName.begin(), meshName.end());
		LPCWSTR sw = stemp.c_str();
		if (MessageBox(NULL, sw, TEXT("bbf file already exists"), MB_YESNO) == IDYES)
		{
			std::fstream outFile(m_filePath, std::fstream::out | std::fstream::binary);

			MainHeader s_Head;
			s_Head.type = (int)Resources::ResourceType::RES_MODEL;
			s_Head.id = (unsigned int)std::hash<std::string>{}(m_filePath);
			m_UID = s_Head.id;

			outFile.write((char*)&s_Head, sizeof(MainHeader));
		}
	}
	else
	{
		std::fstream outFile(m_filePath, std::fstream::out | std::fstream::binary);

		MainHeader s_Head;
		s_Head.type = (int)Resources::ResourceType::RES_MODEL;
		s_Head.id = (unsigned int)std::hash<std::string>{}(m_filePath);
		m_UID = s_Head.id;

		outFile.write((char*)&s_Head, sizeof(MainHeader));
	}
}
