#include "parameters/yaml_parser.h"
#include "file_item/file_item.h"
#include "file_item/file_items_manager.h"
#include "properties_item/properties_item.h"
#include "properties_item/properties_items_manager.h"
#include "analysis/analysis_manager.h"
#include "xml/xml_parser.h"
#include "xml/xml_writer.h"
#include "xml/xml_helper.h"
#include "graph.h"
#include "zip.h"
#include "mock_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));
    UpdateFileState("", false);

    fileItemsManager_ = new CubesFile::FileItemsManager(this, this);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &MainWindow::FileNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &MainWindow::FileListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &MainWindow::FileIncludeNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &MainWindow::FileIncludesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &MainWindow::FileVariableNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &MainWindow::FileVariablesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::ColorChanged, this, &MainWindow::FileColorChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::PropertiesChanged, this, &MainWindow::FilePropertiesChanged);
    
    propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, this, false);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &MainWindow::PropertiesBasePropertiesChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &MainWindow::PropertiesSelectedItemChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &MainWindow::PropertiesPositionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::Error, this, &MainWindow::PropertiesError);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::ConnectionChanged, this, &MainWindow::PropertiesConnectionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PropertiesChanged, this, &MainWindow::PropertiesPropertiesChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FileNameChanged, this, &MainWindow::propertiesFileNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FilesListChanged, this, &MainWindow::fileListChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludeNameChanged, this, &MainWindow::fileIncludeNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludesListChanged, this, &MainWindow::fileIncludesListChanged);

    analysisManager_ = new CubesAnalysis::AnalysisManager(this, this);

    FillParametersInfo();

    //uint32_t fileId{ 0 };
    //fileItemsManager_->Create(QString::fromLocal8Bit("config.xml"), QString::fromLocal8Bit("АРМ"), QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

MainWindow::~MainWindow()
{
}

// ITopManager
bool MainWindow::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool MainWindow::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool MainWindow::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
{
    unitParameters = unitParameters_[unitId];
    return true;
}

bool MainWindow::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
{
    return fileItemsManager_->GetFileIncludeNames(fileId, true, includeNames);
}

bool MainWindow::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
{
    return fileItemsManager_->GetFileIncludeVariables(fileId, includeId, variables);
}

bool MainWindow::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    propertiesItemsManager_->Create(unitId, propertiesId);
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    pi->SetFileIdNames(GetFileNames());
    pi->SetFileIdName(fileItemsManager_->GetCurrentFileId(), fileItemsManager_->GetCurrentFileName());
    pi->SetIncludeIdNames(GetCurrentFileIncludeNames());
    pi->SetIncludeIdName(CubesUnitTypes::InvalidIncludeId, "<not selected>");
    return true;
}

bool MainWindow::GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd)
{
    if (!propertiesItemsManager_->GetPropetiesForDrawing(propertiesId, pfd))
        return false;
    auto fileId = fileItemsManager_->GetFileId(pfd.fileName);
    pfd.color = fileItemsManager_->GetFileColor(fileId);
    return true;
}

bool MainWindow::GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool MainWindow::GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

QMap<QString, QStringList> MainWindow::GetUnitsConnections()
{
    return GetConnectionsInternal(false);
}

QMap<QString, QStringList> MainWindow::GetDependsConnections()
{
    return GetConnectionsInternal(true);
}

bool MainWindow::EnshureVisible(uint32_t propertiesId)
{
    return false;
}

bool MainWindow::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
    return fileItemsManager_->GetAnalysisFiles(files);
}

bool MainWindow::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
    return propertiesItemsManager_->GetAnalysisProperties(properties);
}

// ILogManager
void MainWindow::AddMessage(const CubesLog::LogMessage& m)
{
}

