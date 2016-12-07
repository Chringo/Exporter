#ifndef EXPORTBBF_MODELEXPORT_MODELEXPORT
#define EXPORTBBF_MODELEXPORT_MODELEXPORT
#include "HeaderStructs.h"

class ModelExport
{
private:
	std::string m_filePath;
	unsigned int m_UID = 0;

	modelHeader m_Model;

public:
	ModelExport();
	~ModelExport();

	ModelExport(std::string &filePath);
	void setUID(std::string &name);

	void setMeshId(unsigned int UID) { m_Model.meshId = UID; }
	void setMatId(unsigned int UID) { m_Model.matId = UID; }
	void setSkelId(unsigned int UID) { m_Model.skelId = UID; }
};

#endif // !EXPORTBBF_MODELEXPORT_MODELEXPORT
