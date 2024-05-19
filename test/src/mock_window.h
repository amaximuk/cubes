#pragma once

#include "top/top_manager.h"

namespace CubesTop { class ITopManager; }
namespace CubesUnitTypes { class UnitParameters; }
namespace CubesLog { class LogTableModel; }
namespace CubesLog { class SortFilterModel; }
namespace CubesFile { class FileItemsManager; }
namespace CubesProperties { class PropertiesItemsManager; }
namespace CubesAnalysis { class AnalysisManager; }

class MockWindow : public CubesTop::TopManager
{
private:
    bool modified_;
    QString path_;

public:
    explicit MockWindow(QWidget* parent = nullptr);
    ~MockWindow() override;

public:
    // ILogManager
    void AddMessage(const CubesLog::LogMessage& m) override;

protected:
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
};