void MainWindow::FillParametersInfo()
{
    QString directoryPath(QCoreApplication::applicationDirPath() + "/doc/all_units_solid");
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
                {
                    CubesLog::LogMessage lm{};
                    lm.type = CubesLog::MessageType::error;
                    lm.tag = 0;
                    lm.source = filename;
                    lm.description = QString::fromLocal8Bit("Файл параметров %1 не разобран. Параметры не добавлены.").arg(fullPath);
                    AddMessage(lm);
                }

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

                auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
                up.fileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

// Units
bool MainWindow::AddMainFile(const CubesXml::File& file, const QString& zipFileName)
{
    // Не очищаем, вдруг там уже что-то поменяно
    //if (scene_->items().size() == 0)
    //    fileItemsManager_->Clear();

    //QString fileName = QFileInfo(file.fileName).fileName();

    //QString name = file.name;
    CubesUnitTypes::FileId fileId{ CubesUnitTypes::InvalidFileId };
    fileItemsManager_->Create(file, fileId);
    fileItemsManager_->Select(fileId);
    //QString name;
    //auto res = fileItemsManager_->GetName(fileId, name);

    //for (const auto& include : file.includes)
    //    fileItemsManager_->AddFileInclude(fileId, include.fileName, include.variables);

    //for (int i = 0; i < file.includes.size(); i++)
    //{
    //    QList<QPair<QString, QString>> variables;
    //    for (const auto& kvp : file.includes[i].variables.toStdMap())
    //        variables.push_back({ kvp.first, kvp.second });
    //}

    //QStringList fileNames = fileItemsManager_->GetFileNames();
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    pi->SetFileNames(fileNames);
    //}

    // Convert includes into unit
    //CubesXml::Group g{};
    //g.path = "service";
    //for (int i = 0; i < file.includes.size(); i++)
    //{
    //    CubesXml::Unit u{};
    //    u.id = "group";
    //    u.name = QString::fromLocal8Bit("Группа %1").arg(i);
    //    CubesXml::Param p{};
    //    p.name = "FILE_PATH";
    //    p.type = "str";
    //    p.val = file.includes[i].fileName;
    //    u.params.push_back(std::move(p));
    //    CubesXml::Array a{};
    //    a.name = "VARIABLES";
    //    for (const auto& kvp : file.includes[i].variables.toStdMap())
    //    {
    //        CubesXml::Item i1{};
    //        CubesXml::Param p1{};
    //        p1.name = "NAME";
    //        p1.type = "str";
    //        p1.val = kvp.first;
    //        i1.params.push_back(std::move(p1));
    //        CubesXml::Param p2{};
    //        p2.name = "VALUE";
    //        p2.type = "str";
    //        p2.val = kvp.second;
    //        i1.params.push_back(std::move(p2));
    //        a.items.push_back(std::move(i1));
    //    }
    //    u.arrays.push_back(std::move(a));
    //    g.units.push_back(std::move(u));
    //}

    //if (file.includes.size() > 0)
    //    file.config.groups.push_back(std::move(g));

    if (!AddUnits(fileId, CubesUnitTypes::InvalidIncludeId, file))
        return false;

    if (zipFileName.isEmpty())
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;
        QDir dir = QFileInfo(file.fileName).absoluteDir();

        for (const auto& includeId : includes.keys())
        {
            const auto includePath = fileItemsManager_->GetFileIncludePath(fileId, includeId);

            QString includeName = dir.filePath(includePath);
            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(includeName, includedFile, this))
                return false;

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }

        //for (int i = 0; i < file.includes.size(); i++)
        //{
        //    QString includeFileName = dir.filePath(file.includes[i].fileName);
        //    CubesXml::File includedFile{};
        //    if (!CubesXml::Parser::Parse(includeFileName, includedFile))
        //        return false;
        //    if (!AddUnits(fileId, file.includes[i].fileName, includedFile))
        //        return false;
        //}
    }
    else
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;
        for (const auto& includeId : includes.keys())
        {
            const auto includePath = fileItemsManager_->GetFileIncludePath(fileId, includeId);

            QByteArray byteArray;
            if (!CubesZip::UnZipFile(zipFileName, includePath, byteArray))
                return false;

            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(byteArray, includePath, includedFile, this))
                return false;

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }

        //for (int i = 0; i < file.includes.size(); i++)
        //{
        //    QByteArray byteArray;
        //    if (!CubesZip::UnZipFile(zipFileName, file.includes[i].fileName, byteArray))
        //        return false;

        //    CubesXml::File includedFile{};
        //    CubesXml::Parser::Parse(byteArray, file.includes[i].fileName, includedFile);

        //    if (!AddUnits(name, file.includes[i].fileName, includedFile))
        //        return false;
        //}

    }



    if (!SortUnitsRectangular(true))
        return false;

    return true;
}

