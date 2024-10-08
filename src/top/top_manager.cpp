#include "parameters/yaml_parser.h"
#include "../file/file_item.h"
#include "../file/file_items_manager.h"
#include "../properties/properties_item.h"
#include "../properties/properties_items_manager.h"
#include "../analysis/analysis_manager.h"
#include "../xml/xml_parser.h"
#include "../xml/xml_writer.h"
#include "../xml/xml_helper.h"
#include "../utils/graph.h"
#include "../utils/zip.h"
#include "../unit/unit_types.h"
#include "../unit/unit_helper.h"
#include "../log/log_helper.h"
#include "top_manager.h"

using namespace CubesTop;

TopManager::TopManager(bool isArray, bool isMock, QString path)
{
    uniqueNumber_ = 0;

    using namespace std::placeholders;

    fileItemsManager_ = new CubesFile::FileItemsManager(this, this, isMock);
    fileItemsManager_->SetFileNameChangedDelegate(std::bind<void>(&TopManager::FileNameChanged, this, _1));
    fileItemsManager_->SetFilesListChangedDelegate(std::bind<void>(&TopManager::FileListChanged, this, _1));
    fileItemsManager_->SetIncludeNameChangedDelegate(std::bind<void>(&TopManager::FileIncludeNameChanged, this, _1, _2));
    fileItemsManager_->SetIncludesListChangedDelegate(std::bind<void>(&TopManager::FileIncludesListChanged, this, _1, _2));
    fileItemsManager_->SetVariableNameChangedDelegate(std::bind<void>(&TopManager::FileVariableNameChanged, this, _1, _2, _3, _4));
    fileItemsManager_->SetVariablesListChangedDelegate(std::bind<void>(&TopManager::FileVariablesListChanged, this, _1, _2, _3));
    fileItemsManager_->SetColorChangedDelegate(std::bind<void>(&TopManager::FileColorChanged, this, _1, _2));
    fileItemsManager_->SetPropertiesChangedDelegate(std::bind<void>(&TopManager::FilePropertiesChanged, this));

    propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, this, isArray, isMock);
    propertiesItemsManager_->SetBasePropertiesChangedDelegate(std::bind<void>(&TopManager::PropertiesBasePropertiesChanged, this, _1, _2, _3, _4));
    propertiesItemsManager_->SetPositionChangedDelegate(std::bind<void>(&TopManager::PropertiesPositionChanged, this, _1, _2, _3, _4));
    propertiesItemsManager_->SetSizeChangedDelegate(std::bind<void>(&TopManager::PropertiesSizeChanged, this, _1, _2));
    propertiesItemsManager_->SetTextChangedDelegate(std::bind<void>(&TopManager::PropertiesTextChanged, this, _1, _2, _3, _4, _5, _6));
    propertiesItemsManager_->SetSelectedItemChangedDelegate(std::bind<void>(&TopManager::PropertiesSelectedItemChanged, this, _1));
    propertiesItemsManager_->SetConnectionChangedDelegate(std::bind<void>(&TopManager::PropertiesConnectionChanged, this, _1));
    propertiesItemsManager_->SetPropertiesChangedDelegate(std::bind<void>(&TopManager::PropertiesPropertiesChanged, this));
    
    analysisManager_ = new CubesAnalysis::AnalysisManager(this);

    logHelper_.reset(new CubesLog::LogHelper(this, CubesLog::SourceType::topManager,
        GetTopManagerErrorDescriptions()));

    if (!isArray)
        FillParametersInfo(path, isMock);
}

TopManager::~TopManager()
{
}

