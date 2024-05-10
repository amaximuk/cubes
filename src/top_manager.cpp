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
#include "top_manager.h"

TopManager::TopManager(QWidget *parent)
{
    modified_ = false;
    uniqueNumber_ = 0;

    UpdateFileState("", false);

    fileItemsManager_ = new CubesFile::FileItemsManager(this, this);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &TopManager::FileNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &TopManager::FileListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &TopManager::FileIncludeNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &TopManager::FileIncludesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &TopManager::FileVariableNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &TopManager::FileVariablesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::ColorChanged, this, &TopManager::FileColorChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::PropertiesChanged, this, &TopManager::FilePropertiesChanged);
    
    propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, this, false);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &TopManager::PropertiesBasePropertiesChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &TopManager::PropertiesSelectedItemChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &TopManager::PropertiesPositionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::Error, this, &TopManager::PropertiesError);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::ConnectionChanged, this, &TopManager::PropertiesConnectionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PropertiesChanged, this, &TopManager::PropertiesPropertiesChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FileNameChanged, this, &TopManager::propertiesFileNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FilesListChanged, this, &TopManager::fileListChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludeNameChanged, this, &TopManager::fileIncludeNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludesListChanged, this, &TopManager::fileIncludesListChanged);

    analysisManager_ = new CubesAnalysis::AnalysisManager(this, this);

    FillParametersInfo();

    //uint32_t fileId{ 0 };
    //fileItemsManager_->Create(QString::fromLocal8Bit("config.xml"), QString::fromLocal8Bit("���"), QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

TopManager::~TopManager()
{
}

// ITopManager
bool TopManager::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool TopManager::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool TopManager::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
{
    unitParameters = unitParameters_[unitId];
    return true;
}

bool TopManager::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
{
    return fileItemsManager_->GetFileIncludeNames(fileId, true, includeNames);
}

bool TopManager::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
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
    pi->SetIncludeIdName(CubesUnitTypes::InvalidIncludeId, "<not selected>");
    return true;
}

bool TopManager::GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd)
{
    //if (!propertiesItemsManager_->GetPropetiesForDrawing(propertiesId, pfd))
    //    return false;

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

bool TopManager::GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool TopManager::GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

bool TopManager::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetUnitsConnections(connections);
}

bool TopManager::GetDependsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetDependsConnections(connections);
}

bool TopManager::CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId, const PropertiesForDrawing& pfd, QPointF pos)
{
    return false;
}

bool TopManager::EnshureVisible(uint32_t propertiesId)
{
    return false;
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
void TopManager::AddMessage(const CubesLog::LogMessage& m)
{
}

void TopManager::FillParametersInfo()
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
                    lm.description = QString::fromLocal8Bit("���� ���������� %1 �� ��������. ��������� �� ���������.").arg(fullPath);
                    AddMessage(lm);
                }

                // ��������� �������� - �����������, ��� ��� � ���������� ������, �� �� ����� �������������� � xml �����
                // ������� ��������� ����� �� ��� � � ��������� ����������
                //if (fi.info.id != "group" && fi.info.id != "group_mock")
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

                auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
                up.fileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

// Units
bool TopManager::AddMainFile(const CubesXml::File& file, const QString& zipFileName)
{
    // �� �������, ����� ��� ��� ���-�� ��������
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
    //    u.name = QString::fromLocal8Bit("������ %1").arg(i);
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
        // ����������� ������ �� fileItemsManager_, ����� �������� includeId
        CubesUnitTypes::IncludeIdNames includes;
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
        // ����������� ������ �� fileItemsManager_, ����� �������� includeId
        CubesUnitTypes::IncludeIdNames includes;
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



    //if (!SortUnitsRectangular(true))
    //    return false;

    return true;
}

bool TopManager::AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file)
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
                lm.description = QString::fromLocal8Bit("��� ����� ���������� ��� ����� %1 (%2). ���� �� ��������.").arg(u.name, u.id);
                AddMessage(lm);
            }
        }
    }

    // Get fileNames list
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    QString fileName = fileItemsManager_->GetFileName(fileId);

    // Transform
    CubesDiagram::DiagramItem* di = nullptr;
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

