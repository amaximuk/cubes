#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "parameters.h"
#include "../main_window.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../parameters_compiler/base64.h"
#include "file_item_types.h"
#include "file_item.h"

using namespace CubesFile;
//namespace ids = CubesUnitTypes::ParameterModelIds;


FileItem::FileItem(IFileItemsManagerBoss* fileItemsManager, PropertiesEditor* editor, uint32_t fileId)
{
    fileItemsManager_ = fileItemsManager;
    editor_ = editor;
    fileId_ = fileId;
    model_ = {};
    ignoreEvents_ = false;
    notifyManager_ = false;
    uniqueNumber_ = CubesUnitTypes::InvalidIncludeId;

    CreateParametersModel(nullptr);
    CreateProperties();
    notifyManager_ = true;
}

FileItem::FileItem(IFileItemsManagerBoss* fileItemsManager, PropertiesEditor* editor, const CubesXml::File& xmlFile, uint32_t fileId)
{
    fileItemsManager_ = fileItemsManager;
    editor_ = editor;
    fileId_ = fileId;
    model_ = {};
    ignoreEvents_ = false;
    notifyManager_ = false;
    uniqueNumber_ = CubesUnitTypes::InvalidIncludeId;

    CreateParametersModel(&xmlFile);
    CreateProperties();
    notifyManager_ = true;
}