// ITopManager
bool TopManager::GetUnitsInFileList(CubesUnit::FileId fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool TopManager::GetUnitsInFileIncludeList(CubesUnit::FileId fileId,
    CubesUnit::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool TopManager::GetUnitParametersPtr(const QString& unitId, CubesUnit::UnitParametersPtr& unitParametersPtr)
{
    // ��� �������� - �� �� ����� ��������, �� � ��� ����� ���� ������� � ������
    if (unitId.isEmpty() && unitIdUnitParametersPtr_.size() == 1)
    {
        unitParametersPtr = *unitIdUnitParametersPtr_.begin();
    }
    else
    {
        const auto it = unitIdUnitParametersPtr_.find(unitId);
        if (it == unitIdUnitParametersPtr_.end())
            return false;
        unitParametersPtr = *it;
    }
    
    return true;
}

bool TopManager::GetFileIncludeList(CubesUnit::FileId fileId, CubesUnit::IncludeIdNames& includeNames)
{
    return fileItemsManager_->GetFileIncludeNames(fileId, true, includeNames);
}

bool TopManager::GetFileIncludeVariableList(CubesUnit::FileId fileId,
    CubesUnit::IncludeId includeId, CubesUnit::VariableIdVariables& variables)
{
    return fileItemsManager_->GetFileIncludeVariables(fileId, includeId, variables);
}

bool TopManager::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    propertiesItemsManager_->Create(unitId, propertiesId);
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    pi->SetFileIdNames(GetFileNames());
    pi->SetFileIdName(fileItemsManager_->GetCurrentFileId(), fileItemsManager_->GetCurrentFileName());
    pi->SetIncludeIdNames(GetCurrentFileIncludeNames());
    pi->SetIncludeIdName(CubesUnit::InvalidIncludeId, "<not selected>");
    return true;
}

bool TopManager::GetPropetiesForDrawing(CubesUnit::PropertiesId propertiesId, CubesDiagram::PropertiesForDrawing& pfd)
{
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    if (pi == nullptr)
        return false;

    pfd.pixmap = pi->GetPixmap();
    QString name;
    if (!propertiesItemsManager_->GetName(propertiesId, name))
        return false;
    pfd.name = name;
    pfd.fileName = pi->GetFileName();
    pfd.includeName = pi->GetIncludeName();
    pfd.color = fileItemsManager_->GetFileColor(pi->GetFileId());
    pfd.itemType = pi->GetItemType();

    if (pfd.itemType == CubesDiagram::ItemType::Text)
    {
        pfd.text = pi->GetText();
        pfd.size = pi->GetSize();
    }

    return true;
}

bool TopManager::GetPropetiesUnitParameters(CubesUnit::PropertiesId propertiesId, CubesUnit::UnitParametersPtr& unitParametersPtr)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParametersPtr);
}

bool TopManager::GetPropetiesUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

bool TopManager::GetPropetiesXmlUnit(CubesUnit::PropertiesId propertiesId, CubesXml::Unit& xmlUnit)
{
    return propertiesItemsManager_->GetXmlUnit(propertiesId, xmlUnit);
}

bool TopManager::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetUnitsConnections(connections);
}

bool TopManager::GetDependsConnections(QMap<QString, QStringList>& connections)
{
    const auto fileIdParameterModelPtrs = fileItemsManager_->GetFileIdParameterModelPtrs();
    const auto propertiesIdParameterModelPtrs = propertiesItemsManager_->GetPropertiesIdParameterModelPtrs();

    for (auto& kvp : propertiesIdParameterModelPtrs.toStdMap())
    {
        const auto depends = CubesUnit::Helper::Analyse::GetUnitDependencies(kvp.second,
            fileIdParameterModelPtrs, unitIdUnitParametersPtr_);

        if (!depends.empty())
        {
            const auto name = CubesUnit::Helper::Analyse::GetResolvedUnitName(kvp.second, fileIdParameterModelPtrs);
            auto& connects = connections[name.resolved];
            for (const auto& depend_on : depends)
            {
                if (!connects.contains(depend_on.name.resolved))
                    connects.push_back(depend_on.name.resolved);
            }
        }
    }

    return true;
}

bool TopManager::CreateDiagramItem(CubesUnit::PropertiesId propertiesId)
{
    return true;
}

bool TopManager::EnshureVisible(CubesUnit::PropertiesId propertiesId)
{
    return true;
}