bool MainWindow::AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file)
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
                CubesLog::LogMessage lm{};
                lm.type = CubesLog::MessageType::error;
                lm.tag = 0;
                lm.source = QFileInfo(file.fileName).fileName();
                lm.description = QString::fromLocal8Bit("Нет файла параметров для юнита %1 (%2). Юнит не добавлен.").arg(u.name, u.id);
                AddMessage(lm);
            }
        }
    }

    // Get fileNames list
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    QString fileName = fileItemsManager_->GetFileName(fileId);

    // Transform
    CubeDiagram::DiagramItem* di = nullptr;
    for (int i = 0; i < all_units.size(); i++)
    {
        QString name = all_units[i].id;
        auto up = GetUnitParameters(name);

        if (up != nullptr)
        {
            uint32_t propertiesId{ 0 };
            propertiesItemsManager_->Create(all_units[i], propertiesId);
            auto pi = propertiesItemsManager_->GetItem(propertiesId);

            //pi->ApplyXmlProperties(all_units[i]);
            pi->SetFileIdNames(fileNames);
            pi->SetFileIdName(fileId, fileName);
            //if (includedFileName != "")
            {
                CubesUnitTypes::IncludeIdNames includes;
                if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
                    return false;
                // TODO: Добавить в xml название файла и убрать функцию GetFileIncludeName отовсюду за ненадобностью
                QString includeName = fileItemsManager_->GetFileIncludeName(fileId, includeId);
                if (includeName.isEmpty())
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

QMap<QString, QStringList> MainWindow::GetConnectionsInternal(bool depends)
{
    // Сюда будем собирать реальные соединения на этой сцене
    QMap<QString, QStringList> result;

    // Соберем имена юнитов на главной панели
    QStringList mainUnits;
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        //auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        QString name;
        if (propertiesItemsManager_->GetName(di->propertiesId_, name))
            mainUnits.push_back(name);
    }

    // Для юнитов сцены собираем список зависимостей, а для групп еще список юнитов
    QMap<QString, QStringList> connections;
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        QString name;
        if (propertiesItemsManager_->GetName(di->propertiesId_, name))
        {
            QStringList conn = depends ? pi->GetDependentNames() : pi->GetConnectedNames();
            if (conn.size() > 0)
                connections[name].append(conn);
        }
    }

    // Перебираем юниты сцены и для них рисуем соединения
    for (const auto& kvp : connections.toStdMap())
    {
        // Проверяем этот юнит
        QString unitName = kvp.first;

        // Перебираем все соединения
        for (const auto& name : kvp.second)
        {
            // Проверяем, что соединение с юнитом, который на этой сцене
            if (mainUnits.contains(name))
            {
                // Отсеиваем дубликаты
                if ((result.contains(unitName) && result[unitName].contains(name)) ||
                    (result.contains(name) && result[name].contains(unitName)))
                {
                    // Уже есть
                }
                else
                {
                    // Добавляем
                    result[unitName].push_back(name);
                }
            }
        }
    }

    return result;
}

CubesUnitTypes::UnitParameters* MainWindow::GetUnitParameters(const QString& id)
{
    for (auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

bool MainWindow::Test()
{
    //log_table_model_->Clear();
    analysisManager_->Test();
    return true;
}

// Files
CubesUnitTypes::FileIdNames MainWindow::GetFileNames()
{
    return fileItemsManager_->GetFileNames();
}

//QString MainWindow::GetCurrentFileName()
//{
//    return fileItemsManager_->GetCurrentFileName();
//}

CubesUnitTypes::IncludeIdNames MainWindow::GetCurrentFileIncludeNames()
{
    auto fileId = fileItemsManager_->GetFileId(fileItemsManager_->GetCurrentFileName());
    CubesUnitTypes::IncludeIdNames includes;
    if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
        return {};

    return includes;
}

//QString MainWindow::GetDisplayName(const QString& baseName)
//{
//    CubesUnitTypes::VariableIdVariables variables;
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

void MainWindow::UpdateFileState(const QString& path, bool modified)
{
    QString title = path.isEmpty() ? "untitled" : path;
    if (modified) title += "*";
    setWindowTitle(title);
    path_ = path;
    modified_ = modified;
}

bool MainWindow::SaveFileInternal(const QString& path)
{
    //log_table_model_->Clear();

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
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
                    return false;

                is_first = false;
            }
            else
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                    return false;
            }
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        CubesUnitTypes::IncludeIdNames includes;
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

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::SaveFolderInternal(const QString& path)
{
    //log_table_model_->Clear();

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
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

        CubesUnitTypes::IncludeIdNames includes;
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

bool MainWindow::OpenFileInternal(const QString& path)
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    UpdateFileState("", false);

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

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::OpenFolderInternal(const QString& path)
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    UpdateFileState("", false);

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

    UpdateFileState("", true);

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

// FileItemsManager
void MainWindow::FileNameChanged(const CubesUnitTypes::FileId& fileId)
{
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    auto fileName = fileItemsManager_->GetFileName(fileId);
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);

        if (pi->GetFileId() == fileId)
            pi->SetFileIdName(fileId, fileName);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        pi->SetFileIdNames(fileNames);

        // Если item был добавлен, когда нет ни одного файла, pm->key будет не задан
        // После добавления, у них изменится имя файла и цвет
        // TODO: возможно изменится имя - но не должно
        const auto color = fileItemsManager_->GetFileColor(pi->GetFileId());
        di->InformColorChanged(color);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId)
{
    auto includeName = fileItemsManager_->GetFileIncludeName(fileId, includeId);
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId && pi->GetIncludeId() == includeId)
            pi->SetIncludeIdName(includeId, includeName);

        //const auto includeFileId = pi->GetIncludeFileId();
        //pi->SetIncludeNames(fileIncludeNames);
        //if (includeFileId == oldIncludeId)
        //{
        //    const auto includeName = fileItemsManager_->GetFileIncludeName();
        //    pi->SetIncludeName(includeName);
        //}
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId)
            pi->SetIncludeIdNames(includeNames);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const QString& variableName, const QString& oldVariableName)
{
    propertiesItemsManager_->InformVariableChanged();

    UpdateFileState(path_, true);
}

