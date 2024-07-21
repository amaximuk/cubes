#pragma once

#include "unit/unit_parameter_model.h"
#include "top/top_manager.h"

namespace CubesTop { class ITopManager; }
namespace CubesUnit { class UnitParameters; }
namespace CubesLog { class LogTableModel; }
namespace CubesLog { class SortFilterModel; }
namespace CubesFile { class FileItemsManager; }
namespace CubesFile { class FileItem; }
namespace CubesProperties { class PropertiesItemsManager; }
namespace CubesAnalysis { class AnalysisManager; }

class MockWindow : private CubesTop::TopManager
{
private:
    bool modified_;
    QString path_;
    QVector<CubesLog::Message> messages_;

public:
    explicit MockWindow();
    explicit MockWindow(const QString& parametersPath);
    ~MockWindow() override;

protected:
    // ILogManager
    void AddMessage(const CubesLog::Message& m) override;

public:
    // Units
    bool SortUnitsBoost() override;
    bool SortUnitsRectangular(bool check) override;

    // External
    bool NewFile() override;
    bool SaveFile(const QString& path) override;
    bool SaveFolder(const QString& path) override;
    bool OpenFile(const QString& path) override;
    bool OpenFolder(const QString& path) override;
    bool ImportXml(const QString& path) override;
    bool Test() override;

    // Debug
    QMap<CubesUnit::FileId, QSharedPointer<CubesFile::FileItem>> GetFileItems();
    CubesUnit::FileIdParameterModelsRef GetFileIdParameterModelsRef();

    // Message
    QVector<CubesLog::Message> GetMessages();
    void ClearMessages();
};