bool TopManager::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
    return fileItemsManager_->GetAnalysisFiles(files);
}

bool TopManager::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
    return propertiesItemsManager_->GetAnalysisProperties(properties);
}

bool TopManager::AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds)
{
    const auto fileId = fileItemsManager_->GetCurrentFileId();
    QList<CubesXml::Group> groups;
    CubesXml::Group group{};
    for (const auto& unit : units)
        group.units.push_back(unit);
    groups.push_back(group);
    AddUnits(fileId, CubesUnit::InvalidFileId, groups, addedPropertiesIds);
    return true;
}

bool TopManager::UnitsContextMenuRequested(QPoint globalPosition, QList<CubesUnit::PropertiesId>& propertiesIds)
{
    return true;
}

bool TopManager::GetVisibleSceneRect(QRectF& rect)
{
    return true;
}

// ILogManager
void TopManager::AddMessage(const CubesLog::Message& m)
{
}

void TopManager::FillParametersInfo(const QString& parametersPath, bool isMock)
{
    QString path = parametersPath == "" && !isMock ? QCoreApplication::applicationDirPath() + "/doc/all_units_solid" :
        parametersPath == "" ? "units" : parametersPath;

    QString directoryPath(path);
    QStringList platformDirs;
    QDirIterator directories(directoryPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while (directories.hasNext()) {
        directories.next();
        platformDirs << directories.filePath();
    }

    for (const auto& platformDir : platformDirs)
    {
        QString ymlPath = QDir(platformDir).filePath("yml");
        if (QFileInfo(ymlPath).exists() && QFileInfo(ymlPath).isDir())
        {
            QStringList propFiles = QDir(ymlPath).entryList(QStringList() << "*.yml" << "*.json", QDir::Files);
            foreach(QString filename, propFiles)
            {
                QString fullPath = QDir(ymlPath).filePath(filename);
                parameters::file_info fi{};
                if (!parameters::yaml::parser::parse(fullPath.toStdString(), false, fi))
                    logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::parametersFileInvalid),
                        { {"File name", fullPath} });

                // ��������� �������� - �����������, ��� ��� � ���������� ������, �� �� ����� �������������� � xml �����
                // ������� ��������� ����� �� ��� � � ��������� ����������
                if (fi.info.category != "Misc" && fi.info.category != "misc")
                {
                    parameters::parameter_info pi{};
                    pi.type = QString::fromLocal8Bit("array<string>").toStdString();
                    pi.name = ids_.dependencies.toString().toStdString();
                    pi.display_name = QString::fromLocal8Bit("�����������").toStdString();
                    pi.description = QString::fromLocal8Bit("����������� ����� �� ������ ������").toStdString();
                    pi.required = QString::fromLocal8Bit("false").toStdString();
                    pi.default_ = QString::fromLocal8Bit("�� ������").toStdString();
                    fi.parameters.push_back(std::move(pi));
                }

                if (!unitIdUnitParametersPtr_.contains(QString::fromStdString(fi.info.id)))
                    unitIdUnitParametersPtr_[QString::fromStdString(fi.info.id)] = CubesUnit::CreateUnitParametersPtr();
                auto up = unitIdUnitParametersPtr_[QString::fromStdString(fi.info.id)];
                up->fileInfo = fi;
                up->platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

// Units
bool TopManager::AddMainFile(const CubesXml::File& file, const QString& zipFileName)
{
    // �� �������, ����� ��� ��� ���-�� ��������

    CubesUnit::FileId fileId{ CubesUnit::InvalidFileId };
    fileItemsManager_->Create(file, fileId);
    fileItemsManager_->Select(fileId);

    QList<CubesUnit::PropertiesId> addedPropertiesIds;
    if (!AddUnits(fileId, CubesUnit::InvalidIncludeId, file.config.groups, addedPropertiesIds))
        return false;

    if (zipFileName.isEmpty())
    {
        // ����������� ������ �� fileItemsManager_, ����� �������� includeId
        CubesUnit::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;
        QDir dir = QFileInfo(file.fileName).absoluteDir();

        for (const auto& includeId : includes.keys())
        {
            QString includePath;
            if (!fileItemsManager_->GetFileIncludePath(fileId, includeId, includePath))
                return false;

            QString includeName = dir.filePath(includePath);
            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(includeName, includedFile, this))
                return false;

            QList<CubesUnit::PropertiesId> addedPropertiesIds;
            if (!AddUnits(fileId, includeId, includedFile.config.groups, addedPropertiesIds))
                return false;
        }
    }
    else
    {
        // ����������� ������ �� fileItemsManager_, ����� �������� includeId
        CubesUnit::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;

        for (const auto& includeId : includes.keys())
        {
            QString includePath;
            if (!fileItemsManager_->GetFileIncludePath(fileId, includeId, includePath))
                return false;

            QByteArray byteArray;
            if (!CubesZip::UnZipFile(zipFileName, includePath, byteArray))
                return false;

            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(byteArray, includePath, includedFile, this))
                return false;

            QList<CubesUnit::PropertiesId> addedPropertiesIds;
            if (!AddUnits(fileId, includeId, includedFile.config.groups, addedPropertiesIds))
                return false;
        }
    }

    if (!SortUnitsRectangular(true))
        return false;

    return true;
}

bool TopManager::AddUnits(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
    const QList<CubesXml::Group>& groups, QList<CubesUnit::PropertiesId>& addedPropertiesIds)
{
    QVector<CubesXml::Unit> all_units;
    for (const auto& g : groups)
    {
        for (const auto& u : g.units)
        {
            auto up = GetUnitParametersPtr(u.id);
            if (up != nullptr)
            {
                all_units.push_back(u);
            }
            else
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::noParametersFile),
                    { {"Unit name", u.name}, {"Unit id", u.id}});
            }
        }
    }

    // Get fileNames list
    CubesUnit::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    QString fileName = fileItemsManager_->GetFileName(fileId);

    // Transform
    for (int i = 0; i < all_units.size(); i++)
    {
        QString name = all_units[i].id;
        auto up = GetUnitParametersPtr(name);

        if (up != nullptr)
        {
            CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
            propertiesItemsManager_->Create(all_units[i], propertiesId);
            auto pi = propertiesItemsManager_->GetItem(propertiesId);

            addedPropertiesIds.push_back(propertiesId);

            //pi->ApplyXmlProperties(all_units[i]);
            pi->SetFileIdNames(fileNames);
            pi->SetFileIdName(fileId, fileName);
            //if (includedFileName != "")
            {
                CubesUnit::IncludeIdNames includes;
                if (!fileItemsManager_->GetFileIncludeNames(fileId, true, includes))
                    return false;

                QString includeName;
                if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
                    includeName = "<not selected>";

                pi->SetIncludeIdNames(includes);
                pi->SetIncludeIdName(includeId, includeName);
            }

            CubesDiagram::PropertiesForDrawing pfd{};
            if (!GetPropetiesForDrawing(propertiesId, pfd))
            {
                qDebug() << "ERROR GetPropeties: " << propertiesId;
            }

            //di = new CubeDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
            //di->setX(all_units[i].x);
            //di->setY(all_units[i].y);
            //di->setZValue(all_units[i].z);
            //scene_->addItem(di);
        }
        else
        {
            // error
        }
    }

    //if (all_units.size() > 0 && di != nullptr)
    //{
    //    scene_->clearSelection();
    //    propertiesItemsManager_->Select(0);
    //    DiagramAfterItemCreated(di);
    //}
    //if (!SortUnitsRectangular())
    //    return false;

    return true;
}


