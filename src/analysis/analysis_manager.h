#pragma once

#include <QSharedPointer>
#include "../unit_types.h"
#include "analysis_manager_interface.h"

class ITopManager;
namespace CubesLog
{
	class ILogManager;
}
namespace CubesAnalysis
{
	class FileItemsAnalysis;
}

namespace CubesAnalysis
{
	class AnalysisManager : public QObject, IAnalysisManager
	{
		Q_OBJECT

	private:
		ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		QSharedPointer<FileItemsAnalysis> fileItemAnalysis_;

	public:
		AnalysisManager(ITopManager* topManager, CubesLog::ILogManager* logManager);

	signals:
		void Error(const CubesUnitTypes::PropertiesId propertiesId, const QString& message);

	public:
		// IFileItemsManager
		void Test() override;
	};
}
