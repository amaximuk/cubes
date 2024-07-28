#pragma once

#include <QSharedPointer>
#include "../unit/unit_parameter_model.h"

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
		CubesLog::ILogManager* logManager_;
		QSharedPointer<FileItemsAnalysis> fileItemsAnalysis_;
		QSharedPointer<PropertiesItemsAnalysis> propertiesItemsAnalysis_;

	public:
		AnalysisManager(CubesLog::ILogManager* logManager);

	public:
		void Test(CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs,
			CubesUnit::PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
			CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr);
	};
}