bool TopManager::SortUnitsBoost()
{
    return propertiesItemsManager_->SortUnitsBoost();
}

bool TopManager::SortUnitsRectangular(bool check)
{
    return propertiesItemsManager_->SortUnitsRectangular(check);
}

CubesUnit::UnitParametersPtr TopManager::GetUnitParametersPtr(const QString& id)
{
    for (auto& up : unitIdUnitParametersPtr_)
    {
        if (QString::fromStdString(up->fileInfo.info.id) == id)
            return up;
    }
    return nullptr;
}

bool TopManager::Test()
{
    const auto fileIdParameterModelPtrs = fileItemsManager_->GetFileIdParameterModelPtrs();
    const auto propertiesIdParameterModelPtrs = propertiesItemsManager_->GetPropertiesIdParameterModelPtrs();

    analysisManager_->Test(fileIdParameterModelPtrs, propertiesIdParameterModelPtrs, unitIdUnitParametersPtr_);
    return true;
}

// Files
CubesUnit::FileIdNames TopManager::GetFileNames()
{
    return fileItemsManager_->GetFileNames();
}

//QString TopManager::GetCurrentFileName()
//{
//    return fileItemsManager_->GetCurrentFileName();
//}

CubesUnit::IncludeIdNames TopManager::GetCurrentFileIncludeNames()
{
    auto fileId = fileItemsManager_->GetFileId(fileItemsManager_->GetCurrentFileName());
    CubesUnit::IncludeIdNames includes;
    if (!fileItemsManager_->GetFileIncludeNames(fileId, true, includes))
        return {};

    return includes;
}