CubesUnitTypes::UnitParameters* TopManager::GetUnitParameters(const QString& id)
{
    for (auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

bool TopManager::Test()
{
    //log_table_model_->Clear();
    analysisManager_->Test();
    return true;
}

// Files
CubesUnitTypes::FileIdNames TopManager::GetFileNames()
{
    return fileItemsManager_->GetFileNames();
}

//QString TopManager::GetCurrentFileName()
//{
//    return fileItemsManager_->GetCurrentFileName();
//}

CubesUnitTypes::IncludeIdNames TopManager::GetCurrentFileIncludeNames()
{
    auto fileId = fileItemsManager_->GetFileId(fileItemsManager_->GetCurrentFileName());
    CubesUnitTypes::IncludeIdNames includes;
    if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
        return {};

    return includes;
}

//QString TopManager::GetDisplayName(const QString& baseName)
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

void TopManager::UpdateFileState(const QString& path, bool modified)
{
    QString title = path.isEmpty() ? "untitled" : path;
    if (modified) title += "*";
    //setWindowTitle(title);
    path_ = path;
    modified_ = modified;
}

bool TopManager::SaveFileInternal(const QString& path)
{
    //log_table_model_->Clear();

    // �������� ������ ������� ������
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

    //AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool TopManager::SaveFolderInternal(const QString& path)
{
    //log_table_model_->Clear();

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);

    // �������� ������ ������� ������
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

bool TopManager::OpenFileInternal(const QString& path)
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

    //AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool TopManager::OpenFolderInternal(const QString& path)
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
void TopManager::FileNameChanged(const CubesUnitTypes::FileId& fileId)
{
    const auto fileName = fileItemsManager_->GetFileName(fileId);
    if (!propertiesItemsManager_->InformFileNameChanged(fileId, fileName))
        return;

    UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
    if (!propertiesItemsManager_->InformFileListChanged(fileNames))
        return;

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

    UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId)
{
    QString includeName;
    if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
        return;

    if (!propertiesItemsManager_->InformIncludeNameChanged(fileId, includeId, includeName))
        return;

    UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
    if (!propertiesItemsManager_->InformIncludesListChanged(fileId, includeNames))
        return;

    UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const QString& variableName, const QString& oldVariableName)
{
    if (!propertiesItemsManager_->InformVariableChanged())
        return;

    UpdateFileState(path_, true);
}

void TopManager::FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::VariableIdVariables& variables)
{
    if (!propertiesItemsManager_->InformVariableChanged())
        return;

    UpdateFileState(path_, true);
}

void TopManager::FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{
    if (!propertiesItemsManager_->InformFileColorChanged(fileId))
        return;

    UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::FilePropertiesChanged()
{
    UpdateFileState(path_, true);
}

// PropertiesItemsManager
void TopManager::PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
    const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId)
{
    //for (auto& item : scene_->items())
    //{
    //    CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
    //    if (di->propertiesId_ == propertiesId)
    //    {
    //        di->name_ = name;
    //        di->fileName_ = fileName;
    //        di->includeName_ = includeName;
    //        auto fileId = fileItemsManager_->GetFileId(fileName);
    //        di->color_ = fileItemsManager_->GetFileColor(fileId);
    //        di->InformNameChanged(name, "");
    //        di->InformIncludeChanged();
    //    }
    //}

    //UpdateFileState(path_, true);

    //scene_->invalidate();
}

void TopManager::PropertiesSelectedItemChanged(const uint32_t propertiesId)
{
}

void TopManager::PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ)
{
    //for (auto& item : scene_->items())
    //{
    //    CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
    //    if (di->propertiesId_ == propertiesId)
    //    {
    //        di->setPos(QPointF(posX, posY));
    //        di->setZValue(posZ);
    //        break;
    //    }
    //}

    //UpdateFileState(path_, true);
}

void TopManager::PropertiesError(const uint32_t propertiesId, const QString& message)
{
    CubesLog::LogMessage lm{};
    lm.type = CubesLog::MessageType::error;
    lm.tag = 0;
    lm.source = QString("%1").arg(propertiesId);
    lm.description = message;
    AddMessage(lm);
}

void TopManager::PropertiesConnectionChanged(const uint32_t propertiesId)
{
    UpdateFileState(path_, true);
}

void TopManager::PropertiesPropertiesChanged()
{
    UpdateFileState(path_, true);
}

// ������
void TopManager::OnNewFileAction()
{
    //scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    //log_table_model_->Clear();

    UpdateFileState("", false);
}

void TopManager::OnOpenFileAction()
{
    //if (!OpenFileInternal(selectedFileNames[0]))
    //{
    //    return;
    //}
}

void TopManager::OnOpenFolderAction()
{
    //if (!OpenFolderInternal(folderPath))
    //{
    //    return;
    //}
}

void TopManager::OnImportXmlFileAction()
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

void TopManager::OnSaveFileAction()
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

void TopManager::OnSaveAsFileAction()
{
    //if (!SaveFileInternal(selectedFileName))
    //{
    //    return;
    //}
}

void TopManager::OnSaveFolderAction()
{
    //if (!SaveFolderInternal(folderPath))
    //{
    //    return;
    //}
}

void TopManager::OnSortBoostAction()
{
    //SortUnits();
}

void TopManager::OnSortRectAction()
{
    //SortUnitsRectangular(false);
}

void TopManager::OnTestAction()
{
    Test();
}