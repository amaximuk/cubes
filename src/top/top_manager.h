#pragma once

#include <QObject>
#include <QPointer>
#include "../log/log_manager_interface.h"
#include "../unit/unit_parameter_model_ids.h"
#include "top_manager_interface.h"

namespace CubesUnit { class UnitParameters; }
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

        CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr_;
        QPointer<CubesFile::FileItemsManager> fileItemsManager_;
        QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;
        QPointer<CubesAnalysis::AnalysisManager> analysisManager_;

        // Значения имен параметров
        CubesUnit::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        explicit TopManager(bool isArray, bool isMock = false, QString path = "");
        ~TopManager() override;

    public:
        // ITopManager
        virtual bool GetUnitsInFileList(CubesUnit::FileId fileId, QStringList& unitNames) override;
        virtual bool GetUnitsInFileIncludeList(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
            QStringList& unitNames) override;
        virtual bool GetUnitParametersPtr(const QString& unitId, CubesUnit::UnitParametersPtr& unitParametersPtr) override;
        virtual bool GetFileIncludeList(CubesUnit::FileId fileId, CubesUnit::IncludeIdNames& includeNames) override;
        virtual bool GetFileIncludeVariableList(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
            CubesUnit::VariableIdVariables& variables) override;
        virtual bool CreatePropetiesItem(const QString& unitId, CubesUnit::PropertiesId& propertiesId) override;
        virtual bool GetPropetiesForDrawing(CubesUnit::PropertiesId propertiesId, CubesDiagram::PropertiesForDrawing& pfd) override;
        virtual bool GetPropetiesUnitParameters(CubesUnit::PropertiesId propertiesId,
            CubesUnit::UnitParametersPtr& unitParametersPtr) override;
        virtual bool GetPropetiesUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId) override;
        virtual bool GetPropetiesXmlUnit(CubesUnit::PropertiesId propertiesId, CubesXml::Unit& xmlUnit) override;
        virtual bool GetUnitsConnections(QMap<QString, QStringList>& connections) override;
        virtual bool GetDependsConnections(QMap<QString, QStringList>& connections) override;
        virtual bool CreateDiagramItem(CubesUnit::PropertiesId propertiesId) override;
        virtual bool EnshureVisible(CubesUnit::PropertiesId propertiesId) override;
        virtual bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
        virtual bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;
        virtual bool AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds) override;
        virtual bool UnitsContextMenuRequested(QPoint globalPosition, QList<CubesUnit::PropertiesId>& propertiesIds) override;
        virtual bool GetVisibleSceneRect(QRectF& rect) override;

        // ILogManager
        virtual void AddMessage(const CubesLog::Message& m) override;

    private:
        // Заполнение параметров юнитов
        void FillParametersInfo(const QString& parametersPath, bool isMock);

    protected:
        // Добавление юнитов
        virtual bool AddMainFile(const CubesXml::File& file, const QString& zipFileName);
        virtual bool AddUnits(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
            const QList<CubesXml::Group>& groups, QList<CubesUnit::PropertiesId>& addedPropertiesIds);
        
        // Получение информации
        virtual CubesUnit::UnitParametersPtr GetUnitParametersPtr(const QString& id);
        virtual CubesUnit::FileIdNames GetFileNames();
        virtual CubesUnit::IncludeIdNames GetCurrentFileIncludeNames();

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
        virtual void FileNameChanged(CubesUnit::FileId fileId);
        virtual void FileListChanged(const CubesUnit::FileIdNames& fileIdNames);
        virtual void FileIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId);
        virtual void FileIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames);
        virtual void FileVariableNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
            const QString& variableName, const QString& oldVariableName);
        virtual void FileVariablesListChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
            const CubesUnit::VariableIdVariables& variables);
        virtual void FileColorChanged(CubesUnit::FileId fileId, const QColor& color);
        virtual void FilePropertiesChanged();

        // PropertiesItemsManager
        virtual void PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
            CubesUnit::FileId fileId, CubesUnit::IncludeId includeId);
        virtual void PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId);
        virtual void PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ);
        virtual void PropertiesError(CubesUnit::PropertiesId propertiesId, const QString& message);
        virtual void PropertiesConnectionChanged(CubesUnit::PropertiesId propertiesId);
        virtual void PropertiesPropertiesChanged();
    };
}