//QString TopManager::GetDisplayName(const QString& baseName)
//{
//    CubesUnit::VariableIdVariables variables;
//    for (const auto& item : scene_->items())
//    {
//        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
//        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//        if (pi->GetIncludeName() != "<not selected>")
//        {
//            auto fileId = fileItemsManager_->GetFileId(pi->GetName());
//            if (!fileItemsManager_->GetFileIncludeVariables(fileId, pi->GetIncludeId(), variables))
//                return {};
//        }
//    }
//
//    QString realName = baseName;
//    for (const auto& v : variables)
//    {
//        QString replace = QString("@%1@").arg(v.first);
//        realName.replace(replace, v.second);
//    }
//
//    return realName;
//}

//void TopManager::UpdateFileState(const QString& path, bool modified)
//{
//    QString title = path.isEmpty() ? "untitled" : path;
//    if (modified) title += "*";
//    //setWindowTitle(title);
//    path_ = path;
//    modified_ = modified;
//}

bool TopManager::NewFile()
{
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();

    return true;
}

bool TopManager::SaveFile(const QString& path)
{
    //log_table_model_->Clear();
    
    // �������� ������ ������� ������
    bool is_first = true;
    CubesUnit::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    
    if (fileNames.isEmpty())
    {
        logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::noParametersFile));
        return false;
    }

    for (const auto& kvpFile : fileNames.toStdMap())
    {
        auto xmlFile = fileItemsManager_->GetXmlFile(kvpFile.first);
        QFileInfo xmlFileInfo(xmlFile.fileName);
        const auto xmlFileName = xmlFileInfo.fileName();
        const auto xmlZipFilePath = path;

        {
            auto xmlGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first);
            xmlFile.config.groups = std::move(xmlGroups);

            QByteArray byteArray;
            if (!CubesXml::Helper::Write(byteArray, xmlFile))
                return false;

            if (is_first)
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Create))
                {
                    logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::zipFileError),
                        { {"Xml file name", xmlFileName}, {"Zip file name", xmlZipFilePath} });
                    return false;
                }

                is_first = false;
            }
            else
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                {
                    logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::zipFileError),
                        { {"Xml file name", xmlFileName}, {"Zip file name", xmlZipFilePath} });
                    return false;
                }
            }
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        CubesUnit::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(kvpFile.first, false, includes))
            return false;

        for (const auto& kvpInclude : includes.toStdMap())
        {
            auto includeGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first, kvpInclude.first);
            CubesXml::File includeXmlFile{};

            const auto item = fileItemsManager_->GetItem(kvpFile.first);
            const auto includeName = item->GetIncludeName(kvpInclude.first);
            const auto includePath = item->GetIncludePath(kvpInclude.first);

            ///includeXmlFile.name = includeName;
            //includeXmlFile.platform = xmlFile.platform;
            includeXmlFile.fileName = includePath;

            includeXmlFile.config.logIsSet = false;
            includeXmlFile.config.networkingIsSet = false;
            includeXmlFile.config.groups = includeGroups;


            QFileInfo includeXmlFileInfo(includeXmlFile.fileName);
            const auto includeXmlFileName = includeXmlFileInfo.fileName();


            QByteArray byteArray;
            if (!CubesXml::Helper::Write(byteArray, includeXmlFile))
                return false;

            if (!CubesZip::ZipFile(byteArray, includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                return false;

            //CubesXml::Writer::Write(includeXmlFileName, includeXmlFile);
            //CubesZip::ZipFile(includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append);
        }
    }

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