void FileItem::CreateParametersModel(const CubesXml::File* xmlFile)
{
    // BASE
    // BASE/NAME
    // BASE/PLATFORM
    // BASE/PATH
    // INCLUDES
    // INCLUDES/ITEM_0
    // INCLUDES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VARIABLES
    // INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE
    // PARAMETERS
    // PARAMETERS/NETWORKING
    // PARAMETERS/NETWORKING/ID
    // PARAMETERS/NETWORKING/ACCEPT_PORT
    // PARAMETERS/NETWORKING/KEEP_ALIVE_SEC
    // PARAMETERS/NETWORKING/TIME_CLIENT
    // PARAMETERS/NETWORKING/NETWORK_THREADS
    // PARAMETERS/NETWORKING/BROADCAST_THREADS
    // PARAMETERS/NETWORKING/CLIENTS_THREADS
    // PARAMETERS/NETWORKING/NOTIFY_READY_CLIENTS
    // PARAMETERS/NETWORKING/NOTIFY_READY_SERVERS
    // PARAMETERS/NETWORKING/CONNECT
    // PARAMETERS/NETWORKING/CONNECT/PORT
    // PARAMETERS/NETWORKING/CONNECT/IP
    // PARAMETERS/LOG
    // PARAMETERS/LOG/LOGGING_LEVEL
    // PARAMETERS/LOG/TOTAL_LOG_LIMIT_MB
    // PARAMETERS/LOG/LOG_DIR
    // EDITOR
    // EDITOR/COLOR
    
    {
        CubesUnitTypes::ParameterModel base_group;
        base_group.id = ids_.base;
        base_group.name = QString::fromLocal8Bit("Базовые");
        base_group.value = QVariant();
        //base_group.valueType = "none";
        //base_group.parameterInfoId = "";
        base_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        base_group.editorSettings.is_expanded = true;

        CubesUnitTypes::ParameterModel name;
        name.id = ids_.base + ids_.name;
        name.name = QString::fromLocal8Bit("Имя");
        //name.value = QString::fromLocal8Bit("АРМ");
        if (xmlFile == nullptr || xmlFile->name.isEmpty())
            name.value = QString::fromLocal8Bit("АРМ_%1").arg(fileId_);
        else
            name.value = QString(xmlFile->name);
        //name.valueType = "string";
        //name.parameterInfoId = "";
        name.editorSettings.type = CubesUnitTypes::EditorType::String;
        name.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(name));

        CubesUnitTypes::ParameterModel platform;
        platform.id = ids_.base + ids_.platform;
        platform.name = QString::fromLocal8Bit("Платформа");
        platform.value = QString();
        //platform.valueType = "string";
        //platform.parameterInfoId = "";
        platform.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        platform.editorSettings.is_expanded = false;
        //for (const auto& pl : CubesUnitTypes::platform_names_)
        for (int i = 0; i < CubesUnitTypes::platform_names_.size(); ++i)
            platform.editorSettings.ComboBoxValues[i] = QString::fromStdString(CubesUnitTypes::platform_names_[i]);
        if (CubesUnitTypes::platform_names_.size() > 0)
        {
            
            if (xmlFile == nullptr || xmlFile->name.isEmpty())
                platform.value = QString::fromStdString(CubesUnitTypes::platform_names_[0]);
            else
                platform.value = QString(xmlFile->platform);
        }
        base_group.parameters.push_back(std::move(platform));

        CubesUnitTypes::ParameterModel file_path;
        file_path.id = ids_.base + ids_.path;
        file_path.name = QString::fromLocal8Bit("Имя файла");
        //file_path.value = QString::fromLocal8Bit("config.xml");
        if (xmlFile == nullptr || xmlFile->fileName.isEmpty())
            file_path.value = QString::fromLocal8Bit("config.xml");
        else
            file_path.value = QString(xmlFile->fileName);
        //file_path.valueType = "string";
        //file_path.parameterInfoId = "";
        file_path.editorSettings.type = CubesUnitTypes::EditorType::String;
        file_path.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(file_path));
        model_.parameters.push_back(std::move(base_group));
    }

    {
        CubesUnitTypes::ParameterModel includes;
        includes.id = ids_.includes;
        includes.name = QString::fromLocal8Bit("Включаемые файлы");
        includes.value = int{ 0 };
        includes.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
        includes.editorSettings.is_expanded = true;
        includes.editorSettings.SpinIntergerMin = 0;
        includes.editorSettings.SpinIntergerMax = 1000; // !!! TODO: make a define for a const

        //model_.parameters.push_back(std::move(includes));

        if (xmlFile != nullptr)
        {
            auto xmlCount = xmlFile->includes.size();
            includes.value = QString("%1").arg(xmlCount);

            UpdateIncludesArrayModel(xmlFile, includes, xmlCount);
        }

        //CubesUnitTypes::ParameterModel pm;
        //CreateParameterModel(ArrayType::Includes, ids_.includesGroupName, xmlFile, pm);
        //includes.parameters.push_back(std::move(pm));

        model_.parameters.push_back(std::move(includes));
    }

    {
        CubesUnitTypes::ParameterModel properties_group;
        properties_group.id = ids_.parameters;
        properties_group.name = QString::fromLocal8Bit("Параметры");
        properties_group.value = QVariant();
        //properties_group.valueType = "none";
        properties_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        properties_group.editorSettings.is_expanded = false;
        
        {
            CubesUnitTypes::ParameterModel pm_networking;
            pm_networking.id = ids_.parameters + ids_.networking;
            pm_networking.name = QString::fromLocal8Bit("Соединение");
            pm_networking.value = QVariant();
            //pm_networking.valueType = "none";
            pm_networking.editorSettings.type = CubesUnitTypes::EditorType::None;
            properties_group.editorSettings.is_expanded = true;

            CubesUnitTypes::ParameterModel pm_id;
            pm_id.id = ids_.parameters + ids_.networking + ids_.id;
            pm_id.name = QString::fromLocal8Bit("Идентифиикатор");
            //pm_id.value = int{ CubesXml::Networking::Defaults().id };
            //pm_id.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_id.value = int{ CubesXml::Networking::Defaults().id};
            else
                pm_id.value = int{ xmlFile->config.networking.id };
            pm_id.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_id.editorSettings.SpinIntergerMin = 0;
            pm_id.editorSettings.SpinIntergerMax = std::numeric_limits<int>::max();
            pm_networking.parameters.push_back(std::move(pm_id));

            CubesUnitTypes::ParameterModel pm_accept_port;
            pm_accept_port.id = ids_.parameters + ids_.networking + ids_.acceptPort;
            pm_accept_port.name = QString::fromLocal8Bit("Порт");
            //pm_accept_port.value = int{ CubesXml::Networking::Defaults().acceptPort };
            //pm_accept_port.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_accept_port.value = int{ CubesXml::Networking::Defaults().acceptPort };
            else
                pm_accept_port.value = int{ xmlFile->config.networking.acceptPort };
            pm_accept_port.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_accept_port.editorSettings.SpinIntergerMin = 1000;
            pm_accept_port.editorSettings.SpinIntergerMax = 65535;
            pm_networking.parameters.push_back(std::move(pm_accept_port));

            CubesUnitTypes::ParameterModel pm_keep_alive_sec;
            pm_keep_alive_sec.id = ids_.parameters + ids_.networking + ids_.keepAliveSec;
            pm_keep_alive_sec.name = QString::fromLocal8Bit("Таймаут");
            //pm_keep_alive_sec.value = int{ CubesXml::Networking::Defaults().keepAliveSec };
            //pm_keep_alive_sec.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_keep_alive_sec.value = int{ CubesXml::Networking::Defaults().keepAliveSec };
            else
                pm_keep_alive_sec.value = int{ xmlFile->config.networking.keepAliveSec };
            pm_keep_alive_sec.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_keep_alive_sec.editorSettings.SpinIntergerMin = 0;
            pm_keep_alive_sec.editorSettings.SpinIntergerMax = std::numeric_limits<int>::max();
            pm_networking.parameters.push_back(std::move(pm_keep_alive_sec));

            CubesUnitTypes::ParameterModel pm_time_client;
            pm_time_client.id = ids_.parameters + ids_.networking + ids_.timeClient;
            pm_time_client.name = QString::fromLocal8Bit("Получать время");
            //pm_time_client.value = bool{ false };
            //pm_time_client.valueType = "bool";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_time_client.value = bool{ CubesXml::Networking::Defaults().timeClient };
            else
                pm_time_client.value = bool{ xmlFile->config.networking.timeClient };
            pm_time_client.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_time_client));

            CubesUnitTypes::ParameterModel pm_network_threads;
            pm_network_threads.id = ids_.parameters + ids_.networking + ids_.networkThreads;
            pm_network_threads.name = QString::fromLocal8Bit("Сетевых потоков");
            //pm_network_threads.value = int{ CubesXml::Networking::Defaults().networkThreads };
            //pm_network_threads.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_network_threads.value = int{ CubesXml::Networking::Defaults().networkThreads };
            else
                pm_network_threads.value = int{ xmlFile->config.networking.networkThreads };
            pm_network_threads.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_network_threads.editorSettings.SpinIntergerMin = 1;
            pm_network_threads.editorSettings.SpinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_network_threads));

            CubesUnitTypes::ParameterModel pm_broadcast_threads;
            pm_broadcast_threads.id = ids_.parameters + ids_.networking + ids_.broadcastThreads;
            pm_broadcast_threads.name = QString::fromLocal8Bit("Широковещательных потоков");
            pm_broadcast_threads.value = int{ CubesXml::Networking::Defaults().broadcastThreads };
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_broadcast_threads.value = int{ CubesXml::Networking::Defaults().broadcastThreads };
            else
                pm_broadcast_threads.value = int{ xmlFile->config.networking.broadcastThreads };
            //pm_broadcast_threads.valueType = "int";
            pm_broadcast_threads.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_broadcast_threads.editorSettings.SpinIntergerMin = 1;
            pm_broadcast_threads.editorSettings.SpinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_broadcast_threads));

            CubesUnitTypes::ParameterModel pm_client_threads;
            pm_client_threads.id = ids_.parameters + ids_.networking + ids_.clientsThreads;
            pm_client_threads.name = QString::fromLocal8Bit("Клиентских потоков");
            //pm_client_threads.value = int{ CubesXml::Networking::Defaults().clientsThreads };
            //pm_client_threads.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_client_threads.value = int{ CubesXml::Networking::Defaults().clientsThreads };
            else
                pm_client_threads.value = int{ xmlFile->config.networking.clientsThreads };
            pm_client_threads.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_client_threads.editorSettings.SpinIntergerMin = 1;
            pm_client_threads.editorSettings.SpinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_client_threads));

            CubesUnitTypes::ParameterModel pm_notify_ready_client;
            pm_notify_ready_client.id = ids_.parameters + ids_.networking + ids_.notifyReadyClients;
            pm_notify_ready_client.name = QString::fromLocal8Bit("Информировать клиента");
            //pm_notify_ready_client.value = bool{ CubesXml::Networking::Defaults().notifyReadyClients };
            //pm_notify_ready_client.valueType = "bool";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_notify_ready_client.value = bool{ CubesXml::Networking::Defaults().notifyReadyClients };
            else
                pm_notify_ready_client.value = bool{ xmlFile->config.networking.notifyReadyClients };
            pm_notify_ready_client.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_notify_ready_client));

            CubesUnitTypes::ParameterModel pm_notify_ready_server;
            pm_notify_ready_server.id = ids_.parameters + ids_.networking + ids_.notifyReadyServers;
            pm_notify_ready_server.name = QString::fromLocal8Bit("Информировать сервер");
            //pm_notify_ready_server.value = bool{ CubesXml::Networking::Defaults().notifyReadyServers };
            //pm_notify_ready_server.valueType = "bool";
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_notify_ready_server.value = bool{ CubesXml::Networking::Defaults().notifyReadyServers };
            else
                pm_notify_ready_server.value = bool{ xmlFile->config.networking.notifyReadyServers };
            pm_notify_ready_server.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_notify_ready_server));

            {
                CubesUnitTypes::ParameterModel connect;
                connect.id = ids_.parameters + ids_.networking + ids_.connect;
                connect.name = QString::fromLocal8Bit("Подключения");
                connect.value = int{ 0 };
                connect.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
                connect.editorSettings.is_expanded = true;
                connect.editorSettings.SpinIntergerMin = 0;
                connect.editorSettings.SpinIntergerMax = 1000; // !!! TODO: make a define for a const

                if (xmlFile != nullptr && xmlFile->config.networkingIsSet)
                {
                    int xmlCount = xmlFile->config.networking.connects.size();
                    connect.value = xmlCount;
                    UpdateConnectArrayModel(&xmlFile->config.networking, connect, xmlCount);
                }

                //int xmlCount = 0;
                //if (xmlFile != nullptr && xmlFile->config.networkingIsSet)
                //    xmlCount = xmlFile->config.networking.connects.size();
                //connect.value = QString("%1").arg(xmlCount);

                //UpdateConnectArrayModel(xmlFile, connect, xmlCount);


                //CubesUnitTypes::ParameterModel pm;
                //CreateParameterModel(ArrayType::Includes, ids_.includesGroupName, xmlFile, pm);
                //includes.parameters.push_back(std::move(pm));

                pm_networking.parameters.push_back(std::move(connect));
            }

            properties_group.parameters.push_back(std::move(pm_networking));
        }

        {
            CubesUnitTypes::ParameterModel pm_logging;
            pm_logging.id = ids_.parameters + ids_.log;
            pm_logging.name = QString::fromLocal8Bit("Логирование");
            pm_logging.value = QVariant();
            //pm_logging.valueType = "none";
            pm_logging.editorSettings.type = CubesUnitTypes::EditorType::None;
            properties_group.editorSettings.is_expanded = true;

            CubesUnitTypes::ParameterModel pm_logging_level;
            pm_logging_level.id = ids_.parameters + ids_.log + ids_.loggingLevel;
            pm_logging_level.name = QString::fromLocal8Bit("Уровень");
            //pm_logging_level.value = QString("TRACE");
            //pm_logging_level.valueType = "string";
            pm_logging_level.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
            const QStringList logLevels{ "LOG_TRACE", "LOG_DEBUG", "LOG_INFO" , "LOG_WARNING" , "LOG_ERROR" , "LOG_FATAL" };
            for (int i = 0; i < logLevels.size(); ++i)
                pm_logging_level.editorSettings.ComboBoxValues[i] = logLevels[i];
            //pm_logging_level.editorSettings.ComboBoxValues = QStringList{"LOG_TRACE", "LOG_DEBUG", "LOG_INFO" , "LOG_WARNING" , "LOG_ERROR" , "LOG_FATAL" };
            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_logging_level.value = pm_logging_level.editorSettings.ComboBoxValues[CubesXml::Log::Defaults().loggingLevel];
            else if (xmlFile->config.log.loggingLevel < pm_logging_level.editorSettings.ComboBoxValues.size())
                pm_logging_level.value = pm_logging_level.editorSettings.ComboBoxValues[xmlFile->config.log.loggingLevel];
            else
                pm_logging_level.value = pm_logging_level.editorSettings.ComboBoxValues[0];
            pm_logging.parameters.push_back(std::move(pm_logging_level));

            CubesUnitTypes::ParameterModel pm_log_limit;
            pm_log_limit.id = ids_.parameters + ids_.log + ids_.totalLogLimitMb;
            pm_log_limit.name = QString::fromLocal8Bit("Размер");
            //pm_log_limit.value = int{ 500 };
            //pm_log_limit.valueType = "int";
            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_log_limit.value = int{ CubesXml::Log::Defaults().totalLogLimit };
            else
                pm_log_limit.value = int{ xmlFile->config.log.totalLogLimit };
            pm_log_limit.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            pm_log_limit.editorSettings.SpinIntergerMin = 0;
            pm_log_limit.editorSettings.SpinIntergerMax = 1024 * 1024;
            pm_logging.parameters.push_back(std::move(pm_log_limit));

            CubesUnitTypes::ParameterModel pm_log_dir;
            pm_log_dir.id = ids_.parameters + ids_.log + ids_.logDir;
            pm_log_dir.name = QString::fromLocal8Bit("Директория");
            //pm_log_dir.value = QString();
            //pm_log_dir.valueType = "string";
            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_log_dir.value = CubesXml::Log::Defaults().logDir;
            else
                pm_log_dir.value = xmlFile->config.log.logDir;
            pm_log_dir.editorSettings.type = CubesUnitTypes::EditorType::String;
            pm_logging.parameters.push_back(std::move(pm_log_dir));

            properties_group.parameters.push_back(pm_logging);
        }

        model_.parameters.push_back(std::move(properties_group));
    }

    {
        CubesUnitTypes::ParameterModel editor_group;
        editor_group.id = ids_.editor;
        editor_group.name = QString::fromLocal8Bit("Редактор");
        editor_group.value = QVariant();
        //editor_group.valueType = "none";
        //editor_group.parameterInfoId = "";
        editor_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        editor_group.editorSettings.is_expanded = true;

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = ids_.editor + ids_.color;
            pm.name = QString::fromLocal8Bit("Цвет");

            // Можно запросить у менеджера, пока забил красный defaultColorsFile_[defaultColorFileIndex_++]
            QColor redColor = QColor("Red");
            redColor.setAlpha(32);
            QRgb color = redColor.rgba();
            if (xmlFile != nullptr && !xmlFile->color.isEmpty() && xmlFile->color.size() == 9)
            {
                bool ok = false;
                const QString rgba = xmlFile->color.mid(1);
                const unsigned int parsedValue = rgba.toUInt(&ok, 16);
                if (ok)
                    color = parsedValue;
            }
            pm.value = color;
            //pm.valueType = "string";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::Color;
            editor_group.parameters.push_back(std::move(pm));
        }

        model_.parameters.push_back(std::move(editor_group));
    }
}

