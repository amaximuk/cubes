#pragma once

#include "file_item_types.h"

namespace CubesFile
{
	using FileNameChangedDelegate = std::function<void(CubesUnitTypes::FileId)>;
	using FilesListChangedDelegate = std::function<void(const CubesUnitTypes::FileIdNames&)>;
	using IncludeNameChangedDelegate = std::function<void(CubesUnitTypes::FileId, CubesUnitTypes::IncludeId)>;
	using IncludesListChangedDelegate = std::function<void(CubesUnitTypes::FileId, const CubesUnitTypes::IncludeIdNames&)>;
	using VariableNameChangedDelegate = std::function<void(CubesUnitTypes::FileId, CubesUnitTypes::IncludeId, const QString&, const QString&)>;
	using VariablesListChangedDelegate = std::function<void(CubesUnitTypes::FileId, CubesUnitTypes::IncludeId, const CubesUnitTypes::VariableIdVariables&)>;
	using ColorChangedDelegate = std::function<void(CubesUnitTypes::FileId, const QColor&)>;
	using PropertiesChangedDelegate = std::function<void(void)>;

	class IFileItemsManager
	{
	public:
		virtual ~IFileItemsManager() = default;

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
}