bool TopManager::SaveFolder(const QString& path)
{
    //log_table_model_->Clear();

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);

    // �������� ������ ������� ������
    bool is_first = true;
    CubesUnit::FileIdNames fileNames = fileItemsManager_->GetFileNames();

    if (fileNames.isEmpty())
    {
        logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::noParametersFile));
        return false;
    }

    for (const auto& kvpFile : fileNames.toStdMap())
    {
        auto xmlFile = fileItemsManager_->GetXmlFile(kvpFile.first);
        QFileInfo xmlFileInfo(xmlFile.fileName);
        const auto xmlFileName = xmlFileInfo.fileName();
        const auto xmlZipFilePath = path;

        {
            auto xmlGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first);
            xmlFile.config.groups = std::move(xmlGroups);

            if (!CubesXml::Helper::Write(dir.filePath(xmlFileName), xmlFile))
                return false;


            //QByteArray byteArray;
            //if (!CubesXml::Helper::Write(byteArray, xmlFile))
            //    return false;

            //if (is_first)
            //{
            //    if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Create))
            //        return false;

            //    is_first = false;
            //}
            //else
            //{
            //    if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
            //        return false;
            //}
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        CubesUnit::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(kvpFile.first, false, includes))
            return false;
        for (const auto& kvpInclude : includes.toStdMap())
        {
            auto includeGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first, kvpInclude.first);
            CubesXml::File includeXmlFile{};

            const auto item = fileItemsManager_->GetItem(kvpFile.first);
            const auto includeName = item->GetIncludeName(kvpInclude.first);
            const auto includePath = item->GetIncludePath(kvpInclude.first);

            ///includeXmlFile.name = includeName;
            //includeXmlFile.platform = xmlFile.platform;
            includeXmlFile.fileName = includePath;

            includeXmlFile.config.logIsSet = false;
            includeXmlFile.config.networkingIsSet = false;
            includeXmlFile.config.groups = includeGroups;


            QFileInfo includeXmlFileInfo(includeXmlFile.fileName);
            const auto includeXmlFileName = includeXmlFileInfo.fileName();

            if (!CubesXml::Helper::Write(dir.filePath(includeXmlFileName), includeXmlFile))
                return false;


            //QByteArray byteArray;
            //if (!CubesXml::Helper::Write(byteArray, includeXmlFile))
            //    return false;

            //if (!CubesZip::ZipFile(byteArray, includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
            //    return false;

            //CubesXml::Writer::Write(includeXmlFileName, includeXmlFile);
            //CubesZip::ZipFile(includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append);
        }
    }

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

bool TopManager::OpenFile(const QString& path)
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    //UpdateFileState("", false);

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile())
        return false;

    QList<QString> fileNames;
    if (!CubesZip::GetZippedFileNames(path, fileNames))
        return false;

    for (const auto& fileName : fileNames)
    {
        QByteArray byteArray;
        if (!CubesZip::UnZipFile(path, fileName, byteArray))
            return false;

        CubesXml::File f{};
        CubesXml::Helper::Parse(byteArray, fileName, f, this);

        if (f.config.networkingIsSet)
        {
            if (!AddMainFile(f, path))
                return false;
        }
        else
        {
            continue;
        }
    }

    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    const auto position = pi->GetPosition();
    //    di->setPos(position);
    //}

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

