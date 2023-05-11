#pragma once

#include <QString>
#include "common_interface_definitions.h"

class top_manager_interface
{
public:
	virtual void GetUnitsInFileList(const QString& fileName, QStringList& unitNames) = 0;
	virtual void GetUnitsInFileIncludeList(const QString& fileName, const QString& includeName, QStringList& unitNames) = 0;
};