void MainWindow::FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::VariableIdVariables& variables)
{
    propertiesItemsManager_->InformVariableChanged();

    UpdateFileState(path_, true);
}

void MainWindow::FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
        if (pi->GetFileId() == fileId)
            di->color_ = color;
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FilePropertiesChanged()
{
    UpdateFileState(path_, true);
}

// PropertiesItemsManager
void MainWindow::PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
    const QString& fileName, const QString& includeName)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->name_ = name;
            di->fileName_ = fileName;
            di->includeName_ = includeName;
            auto fileId = fileItemsManager_->GetFileId(fileName);
            di->color_ = fileItemsManager_->GetFileColor(fileId);
            di->InformNameChanged(name, "");
            di->InformIncludeChanged();
        }
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::PropertiesSelectedItemChanged(const uint32_t propertiesId)
{
    QGraphicsItem* item_to_select = nullptr;
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            item_to_select = item;
            break;
        }
    }
    //!!!!!!!!!!!!!!!!!!!
    for (auto& item : scene_->selectedItems())
    {
        if (item != item_to_select)
            item->setSelected(false);
    }

    if (item_to_select != nullptr)
        item_to_select->setSelected(true);
}

void MainWindow::PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->setPos(QPointF(posX, posY));
            di->setZValue(posZ);
            break;
        }
    }

    UpdateFileState(path_, true);
}

void MainWindow::PropertiesError(const uint32_t propertiesId, const QString& message)
{
    CubesLog::LogMessage lm{};
    lm.type = CubesLog::MessageType::error;
    lm.tag = 0;
    lm.source = QString("%1").arg(propertiesId);
    lm.description = message;
    AddMessage(lm);
}

void MainWindow::PropertiesConnectionChanged(const uint32_t propertiesId)
{
    UpdateFileState(path_, true);
}

void MainWindow::PropertiesPropertiesChanged()
{
    UpdateFileState(path_, true);
}

// Кнопки
void MainWindow::OnNewFileAction()
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    UpdateFileState("", false);
}

void MainWindow::OnOpenFileAction()
{
    //if (!OpenFileInternal(selectedFileNames[0]))
    //{
    //    return;
    //}
}

void MainWindow::OnOpenFolderAction()
{
    //if (!OpenFolderInternal(folderPath))
    //{
    //    return;
    //}
}

void MainWindow::OnImportXmlFileAction()
{
    CubesXml::File f{};
    {
        //CubesXml::Helper::Parse(fileNames[0], f, this);
    }

    if (f.config.networkingIsSet)
    {
        if (!AddMainFile(f, ""))
        {
            return;
        }
    }
}

void MainWindow::OnSaveFileAction()
{
    if (!modified_ && !path_.isEmpty())
        return;

    QString selectedFileName = path_;
    if (path_.isEmpty())
    {
        //selectedFileName = selectedFileNames[0];
    }

    if (!SaveFileInternal(selectedFileName))
    {
        return;
    }
}

void MainWindow::OnSaveAsFileAction()
{
    //if (!SaveFileInternal(selectedFileName))
    //{
    //    return;
    //}
}

void MainWindow::OnSaveFolderAction()
{
    //if (!SaveFolderInternal(folderPath))
    //{
    //    return;
    //}
}

void MainWindow::OnSortBoostAction()
{
    SortUnits();
}

void MainWindow::OnSortRectAction()
{
    SortUnitsRectangular(false);
}

void MainWindow::OnTestAction()
{
    Test();
}