void FileItem::CreateProperties()
{
    QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty;
    for (auto& pm : model_.parameters)
        topLevelProperties_.push_back(editor_->CreatePropertyForModel(pm, idToProperty));
    for (const auto& kvp : idToProperty.toStdMap())
        RegisterProperty(kvp.second, kvp.first);
}

//void FileItem::CreateParameterModel(const ArrayType arrayType, const QString& parentModelId,
//    const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model)
//{
//    // Создание модели для массива
//    // Каждому параметру назначается model ID (путь к параметру в модели, разделенный /)
//
//    //auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
//    //    parameterInfoId.type.toStdString(), parameterInfoId.name.toStdString());
//
//    CubesUnitTypes::ParameterModel pm;
//    pm.id = QString("%1/%2").arg(parentModelId, QString::fromStdString(pi.name));
//    pm.name = QString::fromStdString(parameters::helper::parameter::get_display_name(pi));
//    //pm.parameterInfoId = parameterInfoId;
//
//    FillArrayModel(xmlFile, pm);
//    UpdateArrayModel(xmlFile, pm);
//
//    //bool is_array = parameters::helper::common::get_is_array_type(pi.type);
//    //if (is_array)
//    //{
//    //    FillArrayModel(xmlFile, pm);
//    //    UpdateArrayModel(xmlFile, pm);
//    //}
//    //else
//    //{
//    //    FillParameterModel(xmlFile, pm, false);
//    //}
//
//    model = pm;
//
//}
//
//void FileItem::FillParameterModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model, bool isItem)
//{
//
//}
//
//void FileItem::FillArrayModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model)
//{
//    // Созадем модель для параметра, хранящего количество элементов массива
//    // Поля id, name, parameterInfoId должны быть предварительно заполнены
//    // Если xmlUnit != nullptr, значит создаем юнит из файла xml
//    // Если xmlUnit не задан - количество элементов равно нулю
//
//    int xmlCount = 0;
//    if (xmlFile != nullptr)
//        xmlCount = xmlFile->includes.size();
//
//    model.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
//    model.editorSettings.SpinIntergerMin = 0;
//    model.editorSettings.SpinIntergerMax = 1000; // !!! TODO: make a define for a const
//    model.value = QString("%1").arg(xmlCount);
//}
//
//void FileItem::UpdateArrayModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model)
//{
//
//}

