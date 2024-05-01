#pragma once

#include "file_item_types.h"

namespace CubesFile
{
	class IFileItemsManagerBoss
	{
	public:
		virtual ~IFileItemsManagerBoss() = default;

	public:
		// “ут значени€ мен€ютс€ по одному
		//virtual void BeforeFileNameChanged(const CubesUnitTypes::FileId fileId,
		//	const CubesUnitTypes::FileId oldFileId, bool& cancel) = 0;
		virtual void AfterFileNameChanged(const CubesUnitTypes::FileId fileId) = 0;
		//virtual void BeforeIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId,
		//	const CubesUnitTypes::IncludeFileId oldIncludeId, bool& cancel) = 0;
		virtual void AfterIncludeNameChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeId includeId) = 0;

		// “ут могут мен€тьс€ значени€ не по одному, а сразу список, поэтому передаем сразу все имена
		virtual void BeforeIncludesAdd(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel) = 0;
		virtual void BeforeIncludesRemoved(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel) = 0;
		virtual void AfterIncludesListChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeIdNames& includeNames) = 0;
		
		virtual void AfterVariableNameChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeId includeId, const QString& variableName,
			const QString& oldVariableName) = 0;
		virtual void AfterVariablesListChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeId includeId,
			const CubesUnitTypes::VariableIdVariables& variables) = 0;
	
		virtual void AfterColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color) = 0;
		virtual void AfterPropertiesChanged() = 0;
	};

	class IFileItemsManagerWorker
	{
	public:
		virtual ~IFileItemsManagerWorker() = default;
	};
}
