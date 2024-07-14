#pragma once

#include <QObject>
#include <QPointer>
#include "../log/log_manager_interface.h"
#include "top_manager_interface.h"

namespace CubesUnitTypes { class UnitParameters; }
namespace CubesLog { class LogTableModel; }
namespace CubesLog { class LogHelper; }
namespace CubesLog { class SortFilterModel; }
namespace CubesFile { class FileItemsManager; }
namespace CubesProperties { class PropertiesItemsManager; }
namespace CubesAnalysis { class AnalysisManager; }

namespace CubesTop
{
    class TopManager : public ITopManager, public CubesLog::ILogManager
    {
    protected:
        uint32_t uniqueNumber_;

        CubesUnitTypes::UnitIdUnitParameters unitParameters_;
        QPointer<CubesFile::FileItemsManager> fileItemsManager_;
        QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;
        QPointer<CubesAnalysis::AnalysisManager> analysisManager_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        explicit TopManager(bool isArray, bool isMock = false, QString path = "");
        ~TopManager() override;

    public:
        // ITopManager
        virtual bool GetUnitsInFileList(CubesUnitTypes::FileId fileId, QStringList& unitNames) override;
        virtual bool GetUnitsInFileIncludeList(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            QStringList& unitNames) override;
        virtual bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
        virtual bool GetFileIncludeList(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeIdNames& includeNames) override;
        virtual bool GetFileIncludeVariableList(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            CubesUnitTypes::VariableIdVariables& variables) override;
        virtual bool CreatePropetiesItem(const QString& unitId, CubesUnitTypes::PropertiesId& propertiesId) override;
        virtual bool GetPropetiesForDrawing(CubesUnitTypes::PropertiesId propertiesId, PropertiesForDrawing& pfd) override;
        virtual bool GetPropetiesUnitParameters(CubesUnitTypes::PropertiesId propertiesId,
            CubesUnitTypes::UnitParameters& unitParameters) override;
        virtual bool GetPropetiesUnitId(CubesUnitTypes::PropertiesId propertiesId, QString& unitId) override;
        virtual bool GetUnitsConnections(QMap<QString, QStringList>& connections) override;
        virtual bool GetDependsConnections(QMap<QString, QStringList>& connections) override;
        virtual bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId) override;
        virtual bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;
        virtual bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
        virtual bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;

        // ILogManager
        virtual void AddMessage(const CubesLog::Message& m) override;

    private:
        // Заполнение параметров юнитов
        void FillParametersInfo(const QString& parametersPath, bool isMock);

    protected:
        // Добавление юнитов
        virtual bool AddMainFile(const CubesXml::File& file, const QString& zipFileName);
        virtual bool AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file);
        
        // Получение информации
        virtual CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id);
        virtual CubesUnitTypes::FileIdNames GetFileNames();
        virtual CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames();

        // External
        virtual bool NewFile();
        virtual bool SaveFile(const QString& path);
        virtual bool SaveFolder(const QString& path);
        virtual bool OpenFile(const QString& path);
        virtual bool OpenFolder(const QString& path);
        virtual bool ImportXml(const QString& path);
        virtual bool SortUnitsBoost();
        virtual bool SortUnitsRectangular(bool check);
        virtual bool Test();

    protected:
        // FileItemsManager
        virtual void FileNameChanged(CubesUnitTypes::FileId fileId);
        virtual void FileListChanged(const CubesUnitTypes::FileIdNames& fileIdNames);
        virtual void FileIncludeNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
        virtual void FileIncludesListChanged(CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
        virtual void FileVariableNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            const QString& variableName, const QString& oldVariableName);
        virtual void FileVariablesListChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            const CubesUnitTypes::VariableIdVariables& variables);
        virtual void FileColorChanged(CubesUnitTypes::FileId fileId, const QColor& color);
        virtual void FilePropertiesChanged();

        // PropertiesItemsManager
        virtual void PropertiesBasePropertiesChanged(CubesUnitTypes::PropertiesId propertiesId, const QString& name,
            CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
        virtual void PropertiesSelectedItemChanged(CubesUnitTypes::PropertiesId propertiesId);
        virtual void PropertiesPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ);
        virtual void PropertiesError(CubesUnitTypes::PropertiesId propertiesId, const QString& message);
        virtual void PropertiesConnectionChanged(CubesUnitTypes::PropertiesId propertiesId);
        virtual void PropertiesPropertiesChanged();
    };
}