void FileItem::ValueChanged(QtProperty* property, const QVariant& value)
{
    // BASE
    // BASE/NAME
    // BASE/PLATFORM
    // BASE/PATH
    // INCLUDES
    // INCLUDES/ITEM_0
    // INCLUDES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VARIABLES
    // INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE
    // PARAMETERS
    // PARAMETERS/NETWORKING
    // PARAMETERS/NETWORKING/ID
    // PARAMETERS/NETWORKING/ACCEPT_PORT
    // PARAMETERS/NETWORKING/KEEP_ALIVE_SEC
    // PARAMETERS/NETWORKING/TIME_CLIENT
    // PARAMETERS/NETWORKING/NETWORK_THREADS
    // PARAMETERS/NETWORKING/BROADCAST_THREADS
    // PARAMETERS/NETWORKING/CLIENTS_THREADS
    // PARAMETERS/NETWORKING/NOTIFY_READY_CLIENTS
    // PARAMETERS/NETWORKING/NOTIFY_READY_SERVERS
    // PARAMETERS/NETWORKING/CONNECT
    // PARAMETERS/NETWORKING/CONNECT/PORT
    // PARAMETERS/NETWORKING/CONNECT/IP
    // PARAMETERS/LOG
    // PARAMETERS/LOG/LOGGING_LEVEL
    // PARAMETERS/LOG/TOTAL_LOG_LIMIT_MB
    // PARAMETERS/LOG/LOG_DIR
    // EDITOR
    // EDITOR/COLOR

    qDebug() << "ValueChanged value = " << value;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith(ids_.base))
    {
        if (pm->id == ids_.base + ids_.name)
        {
            // Используется модифицированная версия редактора строк qtexpropertymanager
            // Обработка завершения редактирования строки происходит в StringEditingFinished
        }
        else if (pm->id == ids_.base + ids_.platform)
        {
            pm->value = property->valueText();
        }
        else
            pm->value = value.toString();
    }
    else if (pm->id.startsWith(ids_.includes))
    {
        if (pm->id.size() > 1 && ids_.IsItem(pm->id.mid(1, 1)) && pm->id.endsWith(ids_.name))
        {
            // Используется модифицированная версия редактора строк qtexpropertymanager
            // Обработка завершения редактирования строки происходит в StringEditingFinished
        }
        else if ((pm->id == ids_.includes) ||
            (pm->id.size() > 1 && ids_.IsItem(pm->id.mid(1, 1)) && pm->id.endsWith(ids_.variables)))
        {
            int count = value.toInt();
            pm->value = count;

            if (pm->id == ids_.includes)
                UpdateIncludesArrayModel(nullptr, *pm, count);
            else
                UpdateVariablesArrayModel(nullptr, *pm, count);
            editor_->SetIntValue(property, count);

            QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty;
            for (int i = property->subProperties().size(); i < count; ++i)
                property->addSubProperty(editor_->CreatePropertyForModel(pm->parameters[i], idToProperty));
            for (const auto& kvp : idToProperty.toStdMap())
                RegisterProperty(kvp.second, kvp.first);

            auto collect = [](QtProperty* property, QList<QtProperty*>& list, auto&& collect) -> void {
                list.push_back(property);
                for (const auto& p : property->subProperties())
                    collect(p, list, collect);
            };

            QList<QtProperty*> toRemove;
            QList<QtProperty*> toUnregister;
            const auto& subProperties = property->subProperties();
            for (int i = count; i < subProperties.size(); ++i)
            {
                collect(subProperties[i], toUnregister, collect);
                toRemove.push_back(subProperties[i]);
            }

            for (auto& p : toRemove)
                property->removeSubProperty(p);

            for (auto& p : toUnregister)
            {
                UnregisterProperty(p);
                delete p;
            }

            ApplyExpandState();
        }
        else
            pm->value = property->valueText();
    }
    else if (pm->id.startsWith(ids_.parameters))
    {
        if (pm->id == ids_.parameters + ids_.networking + ids_.connect)
        {
            int count = value.toInt();

            UpdateConnectArrayModel(nullptr, *pm, count);
            pm->value = count;
            editor_->SetIntValue(property, count);

            QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty;
            for (int i = property->subProperties().size(); i < count; ++i)
                property->addSubProperty(editor_->CreatePropertyForModel(pm->parameters[i], idToProperty));
            for (const auto& kvp : idToProperty.toStdMap())
                RegisterProperty(kvp.second, kvp.first);

            auto collect = [](QtProperty* property, QList<QtProperty*>& list, auto&& collect) -> void {
                list.push_back(property);
                for (const auto& p : property->subProperties())
                    collect(p, list, collect);
            };

            QList<QtProperty*> toRemove;
            QList<QtProperty*> toUnregister;
            const auto& subProperties = property->subProperties();
            for (int i = count; i < subProperties.size(); ++i)
            {
                collect(subProperties[i], toUnregister, collect);
                toRemove.push_back(subProperties[i]);
            }

            for (auto& p : toRemove)
                property->removeSubProperty(p);

            for (auto& p : toUnregister)
            {
                UnregisterProperty(p);
                delete p;
            }

            ApplyExpandState();
        }
        else
            pm->value = value.toString();
    }
    else if (pm->id.startsWith(ids_.editor))
    {
        if (pm->id == ids_.editor + ids_.color)
        {
            pm->value = value.toInt();
            fileItemsManager_->AfterColorChanged(fileId_, QColor::fromRgba(value.toInt()));
        }
        else
            pm->value = value.toString();
    }
}

void FileItem::StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue)
{
    // Нет необходимости использовать if (notifyManager_) для BeforeFileNameChanged и
    // AfterFileNameChanged, так как в эту функцию можно попасть только с GUI

    qDebug() << "StringEditingFinished value = " << value << ", oldValue = " << oldValue;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id == ids_.base + ids_.name)
    {
        fileItemsManager_->AfterFileNameChanged(fileId_);

        //bool cancel = false;
        //fileItemsManager_->BeforeFileNameChanged(fileId_, oldValue, cancel);

        //if (!cancel)
        //{
        //    QString oldName = pm->value.toString();
        //    pm->value = value;
        //    fileItemsManager_->AfterFileNameChanged(fileId_, oldName);
        //}
        //else
        //{
        //    // Отмена
        //    editor_->SetStringValue(property, oldValue);
        //}
    }
    else if (pm->id.startsWith(ids_.includes) && pm->id.size() == 3 && ids_.IsItem(pm->id.mid(1, 1)) && pm->id.endsWith(ids_.name))
    {
        // INCLUDES/ITEM_0/NAME
        // INCLUDES/ITEM_0/VARIABLES
        // INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
        // INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE
    
        fileItemsManager_->AfterIncludeNameChanged(fileId_, pm->key.toInt());
        //bool cancel = false;
        //fileItemsManager_->BeforeIncludeNameChanged(fileId_, value, oldValue, cancel);

        //if (!cancel)
        //{
        //    QString oldName = pm->value.toString();
        //    pm->value = value;
        //    fileItemsManager_->AfterIncludeNameChanged(fileId_, value, oldName);
        //}
        //else
        //{
        //    // Отмена
        //    editor_->SetStringValue(property, oldValue);
        //}
    }
    else if (pm->id.startsWith(ids_.includes) && pm->id.size() == 5 && ids_.IsItem(pm->id.mid(1, 1)) && 
        pm->id.mid(2, 1) == ids_.variables && ids_.IsItem(pm->id.mid(3, 1)) && pm->id.endsWith(ids_.name))
    {
        // INCLUDES/ITEM_0/NAME
        // INCLUDES/ITEM_0/VARIABLES
        // INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
        // INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE

        //CubesUnitTypes::ParameterModelId parameterName = pm->id.left(pm->id.indexOf(ids_.variables)) + ids_.name;
        //const auto pmIncludesName = GetParameterModel(parameterName);
        //QString includesName = pmIncludesName->value.toString();
        //QString oldName = pm->value.toString();
        //pm->value = value;
        //fileItemsManager_->AfterVariableNameChanged(fileId_, includesName, value, oldName);



        // TODO: Пока возвращаю индекс+1, это фактически совпадает с includeFileId, 
        // но по честному надо доставать id из editorSettings модели INCLUDES
        //const auto includeFileId = ids_.ItemIndex(pm->id.left(pm->id.indexOf(ids_.variables))) + 1;
        const auto pmItem = GetParameterModel(pm->id.left(2));
        const auto includeId = pmItem->key.toInt();

        QString oldName = pm->value.toString();
        pm->value = value;
        fileItemsManager_->AfterVariableNameChanged(fileId_, includeId, value, oldName);
    }
    else if (pm->id.startsWith(ids_.includes) && pm->id.size() == 5 && ids_.IsItem(pm->id.mid(1, 1)) &&
        pm->id.mid(2, 1) == ids_.variables && ids_.IsItem(pm->id.mid(3, 1)) && pm->id.endsWith(ids_.value))
    {
        //CubesUnitTypes::ParameterModelId parameterName = pm->id.left(pm->id.indexOf(ids_.variables)) + ids_.name;
        //const auto pmIncludesName = GetParameterModel(parameterName);
        //QString includesName = pmIncludesName->value.toString();
        //QList<QPair<QString, QString>> variables = GetIncludeVariables(includesName);
        //fileItemsManager_->AfterVariablesListChanged(fileId_, includesName, variables);

        // TODO: Пока возвращаю индекс+1, это фактически совпадает с includeFileId, 
        // но по честному надо доставать id из editorSettings модели INCLUDES
        //const auto includeFileId = ids_.ItemIndex(pm->id.left(pm->id.indexOf(ids_.variables))) + 1;
        const auto pmItem = GetParameterModel(pm->id.left(2));
        const auto includeId = pmItem->key.toInt();
        
        QString oldName = pm->value.toString();
        pm->value = value;
        const auto variables = GetIncludeVariables(includeId);
        fileItemsManager_->AfterVariablesListChanged(fileId_, includeId, variables);
    }
}

