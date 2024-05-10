#pragma once

#include <QSharedPointer>
#include "../unit_types.h"
#include "analysis_manager_interface.h"

namespace CubesTop { class ITopManager; }
namespace CubesLog { class ILogManager; }
namespace CubesAnalysis { class FileItemsAnalysis; }
namespace CubesAnalysis { class PropertiesItemsAnalysis; }

namespace CubesAnalysis
{
	class AnalysisManager : public QObject, IAnalysisManager
	{
		Q_OBJECT

	private:
		CubesTop::ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		QSharedPointer<FileItemsAnalysis> fileItemsAnalysis_;
		QSharedPointer<PropertiesItemsAnalysis> propertiesItemsAnalysis_;

	public:
		AnalysisManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager);

	public:
		void Test();

	signals:
		void Error(const CubesUnitTypes::PropertiesId propertiesId, const QString& message);

	public:
		// IAnalysisManager
		void AfterFileError(const CubesUnitTypes::FileId fileId, const QString& message) override;
		void AfterPropertiesError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message) override;
	};
}
