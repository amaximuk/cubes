#pragma once

#include <QObject>
#include <QPointer>
#include "../log_table/log_table_interface.h"
#include "top_manager_interface.h"

namespace CubesUnitTypes { class UnitParameters; }
namespace CubesLog { class LogTableModel; }
namespace CubesLog { class SortFilterModel; }
namespace CubesFile { class FileItemsManager; }
namespace CubesProperties { class PropertiesItemsManager; }
namespace CubesAnalysis { class AnalysisManager; }

namespace CubesTop
{
    class TopManager : public ITopManager, public CubesLog::ILogManager
    {
    protected:
        bool modified_;
        QString path_;
        uint32_t uniqueNumber_;

        QMap<QString, CubesUnitTypes::UnitParameters> unitParameters_;
        QPointer<CubesFile::FileItemsManager> fileItemsManager_;
        QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;
        QPointer<CubesAnalysis::AnalysisManager> analysisManager_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

    public:
        explicit TopManager(bool isArray);
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
        virtual void AddMessage(const CubesLog::LogMessage& m) override;

    protected:
        virtual void FillParametersInfo();

        // Units
        virtual bool AddMainFile(const CubesXml::File& file, const QString& zipFileName);
        virtual bool AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file);
        virtual bool SortUnits();
        virtual bool SortUnitsRectangular(bool check);
        virtual CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id);
        virtual bool Test();

        // Files
        virtual CubesUnitTypes::FileIdNames GetFileNames();
        virtual CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames();

        // Modified
        virtual void UpdateFileState(const QString& path, bool modified);
        virtual bool SaveFileInternal(const QString& path);
        virtual bool SaveFolderInternal(const QString& path);
        virtual bool OpenFileInternal(const QString& path);
        virtual bool OpenFolderInternal(const QString& path);

    public:
        //// DiagramScene (as manager)
        //void DiagramItemPositionChanged(CubesDiagram::DiagramItem* item);
        //void DiagramAfterItemCreated(CubesDiagram::DiagramItem* item);
        //void DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* item);
        ////void DiagramItemNameChanged(CubeDiagram::DiagramItem* item, QString oldName);
        ////void DiagramItemFileChanged(CubeDiagram::DiagramItem* item);
        ////void DiagramItemGroupChanged(CubeDiagram::DiagramItem* item);
        //void selectionChanged(); // QGraphicsScene

        // FileItemsManager
        void FileNameChanged(CubesUnitTypes::FileId fileId);
        void FileListChanged(const CubesUnitTypes::FileIdNames& fileIdNames);
        void FileIncludeNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
        void FileIncludesListChanged(CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
        void FileVariableNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            const QString& variableName, const QString& oldVariableName);
        void FileVariablesListChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
            const CubesUnitTypes::VariableIdVariables& variables);
        void FileColorChanged(CubesUnitTypes::FileId fileId, const QColor& color);
        void FilePropertiesChanged();

        // PropertiesItemsManager
        void PropertiesBasePropertiesChanged(CubesUnitTypes::PropertiesId propertiesId, const QString& name,
            CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
        void PropertiesSelectedItemChanged(CubesUnitTypes::PropertiesId propertiesId);
        void PropertiesPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ);
        void PropertiesError(CubesUnitTypes::PropertiesId propertiesId, const QString& message);
        void PropertiesConnectionChanged(CubesUnitTypes::PropertiesId propertiesId);
        void PropertiesPropertiesChanged();

    protected:
        // Кнопки
        void OnNewFileAction();
        void OnOpenFileAction();
        void OnOpenFolderAction();
        void OnImportXmlFileAction();
        void OnSaveFileAction();
        void OnSaveAsFileAction();
        void OnSaveFolderAction();
        //void OnQuitAction();
        void OnSortBoostAction();
        void OnSortRectAction();
        //void OnRecentAction();
        void OnTestAction();

        // Лог
        //void OnErrorButtonClicked(bool checked);
        //void OnWarningButtonClicked(bool checked);
        //void OnInformationButtonClicked(bool checked);
        //void OnDoubleClicked(const QModelIndex& index);
    };
}