void FileItem::Select()
{
    qDebug() << connect(editor_, &PropertiesEditor::ValueChanged, this, &FileItem::ValueChanged);
    qDebug() << connect(editor_, &PropertiesEditor::StringEditingFinished, this, &FileItem::StringEditingFinished);

    auto pe = editor_->GetPropertyEditor();
    pe->clear();
    ignoreEvents_ = true;
    for (auto& pr : topLevelProperties_)
        pe->addProperty(pr);
    ignoreEvents_ = false;

    ApplyExpandState();
}

void FileItem::UnSelect()
{
    qDebug() << disconnect(editor_, &PropertiesEditor::ValueChanged, this, &FileItem::ValueChanged);
    qDebug() << disconnect(editor_, &PropertiesEditor::StringEditingFinished, this, &FileItem::StringEditingFinished);
}

QString FileItem::GetName()
{
    auto pm = GetParameterModel(ids_.base + ids_.name);
    if (pm == nullptr)
        return "";

    return pm->value.toString();
};

QColor FileItem::GetColor()
{
    auto pm = GetParameterModel(ids_.editor + ids_.color);
    if (pm == nullptr)
        return "";

    return QColor::fromRgba(pm->value.toInt());
};

QString FileItem::GetPropertyDescription(const QtProperty* property)
{
    QString id = GetPropertyId(property).toString();
    return id;
}

void FileItem::ExpandedChanged(const QtProperty* property, bool is_expanded)
{
    if (!ignoreEvents_)
    {
        auto pm = GetParameterModel(property);
        if (pm != nullptr)
            pm->editorSettings.is_expanded = is_expanded;
    }
}

void FileItem::SetName(QString name, bool setOldName, QString oldName)
{
    auto pm = GetParameterModel(ids_.base + ids_.name);
    pm->value = name;

    auto pr = GetProperty(pm->id);
    editor_->SetStringValue(pr, name, setOldName, oldName);
}

void FileItem::SetPlatform(QString platform)
{
    auto pm = GetParameterModel(ids_.base + ids_.platform);
    pm->value = QString(platform);

    auto pr = GetProperty(pm->id);
    editor_->SetEnumValue(pr, pm->value);
}

void FileItem::SetPath(QString name, bool setOldName, QString oldName)
{
    auto pm = GetParameterModel(ids_.base + ids_.path);
    pm->value = name;

    auto pr = GetProperty(pm->id);
    editor_->SetStringValue(pr, name, setOldName, oldName);
}

void FileItem::SetColor(QColor color)
{
    auto pm = GetParameterModel(ids_.editor + ids_.color);
    pm->value = color.rgba();

    auto pr = GetProperty(pm->id);
    editor_->SetColorValue(pr, color);
}

void FileItem::AddInclude(const CubesUnitTypes::IncludeId includeId, const CubesUnitTypes::VariableIdVariables& variables)
{
    auto pmi = GetParameterModel(ids_.includes);
    int ci = pmi->value.toInt() + 1;

    auto pri = GetProperty(pmi->id);
    editor_->SetIntValue(pri, ci);
    // Установка количества элементов в Property Browser вызывает операцию по добавлению
    // необходимого количества заготовок через ValueChanged

    auto pmin = GetParameterModel(ids_.includes + ids_.Item(ci - 1) + ids_.name);
    pmin->value = QString::fromLocal8Bit("Включение%1").arg(ci);

    auto prin = GetProperty(pmin->id);
    editor_->SetStringValue(prin, pmin->value.toString());

    const auto includeName = GetIncludeName(includeId);

    auto pmifn = GetParameterModel(ids_.includes + ids_.Item(ci - 1) + ids_.filePath);
    pmifn->key = includeId;
    pmifn->value = includeName;

    auto prifn = GetProperty(pmifn->id);
    editor_->SetStringValue(prifn, includeName);

    auto pmiv = GetParameterModel(ids_.includes + ids_.Item(ci - 1) + ids_.variables);

    auto priv = GetProperty(pmiv->id);
    editor_->SetIntValue(priv, variables.size());
    // Установка количества элементов в Property Browser вызывает операцию по добавлению
    // необходимого количества заготовок через ValueChanged

    for (int i = 0; i < variables.values().size(); i++)
    {
        auto& v = variables.values().at(i);
        auto pmivn = GetParameterModel(ids_.includes + ids_.Item(ci - 1) + ids_.variables + ids_.Item(i) + ids_.name);
        pmivn->value = v.first;

        auto privn = GetProperty(pmivn->id);
        editor_->SetStringValue(privn, v.first);

        auto pmivv = GetParameterModel(ids_.includes + ids_.Item(ci - 1) + ids_.variables + ids_.Item(i) + ids_.value);
        pmivv->value = v.second;

        auto privv = GetProperty(pmivv->id);
        editor_->SetStringValue(privv, v.second);
    }
}

CubesUnitTypes::IncludeIdNames FileItem::GetIncludes()
{
    CubesUnitTypes::IncludeIdNames includeNamesMap;

    const auto pm = GetParameterModel(ids_.includes);
    if (pm == nullptr)
        return includeNamesMap;

    int index = 1;
    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmi = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.name);
        includeNamesMap[index++] = pmi->value.toString();
    }

    return includeNamesMap;
}