bool TopManager::OpenFolder(const QString& path)
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    //UpdateFileState("", false);

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);
    QStringList fileNames = dir.entryList({ "*.xml" }, QDir::Files);

    for (const auto& fileName : fileNames)
    {
        CubesXml::File f{};
        CubesXml::Helper::Parse(dir.filePath(fileName), f, this);

        if (f.config.networkingIsSet)
        {
            if (!AddMainFile(f, ""))
                return false;
        }
        else
        {
            continue;
        }
    }

    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    const auto position = pi->GetPosition();
    //    di->setPos(position);
    //}

    //UpdateFileState("", true);

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

bool TopManager::ImportXml(const QString& path)
{
    CubesXml::File f{};

    if (!CubesXml::Helper::Parse(path, f, this))
        return false;

    if (!AddMainFile(f, ""))
        return false;

    return true;
}


// FileItemsManager
void TopManager::FileNameChanged(CubesUnit::FileId fileId)
{
    const auto fileName = fileItemsManager_->GetFileName(fileId);
    if (!propertiesItemsManager_->InformFileNameChanged(fileId, fileName))
        return;

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileListChanged(const CubesUnit::FileIdNames& fileNames)
{
    if (!propertiesItemsManager_->InformFileListChanged(fileNames))
        return;

    // ���� ��� ������ ����, ���� ������������ �����
    if (fileNames.size() == 1)
    {
        if (!propertiesItemsManager_->InformFileColorChanged(*fileNames.keyBegin()))
            return;
    }

    // ���� item ��� ��������, ����� ��� �� ������ �����, pm->key ����� �� �����
    // ����� ����������, � ��� ��������� ��� ����� � ����
    // TODO: �������� ��������� ��� - �� �� ������
    // TODO: ��������� ��� �� ����� ����� ��������� ���������, ����� �� ����� �������� � Item
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    const auto color = fileItemsManager_->GetFileColor(pi->GetFileId());
    //    di->InformColorChanged(color);
    //}

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId)
{
    QString includeName;
    if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
        return;

    if (!propertiesItemsManager_->InformIncludeNameChanged(fileId, includeId, includeName))
        return;

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames)
{
    if (!propertiesItemsManager_->InformIncludesListChanged(fileId, includeNames))
        return;

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileVariableNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
    const QString& variableName, const QString& oldVariableName)
{
    if (!propertiesItemsManager_->InformVariableChanged())
        return;

    //UpdateFileState(path_, true);
}

void TopManager::FileVariablesListChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
    const CubesUnit::VariableIdVariables& variables)
{
    if (!propertiesItemsManager_->InformVariableChanged())
        return;

    //UpdateFileState(path_, true);
}

void TopManager::FileColorChanged(CubesUnit::FileId fileId, const QColor& color)
{
    if (!propertiesItemsManager_->InformFileColorChanged(fileId))
        return;

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FilePropertiesChanged()
{
    //UpdateFileState(path_, true);
}

// PropertiesItemsManager
void TopManager::PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
    CubesUnit::FileId fileId, CubesUnit::IncludeId includeId)
{
}

void TopManager::PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId)
{
}

void TopManager::PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ)
{
}

void TopManager::PropertiesSizeChanged(CubesUnit::PropertiesId propertiesId, QSizeF size)
{
}

void TopManager::PropertiesTextChanged(CubesUnit::PropertiesId propertiesId, QString text, uint32_t fontSize, bool showBorder,
    CubesDiagram::HorizontalAlignment horizontalAlignment, CubesDiagram::VerticalAlignment verticalAlignment)
{
}

void TopManager::PropertiesConnectionChanged(CubesUnit::PropertiesId propertiesId)
{
}

void TopManager::PropertiesPropertiesChanged()
{
}
