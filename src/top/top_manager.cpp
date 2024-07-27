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
    propertiesItemsManager_->SetSelectedItemChangedDelegate(std::bind<void>(&TopManager::PropertiesSelectedItemChanged, this, _1));
    propertiesItemsManager_->SetErrorDelegate(std::bind<void>(&TopManager::PropertiesError, this, _1, _2));
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

bool TopManager::GetUnitParameters(const QString& unitId, CubesUnit::UnitParameters& unitParameters)
{
    // Для массивов - мы не знаем название, но у нас всего один элемент в списке
    if (unitId.isEmpty() && unitIdUnitParameters_.size() == 1)
    {
        unitParameters = *unitIdUnitParameters_.begin();
    }
    else
    {
        const auto it = unitIdUnitParameters_.find(unitId);
        if (it == unitIdUnitParameters_.end())
            return false;
        unitParameters = *it;
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

bool TopManager::GetPropetiesForDrawing(CubesUnit::PropertiesId propertiesId, PropertiesForDrawing& pfd)
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

    return true;
}

bool TopManager::GetPropetiesUnitParameters(CubesUnit::PropertiesId propertiesId, CubesUnit::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool TopManager::GetPropetiesUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

bool TopManager::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetUnitsConnections(connections);
}

bool TopManager::GetDependsConnections(QMap<QString, QStringList>& connections)
{
    // TODO: REF!!! не копировать параметры
    const auto fileIdParameterModelPtrs = fileItemsManager_->GetFileIdParameterModelPtrs();
    const auto propertiesIdParameterModelPtrs = propertiesItemsManager_->GetFileIdParameterModelPtrs();

    for (auto& kvp : propertiesIdParameterModels.toStdMap())
    {
        const auto depends = CubesUnit::Helper::Analyse::GetParameterModelsDependencies(kvp.second,
            fileIdParameterModels, unitIdUnitParameters_);

        if (!depends.empty())
        {
            const auto name = CubesUnit::Helper::Analyse::GetResolvedUnitName(kvp.second, fileIdParameterModels);
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

                // Добавляем параметр - зависимости, его нет в параметрах юнитов, но он может присутствовать в xml файле
                // Принцип обработки такой же как и у остальных параметров
                //if (fi.info.id != "group" && fi.info.id != "group_mock")
                {
                    parameters::parameter_info pi{};
                    pi.type = QString::fromLocal8Bit("array<string>").toStdString();
                    pi.name = ids_.dependencies.toString().toStdString();
                    pi.display_name = QString::fromLocal8Bit("Зависимости").toStdString();
                    pi.description = QString::fromLocal8Bit("Зависимости юнита от других юнитов").toStdString();
                    pi.required = QString::fromLocal8Bit("false").toStdString();
                    pi.default_ = QString::fromLocal8Bit("не задано").toStdString();
                    fi.parameters.push_back(std::move(pi));
                }

                auto& up = unitIdUnitParameters_[QString::fromStdString(fi.info.id)];
                up.fileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

// Units
bool TopManager::AddMainFile(const CubesXml::File& file, const QString& zipFileName)
{
    // Не очищаем, вдруг там уже что-то поменяно

    CubesUnit::FileId fileId{ CubesUnit::InvalidFileId };
    fileItemsManager_->Create(file, fileId);
    fileItemsManager_->Select(fileId);

    if (!AddUnits(fileId, CubesUnit::InvalidIncludeId, file))
        return false;

    if (zipFileName.isEmpty())
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
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

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }
    }
    else
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
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

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }
    }

    if (!SortUnitsRectangular(true))
        return false;

    return true;
}

bool TopManager::AddUnits(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId, const CubesXml::File& file)
{
    QVector<CubesXml::Unit> all_units;
    for (const auto& g : file.config.groups)
    {
        for (const auto& u : g.units)
        {
            auto up = GetUnitParameters(u.id);
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
        auto up = GetUnitParameters(name);

        if (up != nullptr)
        {
            CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
            propertiesItemsManager_->Create(all_units[i], propertiesId);
            auto pi = propertiesItemsManager_->GetItem(propertiesId);

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

            PropertiesForDrawing pfd{};
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

CubesUnit::UnitParameters* TopManager::GetUnitParameters(const QString& id)
{
    for (auto& up : unitIdUnitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

bool TopManager::Test()
{
    //log_table_model_->Clear();
    CubesUnit::FileIdParameterModels fileModels;
    if (!fileItemsManager_->GetParameterModels(fileModels))
        return false;
    CubesUnit::PropertiesIdParameterModels propertiesModels;
    if (!propertiesItemsManager_->GetParameterModels(propertiesModels))
        return false;
    //CubesUnit::PropertiesIdUnitParameters unitParameters;
    //if (!propertiesItemsManager_->GetUnitParameters(unitParameters))
    //    return false;
    analysisManager_->Test(fileModels, propertiesModels, unitIdUnitParameters_);
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

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnit::FileIdNames fileNames = fileItemsManager_->GetFileNames();
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

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnit::FileIdNames fileNames = fileItemsManager_->GetFileNames();
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

    // Если это первый файл, надо перерисовать рамки
    if (fileNames.size() == 1)
    {
        if (!propertiesItemsManager_->InformFileColorChanged(*fileNames.keyBegin()))
            return;
    }

    // Если item был добавлен, когда нет ни одного файла, pm->key будет не задан
    // После добавления, у них изменится имя файла и цвет
    // TODO: возможно изменится имя - но не должно
    // TODO: разрулить как то через слоты изменения состояния, чтобы не лезть напрямую в Item
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

void TopManager::PropertiesError(CubesUnit::PropertiesId propertiesId, const QString& message)
{
    //CubesLog::Message lm{};
    //lm.type = CubesLog::MessageType::error;
    //lm.tag = 0;
    //lm.source = QString("%1").arg(propertiesId);
    //lm.description = message;
    //AddMessage(lm);
}

void TopManager::PropertiesConnectionChanged(CubesUnit::PropertiesId propertiesId)
{
}

void TopManager::PropertiesPropertiesChanged()
{
}