CubesUnitTypes::VariableIdVariables FileItem::GetIncludeVariables(const CubesUnitTypes::IncludeId includeId)
{
    CubesUnitTypes::VariableIdVariables result;

    const auto pm = GetParameterModel(ids_.includes);
    if (pm == nullptr)
        return result;

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmi = GetParameterModel(ids_.includes + ids_.Item(i));
        if (pmi->key.toInt() == includeId)
        {
            const auto pmiv = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.variables);
            for (int j = 0; j < pmiv->value.toInt(); j++)
            {
                auto pmiv = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.variables + ids_.Item(j));
                auto pmivn = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.variables + ids_.Item(j) + ids_.name);
                auto pmivv = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.variables + ids_.Item(j) + ids_.value);
                result[pmiv->key.toInt()] = { pmivn->value.toString(), pmivv->value.toString() };
            }
            break;
        }
    }

    return result;
}

QString FileItem::GetIncludeName(const QString& includePath)
{
    const auto pm = GetParameterModel(ids_.includes);
    if (pm == nullptr)
        return "";

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmif = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.filePath);
        if (pmif->value == includePath)
        {
            const auto pmin = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.name);
            return pmin->value.toString();
        }
    }
    return "";
}

QString FileItem::GetIncludeName(const CubesUnitTypes::IncludeId includeId)
{
    const auto pm = GetParameterModel(ids_.includes);
    if (pm == nullptr)
        return "";

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmin = GetParameterModel(ids_.includes + ids_.Item(i));
        if (pmin->key.toInt() == includeId)
        {
            const auto pmif = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.name);
            return pmif->value.toString();
        }
    }

    return "";
}

QString FileItem::GetIncludePath(const CubesUnitTypes::IncludeId includeId)
{
    const auto pm = GetParameterModel(ids_.includes);
    if (pm == nullptr)
        return "";

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmin = GetParameterModel(ids_.includes + ids_.Item(i));
        if (pmin->key.toInt() == includeId)
        {
            const auto pmif = GetParameterModel(ids_.includes + ids_.Item(i) + ids_.filePath);
            return pmif->value.toString();
        }
    }
    return "";
}

File FileItem::GetFile()
{
    File result{};
    result.name = GetParameterModel(ids_.base + ids_.name)->value.toString();
    result.platform = GetParameterModel(ids_.base + ids_.platform)->value.toString();
    result.path = GetParameterModel(ids_.base + ids_.path)->value.toString();

    result.network.id = GetParameterModel(ids_.parameters + ids_.networking + ids_.id)->value.toInt();
    result.network.accept_port = GetParameterModel(ids_.parameters + ids_.networking + ids_.acceptPort)->value.toInt();
    result.network.keep_alive_sec = GetParameterModel(ids_.parameters + ids_.networking + ids_.keepAliveSec)->value.toInt();
    result.network.time_client = GetParameterModel(ids_.parameters + ids_.networking + ids_.timeClient)->value.toBool();
    result.network.network_threads = GetParameterModel(ids_.parameters + ids_.networking + ids_.networkThreads)->value.toInt();
    result.network.broadcast_threads = GetParameterModel(ids_.parameters + ids_.networking + ids_.broadcastThreads)->value.toInt();
    result.network.clients_threads = GetParameterModel(ids_.parameters + ids_.networking + ids_.clientsThreads)->value.toInt();
    result.network.notify_ready_clients = GetParameterModel(ids_.parameters + ids_.networking + ids_.notifyReadyClients)->value.toBool();
    result.network.notify_ready_servers = GetParameterModel(ids_.parameters + ids_.networking + ids_.notifyReadyServers)->value.toBool();

    int count = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect)->value.toInt();
    for (int i = 0; i < count; i++)
    {
        Connect connect{};
        connect.port = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.port)->value.toInt();
        connect.ip = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.ip)->value.toString();
        result.network.connect.push_back(connect);
    }

    result.log.level = static_cast<LoggingLevel>(GetParameterModel(ids_.parameters + ids_.log + ids_.loggingLevel)->value.toInt());
    result.log.limit_mb = GetParameterModel(ids_.parameters + ids_.log + ids_.totalLogLimitMb)->value.toInt();
    result.log.directory_path = GetParameterModel(ids_.parameters + ids_.log + ids_.logDir)->value.toString();

    const auto includes = GetIncludes();
    for (const auto& kvp : includes.toStdMap())
    {
        Include include{};
        include.name = kvp.second;
        include.path = GetIncludePath(kvp.first);
        const auto variables = GetIncludeVariables(kvp.first);
        for(const auto& v : variables.values())
            include.variables.push_back(v);
        result.includes.push_back(include);
    }

    return result;
}

CubesXml::File FileItem::GetXmlFile()
{
    CubesXml::File result{};
    result.name = GetParameterModel(ids_.base + ids_.name)->value.toString();
    result.platform = GetParameterModel(ids_.base + ids_.platform)->value.toString();
    result.fileName = GetParameterModel(ids_.base + ids_.path)->value.toString();
    QColor color = QColor::fromRgba(GetParameterModel(ids_.editor + ids_.color)->value.toUInt());
    result.color = QString("#%1").arg(color.rgba(), 8, 16, QLatin1Char('0'));

    result.config.networkingIsSet = true;
    result.config.networking.id = GetParameterModel(ids_.parameters + ids_.networking + ids_.id)->value.toInt();
    result.config.networking.acceptPort = GetParameterModel(ids_.parameters + ids_.networking + ids_.acceptPort)->value.toInt();
    result.config.networking.keepAliveSec = GetParameterModel(ids_.parameters + ids_.networking + ids_.keepAliveSec)->value.toInt();
    result.config.networking.timeClient = GetParameterModel(ids_.parameters + ids_.networking + ids_.timeClient)->value.toBool();
    result.config.networking.networkThreads = GetParameterModel(ids_.parameters + ids_.networking + ids_.networkThreads)->value.toInt();
    result.config.networking.broadcastThreads = GetParameterModel(ids_.parameters + ids_.networking + ids_.broadcastThreads)->value.toInt();
    result.config.networking.clientsThreads = GetParameterModel(ids_.parameters + ids_.networking + ids_.clientsThreads)->value.toInt();
    result.config.networking.notifyReadyClients = GetParameterModel(ids_.parameters + ids_.networking + ids_.notifyReadyClients)->value.toBool();
    result.config.networking.notifyReadyServers = GetParameterModel(ids_.parameters + ids_.networking + ids_.notifyReadyServers)->value.toBool();

    int count = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect)->value.toInt();
    for (int i = 0; i < count; i++)
    {
        CubesXml::Connect connect{};
        connect.port = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.port)->value.toInt();
        connect.ip = GetParameterModel(ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.ip)->value.toString();
        result.config.networking.connects.push_back(connect);
    }

    result.config.logIsSet = true;
    result.config.log.loggingLevel = static_cast<LoggingLevel>(GetParameterModel(ids_.parameters + ids_.log + ids_.loggingLevel)->value.toInt());
    result.config.log.totalLogLimit = GetParameterModel(ids_.parameters + ids_.log + ids_.totalLogLimitMb)->value.toInt();
    result.config.log.logDir = GetParameterModel(ids_.parameters + ids_.log + ids_.logDir)->value.toString();

    const auto includes = GetIncludes();
    for (const auto& kvp : includes.toStdMap())
    {
        CubesXml::Include include{};
        include.name = kvp.second;
        include.fileName = GetIncludePath(kvp.first);
        const auto variables = GetIncludeVariables(kvp.first);
        for (const auto& v : variables.values())
            include.variables.push_back(v);
        result.includes.push_back(include);
    }

    return result;
}

