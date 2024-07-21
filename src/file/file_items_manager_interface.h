#pragma once

#include "file_item_types.h"

namespace CubesFile
{
	using FileNameChangedDelegate = std::function<void(CubesUnit::FileId)>;
	using FilesListChangedDelegate = std::function<void(const CubesUnit::FileIdNames&)>;
	using IncludeNameChangedDelegate = std::function<void(CubesUnit::FileId, CubesUnit::IncludeId)>;
	using IncludesListChangedDelegate = std::function<void(CubesUnit::FileId, const CubesUnit::IncludeIdNames&)>;
	using VariableNameChangedDelegate = std::function<void(CubesUnit::FileId, CubesUnit::IncludeId, const QString&, const QString&)>;
	using VariablesListChangedDelegate = std::function<void(CubesUnit::FileId, CubesUnit::IncludeId, const CubesUnit::VariableIdVariables&)>;
	using ColorChangedDelegate = std::function<void(CubesUnit::FileId, const QColor&)>;
	using PropertiesChangedDelegate = std::function<void(void)>;

	class IFileItemsManager
	{
	public:
		virtual ~IFileItemsManager() = default;

	public:
		// “ут значени€ мен€ютс€ по одному
		//virtual void BeforeFileNameChanged(const CubesUnit::FileId fileId,
		//	const CubesUnit::FileId oldFileId, bool& cancel) = 0;
		virtual void AfterFileNameChanged(const CubesUnit::FileId fileId) = 0;
		//virtual void BeforeIncludeNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeFileId includeId,
		//	const CubesUnit::IncludeFileId oldIncludeId, bool& cancel) = 0;
		virtual void AfterIncludeNameChanged(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeId includeId) = 0;

		// “ут могут мен€тьс€ значени€ не по одному, а сразу список, поэтому передаем сразу все имена
		virtual void BeforeIncludesAdd(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeIdNames& includeNames, bool& cancel) = 0;
		virtual void BeforeIncludesRemoved(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeIdNames& includeNames, bool& cancel) = 0;
		virtual void AfterIncludesListChanged(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeIdNames& includeNames) = 0;
		
		virtual void AfterVariableNameChanged(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeId includeId, const QString& variableName,
			const QString& oldVariableName) = 0;
		virtual void AfterVariablesListChanged(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeId includeId,
			const CubesUnit::VariableIdVariables& variables) = 0;
	
		virtual void AfterColorChanged(const CubesUnit::FileId fileId, const QColor& color) = 0;
		virtual void AfterPropertiesChanged() = 0;
	};
}