void FileItem::UpdateIncludesArrayModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model, int& count)
{
    // Сначала добавляем
    if (model.parameters.size() < count)
    {
        // Получаем список имеющихся имен включаемых файлов
        QStringList includeNames;
        for (const auto& i : model.parameters)
        {
            for (const auto& si : i.parameters)
            {
                if (si.id.endsWith(ids_.name))
                {
                    includeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        // Получаем список новых имен файлов
        QStringList addingIncludeNames;
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Получаем уникальное имя
            QString includeName;
            if (xmlFile != nullptr && i < xmlFile->includes.size() && !xmlFile->includes[i].name.isEmpty())
                includeName = xmlFile->includes[i].name;
            else
                includeName = CubesUnitTypes::GetUniqueName(QString::fromLocal8Bit("Файл"), " ", includeNames);

            // Добавляем в списки
            addingIncludeNames.push_back(includeName);
            includeNames.push_back(includeName);
        }

        // Проверяем, что можно создавать
        //bool cancel = false;
        //fileItemsManager_->BeforeIncludesAdd(fileId_, addingIncludeNames, cancel);
        //if (cancel)
        //{
        //    // Не сделали все, что просили. Возвращаем count, равный фактическому количеству элементов
        //    count = model.parameters.count();
        //    return;
        //}

        // Добавляем
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Получаем уникальное имя
            QString includeName = includeNames[i];

            // Создаем
            CubesUnitTypes::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            //group_model.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.key = ++uniqueNumber_;

            CubesUnitTypes::ParameterModel name;
            name.editorSettings.type = CubesUnitTypes::EditorType::String;
            name.id = group_model.id + ids_.name;
            //name.id = QString("%1/%2").arg(group_model.id, ids_.name);
            name.name = QString::fromLocal8Bit("Имя");
            name.value = includeName;
            //name.valueType = "string";
            //if (xmlFile != nullptr && i < xmlFile->includes.size())
            //    name.value = xmlFile->includes[i].name;
            //else
            //    name.value = includeName;
            name.editorSettings.type = CubesUnitTypes::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            CubesUnitTypes::ParameterModel file_path;
            file_path.editorSettings.type = CubesUnitTypes::EditorType::String;
            file_path.id = group_model.id + ids_.filePath;
            //file_path.id = QString("%1/%2").arg(group_model.id, ids_.filePath);
            file_path.name = QString::fromLocal8Bit("Имя файла");
            //file_path.value = QString::fromLocal8Bit("include.xml");
            //file_path.valueType = "string";
            if (xmlFile != nullptr && i < xmlFile->includes.size())
                file_path.value = xmlFile->includes[i].fileName;
            else
                file_path.value = QString::fromLocal8Bit("include.xml");;
            file_path.editorSettings.type = CubesUnitTypes::EditorType::String;
            file_path.editorSettings.is_expanded = false;
            group_model.parameters.push_back(std::move(file_path));

            //model.parameters.push_back(std::move(group_model));

            {
                CubesUnitTypes::ParameterModel variables;
                variables.id = group_model.id + ids_.variables;
                //variables.id = QString("%1/%2").arg(group_model.id, ids_.variablesGroupName);
                variables.name = QString::fromLocal8Bit("Переменные");
                //variables.value = int{ 0 };
                variables.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
                variables.editorSettings.is_expanded = true;
                variables.editorSettings.SpinIntergerMin = 0;
                variables.editorSettings.SpinIntergerMax = 1000; // !!! TODO: make a define for a const

                //group_model.parameters.push_back(std::move(variables));

                if (xmlFile != nullptr && i < xmlFile->includes.size())
                {
                    int xmlCount = xmlFile->includes[i].variables.size();
                    variables.value = xmlCount;
                    UpdateVariablesArrayModel(&xmlFile->includes[i], variables, xmlCount);
                }

                //int xmlCount = 0;
                //if (xmlFile != nullptr)
                //{
                //    for (const auto& i : xmlFile->includes)
                //    {
                //        if (i.name == includeName)
                //        {
                //            xmlCount = i.variables.size();
                //            break;
                //        }
                //    }
                //    xmlCount = xmlFile->includes.size();
                //}
                //variables.value = xmlCount;

                //UpdateVariablesArrayModel(xmlFile, variables, xmlCount);


                //CubesUnitTypes::ParameterModel pm;
                //CreateParameterModel(ArrayType::Includes, ids_.includesGroupName, xmlFile, pm);
                //includes.parameters.push_back(std::move(pm));

                group_model.parameters.push_back(std::move(variables));
            }

            model.parameters.push_back(std::move(group_model));
        }

        //CubesUnitTypes::IncludeFileIdNames includeNamesMap;
        //for (const auto& pm : model.parameters)
        //{
        //    for (const auto& si : pm.parameters)
        //    {
        //        if (si.id.endsWith(ids_.name))
        //        {
        //            includeNamesMap[pm.key.toInt()] = si.value.toString();
        //            break;
        //        }
        //    }
        //}

        ////CubesUnitTypes::IncludeFileIdNames includeNamesMap;
        ////int index = 1;
        ////for (const auto& in : includeNames)
        ////    includeNamesMap[index++] = in;

        //// Информируем, что создали
        //if (notifyManager_)
        //    fileItemsManager_->AfterIncludesListChanged(fileId_, includeNamesMap);
    }

    // Теперь удаляем
    if (model.parameters.size() > count)
    {
        // Получаем имена удаляемых включаемых файлов
        QStringList removingIncludeNames;
        for (int i = count; i < model.parameters.count(); ++i)
        {
            for (const auto& si : model.parameters[i].parameters)
            {
                if (si.id.endsWith(ids_.name))
                {
                    removingIncludeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        // Проверяем, что можно удалять
        //bool cancel = false;
        //fileItemsManager_->BeforeIncludesRemoved(fileId_, removingIncludeNames, cancel);
        //if (cancel)
        //{
        //    // Не сделали все, что просили. Возвращаем count, равный фактическому количеству элементов
        //    count = model.parameters.size();
        //    return;
        //}

        // Удаляем
        while (model.parameters.size() > count)
        {
            // Удаляем параметр
            model.parameters.pop_back();
        }

        // Получаем список имеющихся имен включаемых файлов
        QStringList includeNames;
        for (const auto& i : model.parameters)
        {
            for (const auto& si : i.parameters)
            {
                if (si.id.endsWith(ids_.name))
                {
                    includeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        //CubesUnitTypes::IncludeFileIdNames includeNamesMap;
        //int index = 1;
        //for (const auto& in : includeNames)
        //    includeNamesMap[index++] = in;

        //// Информируем, что удалили
        //if (notifyManager_)
        //    fileItemsManager_->AfterIncludesListChanged(fileId_, includeNamesMap);
    }

    CubesUnitTypes::IncludeIdNames includeNamesMap;
    for (const auto& pm : model.parameters)
    {
        for (const auto& si : pm.parameters)
        {
            if (si.id.endsWith(ids_.name))
            {
                includeNamesMap[pm.key.toInt()] = si.value.toString();
                break;
            }
        }
    }

    // Информируем, что удалили
    if (notifyManager_)
        fileItemsManager_->AfterIncludesListChanged(fileId_, includeNamesMap);
}

void FileItem::UpdateVariablesArrayModel(const CubesXml::Include* xmlInclude, CubesUnitTypes::ParameterModel& model, int& count)
{
    //// Разделяем путь на части
    //QStringList path = model.id.split("/");
    //if (path.size() != 3 || path[0] != ids_.includesGroupName)
    //    return;

    //// Получаем модель (INCLUDES/ITEM_X)
    //const auto pmi = GetParameterModel(QString("%1/%2").arg(path[0], path[1]));
    //if (pmi == nullptr)
    //    return;

    //// Получаем имя
    //QString includeName;
    //for (const auto& si : pmi->parameters)
    //{
    //    if (si.id.endsWith(ids_.name))
    //    {
    //        includeName = si.value.toString();
    //        break;
    //    }
    //}

    // Сначала добавляем
    if (model.parameters.size() < count)
    {
        // Добавляем
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Создаем
            CubesUnitTypes::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            //group_model.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.key = ++uniqueNumber_;

            CubesUnitTypes::ParameterModel name;
            name.editorSettings.type = CubesUnitTypes::EditorType::String;
            name.id = group_model.id + ids_.name;
            //name.id = QString("%1/%2").arg(group_model.id, ids_.name);
            name.name = QString::fromLocal8Bit("Имя");
            //name.value = QString::fromLocal8Bit("variable_%1").arg(i);
            //name.valueType = "string";
            if (xmlInclude != nullptr && i < xmlInclude->variables.size())
                name.value = xmlInclude->variables[i].first;
            else
                name.value = QString::fromLocal8Bit("BoxId").arg(i);
            name.editorSettings.type = CubesUnitTypes::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            CubesUnitTypes::ParameterModel variable;
            variable.editorSettings.type = CubesUnitTypes::EditorType::String;
            variable.id = group_model.id + ids_.value;
            //variable.id = QString("%1/%2").arg(group_model.id, "VALUE");
            variable.name = QString::fromLocal8Bit("Значение");
            //variable.value = QString();
            //variable.valueType = "string";
            if (xmlInclude != nullptr && i < xmlInclude->variables.size())
                variable.value = xmlInclude->variables[i].second;
            else
                variable.value = QString::fromLocal8Bit("");
            variable.editorSettings.type = CubesUnitTypes::EditorType::String;
            variable.editorSettings.is_expanded = false;
            group_model.parameters.push_back(std::move(variable));

            model.parameters.push_back(std::move(group_model));
        }
    }

    // Теперь удаляем
    if (model.parameters.size() > count)
    {
        // Удаляем
        while (model.parameters.size() > count)
        {
            // Удаляем параметр
            model.parameters.pop_back();
        }
    }

    // Получаем список имеющихся переменных
    QList<QPair<QString, QString>> variables;
    for (const auto& v : model.parameters)
    {
        QString name;
        QString value;
        for (const auto& si : v.parameters)
        {
            if (si.id.endsWith(ids_.name))
                name = si.value.toString();
            else if (si.id.endsWith(ids_.value))
                value = si.value.toString();
        }
        if (!name.isEmpty())
            variables.push_back({ name, value });
    }

    // Информируем, что создали
    //fileItemsManager_->AfterVariablesListChanged(fileId_, includeName, variables);
}

void FileItem::UpdateConnectArrayModel(const CubesXml::Networking* xmlNetworking, CubesUnitTypes::ParameterModel& model, int& count)
{
    // Сначала добавляем
    if (model.parameters.size() < count)
    {
        // Добавляем
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Создаем
            CubesUnitTypes::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            //group_model.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            CubesUnitTypes::ParameterModel port;
            port.editorSettings.type = CubesUnitTypes::EditorType::String;
            port.id = group_model.id + ids_.port;
            //port.id = QString("%1/%2").arg(group_model.id, "PORT");
            port.name = QString::fromLocal8Bit("Порт");
            //port.value = int{ 50000 };
            //port.valueType = "int";
            if (xmlNetworking != nullptr && i < xmlNetworking->connects.size())
                port.value = xmlNetworking->connects[i].port;
            else
                port.value = CubesXml::Connect::Defaults().port;
            port.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
            port.editorSettings.SpinIntergerMin = 1000;
            port.editorSettings.SpinIntergerMax = 65535;
            group_model.parameters.push_back(std::move(port));

            CubesUnitTypes::ParameterModel ip;
            ip.editorSettings.type = CubesUnitTypes::EditorType::String;
            ip.id = group_model.id + ids_.ip;
            //ip.id = QString("%1/%2").arg(group_model.id, "IP");
            ip.name = QString::fromLocal8Bit("Хост");
            //ip.value = QString::fromLocal8Bit("127.0.0.1");
            //ip.valueType = "string";
            if (xmlNetworking != nullptr && i < xmlNetworking->connects.size())
                ip.value = xmlNetworking->connects[i].ip;
            else
                ip.value = CubesXml::Connect::Defaults().ip;
            ip.editorSettings.type = CubesUnitTypes::EditorType::String;
            group_model.parameters.push_back(std::move(ip));

            model.parameters.push_back(std::move(group_model));
        }
    }

    // Теперь удаляем
    while (model.parameters.size() > count)
    {
        // Удаляем параметр
        model.parameters.pop_back();
    }
}

void FileItem::RegisterProperty(const QtProperty* property, const CubesUnitTypes::ParameterModelId& id)
{
    propertyToId_[property] = id;
    idToProperty_[id] = property;
}

void FileItem::UnregisterProperty(const CubesUnitTypes::ParameterModelId& id)
{
    UnregisterProperty(idToProperty_[id]);
}

void FileItem::UnregisterProperty(const QtProperty* property)
{
    for (auto p : property->subProperties())
        UnregisterProperty(p);

    idToProperty_.remove(propertyToId_[property]);
    propertyToId_.remove(property);
}

QtProperty* FileItem::GetProperty(const CubesUnitTypes::ParameterModelId& id)
{
    auto it = idToProperty_.find(id);
    if (it != idToProperty_.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;
}

CubesUnitTypes::ParameterModelId FileItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return QString();
}

CubesUnitTypes::ParameterModel* FileItem::GetParameterModel(const CubesUnitTypes::ParameterModelId& id)
{
    CubesUnitTypes::ParameterModel* pm = nullptr;

    {
        auto sl = id.split();
        auto ql = &model_.parameters;
        CubesUnitTypes::ParameterModelId idt;
        while (sl.size() > 0)
        {
            idt += sl[0];
            //idt = idt == "" ? sl[0] : idt + "/" + sl[0];
            bool found = false;
            for (auto& x : *ql)
            {
                if (x.id == idt)
                {
                    pm = &x;
                    ql = &x.parameters;
                    sl.pop_front();
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                pm = nullptr;
                break;
            }
        }
    }

    return pm;
}

CubesUnitTypes::ParameterModel* FileItem::GetParameterModel(const QtProperty* property)
{
    const auto id = GetPropertyId(property);
    if (id.empty())
        return nullptr;

    return GetParameterModel(id);
}

void FileItem::ApplyExpandState(QtBrowserItem* index)
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> children = index->children();
    QListIterator<QtBrowserItem*> itChild(children);
    while (itChild.hasNext())
        ApplyExpandState(itChild.next());
    QtProperty* prop = index->property();

    auto pm = GetParameterModel(prop);
    if (pm != nullptr)
        pe->setExpanded(index, pm->editorSettings.is_expanded);
}

void FileItem::ApplyExpandState()
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> indexes = pe->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}
