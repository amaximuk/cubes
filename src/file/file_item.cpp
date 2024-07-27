#include <QDebug>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "parameters.h"
#include "../windows/main_window.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../parameters/base64.h"
#include "../unit/unit_helper.h"
#include "file_item_types.h"
#include "file_item.h"

using namespace CubesFile;

FileItem::FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, CubesUnit::FileId fileId)
{
    fileItemsManager_ = fileItemsManager;
    editor_ = editor;
    fileId_ = fileId;
    parameterModels_ = {};
    ignoreEvents_ = false;
    notifyManager_ = false;
    uniqueNumber_ = CubesUnit::InvalidIncludeId;

    CreateParametersModel(nullptr);
    CreateProperties();
    notifyManager_ = true;
}

FileItem::FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, const CubesXml::File& xmlFile, CubesUnit::FileId fileId)
{
    fileItemsManager_ = fileItemsManager;
    editor_ = editor;
    fileId_ = fileId;
    parameterModels_ = {};
    ignoreEvents_ = false;
    notifyManager_ = false;
    uniqueNumber_ = CubesUnit::InvalidIncludeId;

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
        CubesUnit::ParameterModel base_group;
        base_group.id = ids_.base;
        base_group.name = QString::fromLocal8Bit("Базовые");
        base_group.value = QVariant();
        base_group.editorSettings.type = CubesUnit::EditorType::None;
        base_group.editorSettings.isExpanded = true;

        CubesUnit::ParameterModel name;
        name.id = ids_.base + ids_.name;
        name.name = QString::fromLocal8Bit("Имя");
        if (xmlFile == nullptr || xmlFile->name.isEmpty())
            name.value = QString::fromLocal8Bit("АРМ_%1").arg(fileId_);
        else
            name.value = QString(xmlFile->name);
        name.editorSettings.type = CubesUnit::EditorType::String;
        name.editorSettings.isExpanded = false;
        base_group.parameters.push_back(std::move(name));

        CubesUnit::ParameterModel platform;
        platform.id = ids_.base + ids_.platform;
        platform.name = QString::fromLocal8Bit("Платформа");
        platform.value = QString();
        platform.editorSettings.type = CubesUnit::EditorType::ComboBox;
        platform.editorSettings.isExpanded = false;
        for (int i = 0; i < CubesUnit::platform_names_.size(); ++i)
        {
            platform.editorSettings.comboBoxValues.push_back({ static_cast<CubesUnit::BaseId>(i),
                QString::fromStdString(CubesUnit::platform_names_[i]) });
        }
        if (CubesUnit::platform_names_.size() > 0)
        {
            if (xmlFile == nullptr || xmlFile->name.isEmpty())
                platform.SetComboBoxValue(QString::fromStdString(CubesUnit::platform_names_[0]));
            else
                platform.SetComboBoxValue(xmlFile->platform);
        }
        base_group.parameters.push_back(std::move(platform));

        CubesUnit::ParameterModel file_path;
        file_path.id = ids_.base + ids_.path;
        file_path.name = QString::fromLocal8Bit("Имя файла");
        if (xmlFile == nullptr || xmlFile->fileName.isEmpty())
            file_path.value = QString::fromLocal8Bit("config.xml");
        else
            file_path.value = QString(xmlFile->fileName);
        file_path.editorSettings.type = CubesUnit::EditorType::String;
        file_path.editorSettings.isExpanded = false;
        base_group.parameters.push_back(std::move(file_path));
        parameterModels_.push_back(std::move(base_group));
    }

    {
        CubesUnit::ParameterModel includes;
        includes.id = ids_.includes;
        includes.name = QString::fromLocal8Bit("Включаемые файлы");
        includes.value = int{ 0 };
        includes.editorSettings.type = CubesUnit::EditorType::SpinInterger;
        includes.editorSettings.isExpanded = true;
        includes.editorSettings.spinIntergerMin = 0;
        includes.editorSettings.spinIntergerMax = 1000; // !!! TODO: make a define for a const

        if (xmlFile != nullptr)
        {
            auto xmlCount = xmlFile->includes.size();
            includes.value = QString("%1").arg(xmlCount);

            UpdateIncludesArrayModel(xmlFile, includes, xmlCount);
        }

        parameterModels_.push_back(std::move(includes));
    }

    {
        CubesUnit::ParameterModel properties_group;
        properties_group.id = ids_.parameters;
        properties_group.name = QString::fromLocal8Bit("Параметры");
        properties_group.value = QVariant();
        properties_group.editorSettings.type = CubesUnit::EditorType::None;
        properties_group.editorSettings.isExpanded = false;
        
        {
            CubesUnit::ParameterModel pm_networking;
            pm_networking.id = ids_.parameters + ids_.networking;
            pm_networking.name = QString::fromLocal8Bit("Соединение");
            pm_networking.value = QVariant();
            pm_networking.editorSettings.type = CubesUnit::EditorType::None;
            properties_group.editorSettings.isExpanded = true;

            CubesUnit::ParameterModel pm_id;
            pm_id.id = ids_.parameters + ids_.networking + ids_.id;
            pm_id.name = QString::fromLocal8Bit("Идентифиикатор");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_id.value = int{ CubesXml::Networking::Defaults().id};
            else
                pm_id.value = int{ xmlFile->config.networking.id };
            pm_id.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_id.editorSettings.spinIntergerMin = 0;
            pm_id.editorSettings.spinIntergerMax = std::numeric_limits<int>::max();
            pm_networking.parameters.push_back(std::move(pm_id));

            CubesUnit::ParameterModel pm_accept_port;
            pm_accept_port.id = ids_.parameters + ids_.networking + ids_.acceptPort;
            pm_accept_port.name = QString::fromLocal8Bit("Порт");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_accept_port.value = int{ CubesXml::Networking::Defaults().acceptPort };
            else
                pm_accept_port.value = int{ xmlFile->config.networking.acceptPort };
            pm_accept_port.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_accept_port.editorSettings.spinIntergerMin = 1000;
            pm_accept_port.editorSettings.spinIntergerMax = 65535;
            pm_networking.parameters.push_back(std::move(pm_accept_port));

            CubesUnit::ParameterModel pm_keep_alive_sec;
            pm_keep_alive_sec.id = ids_.parameters + ids_.networking + ids_.keepAliveSec;
            pm_keep_alive_sec.name = QString::fromLocal8Bit("Таймаут");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_keep_alive_sec.value = int{ CubesXml::Networking::Defaults().keepAliveSec };
            else
                pm_keep_alive_sec.value = int{ xmlFile->config.networking.keepAliveSec };
            pm_keep_alive_sec.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_keep_alive_sec.editorSettings.spinIntergerMin = 0;
            pm_keep_alive_sec.editorSettings.spinIntergerMax = std::numeric_limits<int>::max();
            pm_networking.parameters.push_back(std::move(pm_keep_alive_sec));

            CubesUnit::ParameterModel pm_time_client;
            pm_time_client.id = ids_.parameters + ids_.networking + ids_.timeClient;
            pm_time_client.name = QString::fromLocal8Bit("Получать время");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_time_client.value = bool{ CubesXml::Networking::Defaults().timeClient };
            else
                pm_time_client.value = bool{ xmlFile->config.networking.timeClient };
            pm_time_client.editorSettings.type = CubesUnit::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_time_client));

            CubesUnit::ParameterModel pm_network_threads;
            pm_network_threads.id = ids_.parameters + ids_.networking + ids_.networkThreads;
            pm_network_threads.name = QString::fromLocal8Bit("Сетевых потоков");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_network_threads.value = int{ CubesXml::Networking::Defaults().networkThreads };
            else
                pm_network_threads.value = int{ xmlFile->config.networking.networkThreads };
            pm_network_threads.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_network_threads.editorSettings.spinIntergerMin = 1;
            pm_network_threads.editorSettings.spinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_network_threads));

            CubesUnit::ParameterModel pm_broadcast_threads;
            pm_broadcast_threads.id = ids_.parameters + ids_.networking + ids_.broadcastThreads;
            pm_broadcast_threads.name = QString::fromLocal8Bit("Широковещательных потоков");
            pm_broadcast_threads.value = int{ CubesXml::Networking::Defaults().broadcastThreads };
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_broadcast_threads.value = int{ CubesXml::Networking::Defaults().broadcastThreads };
            else
                pm_broadcast_threads.value = int{ xmlFile->config.networking.broadcastThreads };
            pm_broadcast_threads.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_broadcast_threads.editorSettings.spinIntergerMin = 1;
            pm_broadcast_threads.editorSettings.spinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_broadcast_threads));

            CubesUnit::ParameterModel pm_client_threads;
            pm_client_threads.id = ids_.parameters + ids_.networking + ids_.clientsThreads;
            pm_client_threads.name = QString::fromLocal8Bit("Клиентских потоков");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_client_threads.value = int{ CubesXml::Networking::Defaults().clientsThreads };
            else
                pm_client_threads.value = int{ xmlFile->config.networking.clientsThreads };
            pm_client_threads.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_client_threads.editorSettings.spinIntergerMin = 1;
            pm_client_threads.editorSettings.spinIntergerMax = 1024;
            pm_networking.parameters.push_back(std::move(pm_client_threads));

            CubesUnit::ParameterModel pm_notify_ready_client;
            pm_notify_ready_client.id = ids_.parameters + ids_.networking + ids_.notifyReadyClients;
            pm_notify_ready_client.name = QString::fromLocal8Bit("Информировать клиента");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_notify_ready_client.value = bool{ CubesXml::Networking::Defaults().notifyReadyClients };
            else
                pm_notify_ready_client.value = bool{ xmlFile->config.networking.notifyReadyClients };
            pm_notify_ready_client.editorSettings.type = CubesUnit::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_notify_ready_client));

            CubesUnit::ParameterModel pm_notify_ready_server;
            pm_notify_ready_server.id = ids_.parameters + ids_.networking + ids_.notifyReadyServers;
            pm_notify_ready_server.name = QString::fromLocal8Bit("Информировать сервер");
            if (xmlFile == nullptr || !xmlFile->config.networkingIsSet)
                pm_notify_ready_server.value = bool{ CubesXml::Networking::Defaults().notifyReadyServers };
            else
                pm_notify_ready_server.value = bool{ xmlFile->config.networking.notifyReadyServers };
            pm_notify_ready_server.editorSettings.type = CubesUnit::EditorType::CheckBox;
            pm_networking.parameters.push_back(std::move(pm_notify_ready_server));

            {
                CubesUnit::ParameterModel connect;
                connect.id = ids_.parameters + ids_.networking + ids_.connect;
                connect.name = QString::fromLocal8Bit("Подключения");
                connect.value = int{ 0 };
                connect.editorSettings.type = CubesUnit::EditorType::SpinInterger;
                connect.editorSettings.isExpanded = true;
                connect.editorSettings.spinIntergerMin = 0;
                connect.editorSettings.spinIntergerMax = 1000; // !!! TODO: make a define for a const

                if (xmlFile != nullptr && xmlFile->config.networkingIsSet)
                {
                    int xmlCount = xmlFile->config.networking.connects.size();
                    connect.value = xmlCount;
                    UpdateConnectArrayModel(&xmlFile->config.networking, connect, xmlCount);
                }

                pm_networking.parameters.push_back(std::move(connect));
            }

            properties_group.parameters.push_back(std::move(pm_networking));
        }

        {
            CubesUnit::ParameterModel pm_logging;
            pm_logging.id = ids_.parameters + ids_.log;
            pm_logging.name = QString::fromLocal8Bit("Логирование");
            pm_logging.value = QVariant();
            pm_logging.editorSettings.type = CubesUnit::EditorType::None;
            properties_group.editorSettings.isExpanded = true;

            CubesUnit::ParameterModel pm_logging_level;
            pm_logging_level.id = ids_.parameters + ids_.log + ids_.loggingLevel;
            pm_logging_level.name = QString::fromLocal8Bit("Уровень");
            pm_logging_level.editorSettings.type = CubesUnit::EditorType::ComboBox;
            const QStringList logLevels{ "LOG_TRACE", "LOG_DEBUG", "LOG_INFO" , "LOG_WARNING" , "LOG_ERROR" , "LOG_FATAL" };
            for (int i = 0; i < logLevels.size(); ++i)
                pm_logging_level.editorSettings.comboBoxValues.push_back({ static_cast<CubesUnit::BaseId>(i), logLevels[i] });
            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_logging_level.SetComboBoxValue(CubesXml::Log::Defaults().loggingLevel);
            else if (xmlFile->config.log.loggingLevel < pm_logging_level.editorSettings.comboBoxValues.size())
                pm_logging_level.SetComboBoxValue(xmlFile->config.log.loggingLevel);
            else
                pm_logging_level.SetComboBoxValueDefault();
            pm_logging.parameters.push_back(std::move(pm_logging_level));

            CubesUnit::ParameterModel pm_log_limit;
            pm_log_limit.id = ids_.parameters + ids_.log + ids_.totalLogLimitMb;
            pm_log_limit.name = QString::fromLocal8Bit("Размер");
            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_log_limit.value = int{ CubesXml::Log::Defaults().totalLogLimit };
            else
                pm_log_limit.value = int{ xmlFile->config.log.totalLogLimit };
            pm_log_limit.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            pm_log_limit.editorSettings.spinIntergerMin = 0;
            pm_log_limit.editorSettings.spinIntergerMax = 1024 * 1024;
            pm_logging.parameters.push_back(std::move(pm_log_limit));

            CubesUnit::ParameterModel pm_log_dir;
            pm_log_dir.id = ids_.parameters + ids_.log + ids_.logDir;
            pm_log_dir.name = QString::fromLocal8Bit("Директория");

            if (xmlFile == nullptr || !xmlFile->config.logIsSet)
                pm_log_dir.value = CubesXml::Log::Defaults().logDir;
            else
                pm_log_dir.value = xmlFile->config.log.logDir;
            pm_log_dir.editorSettings.type = CubesUnit::EditorType::String;
            pm_logging.parameters.push_back(std::move(pm_log_dir));

            properties_group.parameters.push_back(pm_logging);
        }

        parameterModels_.push_back(std::move(properties_group));
    }

    {
        CubesUnit::ParameterModel editor_group;
        editor_group.id = ids_.editor;
        editor_group.name = QString::fromLocal8Bit("Редактор");
        editor_group.value = QVariant();
        editor_group.editorSettings.type = CubesUnit::EditorType::None;
        editor_group.editorSettings.isExpanded = true;

        {
            CubesUnit::ParameterModel pm;
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
            pm.editorSettings.type = CubesUnit::EditorType::Color;
            editor_group.parameters.push_back(std::move(pm));
        }

        parameterModels_.push_back(std::move(editor_group));
    }
}

void FileItem::CreateProperties()
{
    if (editor_ != nullptr)
    {
        QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
        for (auto& pm : parameterModels_)
            topLevelProperties_.push_back(editor_->CreatePropertyForModel(pm, idToProperty));
        for (const auto& kvp : idToProperty.toStdMap())
            RegisterProperty(kvp.second, kvp.first);
    }
}

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
            {
                const auto pmItem = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, pm->id.left(2));
                const auto includeId = pmItem->key;

                UpdateVariablesArrayModel(nullptr, includeId, *pm, count);
            }
            editor_->SetIntValue(property, count);

            QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
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

            QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
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

    fileItemsManager_->AfterPropertiesChanged();
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
        pm->value = value;
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
    
        const auto pmItem = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, pm->id.left(2));
        const auto includeId = pmItem->key;

        pm->value = value;
        fileItemsManager_->AfterIncludeNameChanged(fileId_, includeId);
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

        const auto pmItem = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, pm->id.left(2));
        const auto includeId = pmItem->key;

        QString oldName = pm->value.toString();
        pm->value = value;
        fileItemsManager_->AfterVariableNameChanged(fileId_, includeId, value, oldName);
    }
    else if (pm->id.startsWith(ids_.includes) && pm->id.size() == 5 && ids_.IsItem(pm->id.mid(1, 1)) &&
        pm->id.mid(2, 1) == ids_.variables && ids_.IsItem(pm->id.mid(3, 1)) && pm->id.endsWith(ids_.value))
    {
        const auto pmItem = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, pm->id.left(2));
        const auto includeId = pmItem->key;
        
        QString oldName = pm->value.toString();
        pm->value = value;
        CubesUnit::VariableIdVariables variables;
        if (!GetIncludeVariables(includeId, variables))
            return;
        fileItemsManager_->AfterVariablesListChanged(fileId_, includeId, variables);
    }

    fileItemsManager_->AfterPropertiesChanged();
}

CubesUnit::FileId FileItem::GetFileId() const
{
    return fileId_;
}

void FileItem::Select()
{
    if (editor_ != nullptr)
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
}

void FileItem::UnSelect()
{
    qDebug() << disconnect(editor_, &PropertiesEditor::ValueChanged, this, &FileItem::ValueChanged);
    qDebug() << disconnect(editor_, &PropertiesEditor::StringEditingFinished, this, &FileItem::StringEditingFinished);
}

QString FileItem::GetName()
{
    auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.name);
    if (pm == nullptr)
        return "";

    return pm->value.toString();
};

QColor FileItem::GetColor()
{
    auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.editor + ids_.color);
    if (pm == nullptr)
        return "";

    return QColor::fromRgba(pm->value.toInt());
};

void FileItem::ExpandedChanged(const QtProperty* property, bool is_expanded)
{
    if (!ignoreEvents_)
    {
        auto pm = GetParameterModel(property);
        if (pm != nullptr)
            pm->editorSettings.isExpanded = is_expanded;
    }
}

void FileItem::SetName(QString name, bool setOldName, QString oldName)
{
    auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.name);
    pm->value = name;

    auto pr = GetProperty(pm->id);
    editor_->SetStringValue(pr, name, setOldName, oldName);
}

void FileItem::SetPath(QString name, bool setOldName, QString oldName)
{
    auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.path);
    pm->value = name;

    auto pr = GetProperty(pm->id);
    editor_->SetStringValue(pr, name, setOldName, oldName);
}

void FileItem::SetColor(QColor color)
{
    auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.editor + ids_.color);
    pm->value = color.rgba();

    auto pr = GetProperty(pm->id);
    editor_->SetColorValue(pr, color);
}

void FileItem::AddInclude(const CubesUnit::IncludeId includeId, const CubesUnit::VariableIdVariables& variables)
{
    auto pmi = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes);
    int ci = pmi->value.toInt() + 1;

    auto pri = GetProperty(pmi->id);
    editor_->SetIntValue(pri, ci);
    // Установка количества элементов в Property Browser вызывает операцию по добавлению
    // необходимого количества заготовок через ValueChanged

    auto pmItem = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1));
    pmItem->key = includeId;

    auto pmin = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1) + ids_.name);
    pmin->value = QString::fromLocal8Bit("Включение%1").arg(ci);

    auto prin = GetProperty(pmin->id);
    editor_->SetStringValue(prin, pmin->value.toString());

    const auto includeName = GetIncludeName(includeId);

    auto pmifn = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1) + ids_.filePath);
    pmifn->value = includeName;

    auto prifn = GetProperty(pmifn->id);
    editor_->SetStringValue(prifn, includeName);

    auto pmiv = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1) + ids_.variables);

    auto priv = GetProperty(pmiv->id);
    editor_->SetIntValue(priv, variables.size());
    // Установка количества элементов в Property Browser вызывает операцию по добавлению
    // необходимого количества заготовок через ValueChanged

    for (int i = 0; i < variables.values().size(); i++)
    {
        auto& v = variables.values().at(i);
        auto pmivn = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1) + ids_.variables + ids_.Item(i) + ids_.name);
        pmivn->value = v.first;

        auto privn = GetProperty(pmivn->id);
        editor_->SetStringValue(privn, v.first);

        auto pmivv = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(ci - 1) + ids_.variables + ids_.Item(i) + ids_.value);
        pmivv->value = v.second;

        auto privv = GetProperty(pmivv->id);
        editor_->SetStringValue(privv, v.second);
    }
}

CubesUnit::IncludeIdNames FileItem::GetIncludes()
{
    return CubesUnit::Helper::File::GetIncludes(parameterModels_);
}

bool FileItem::GetIncludeVariables(const CubesUnit::IncludeId includeId, CubesUnit::VariableIdVariables& variables)
{
    return CubesUnit::Helper::File::GetIncludeVariables(parameterModels_, includeId, variables);
}

QString FileItem::GetIncludeName(const CubesUnit::IncludeId includeId)
{
    return CubesUnit::Helper::File::GetIncludeName(parameterModels_, includeId);
}

QString FileItem::GetIncludePath(const CubesUnit::IncludeId includeId)
{
    return CubesUnit::Helper::File::GetIncludePath(parameterModels_, includeId);
}

File FileItem::GetFile()
{
    File result{};
    result.name = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.name)->value.toString();
    result.platform = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.platform)->value.toString();
    result.path = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.path)->value.toString();

    result.network.id = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.id)->value.toInt();
    result.network.accept_port = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.acceptPort)->value.toInt();
    result.network.keep_alive_sec = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.keepAliveSec)->value.toInt();
    result.network.time_client = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.timeClient)->value.toBool();
    result.network.network_threads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.networkThreads)->value.toInt();
    result.network.broadcast_threads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.broadcastThreads)->value.toInt();
    result.network.clients_threads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.clientsThreads)->value.toInt();
    result.network.notify_ready_clients = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.notifyReadyClients)->value.toBool();
    result.network.notify_ready_servers = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.notifyReadyServers)->value.toBool();

    int count = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect)->value.toInt();
    for (int i = 0; i < count; i++)
    {
        Connect connect{};
        connect.port = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.port)->value.toInt();
        connect.ip = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.ip)->value.toString();
        result.network.connect.push_back(connect);
    }

    result.log.level = static_cast<LoggingLevel>(CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.loggingLevel)->value.toInt());
    result.log.limit_mb = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.totalLogLimitMb)->value.toInt();
    result.log.directory_path = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.logDir)->value.toString();

    const auto includes = GetIncludes();
    for (const auto& kvp : includes.toStdMap())
    {
        Include include{};
        include.name = kvp.second;
        include.path = GetIncludePath(kvp.first);

        CubesUnit::VariableIdVariables variables;
        if (!GetIncludeVariables(kvp.first, variables))
            return {};
        for(const auto& v : variables.values())
            include.variables.push_back(v);
        result.includes.push_back(include);
    }

    return result;
}

CubesXml::File FileItem::GetXmlFile()
{
    CubesXml::File result{};
    result.name = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.name)->value.toString();
    result.platform = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.platform)->value.toString();
    result.fileName = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.path)->value.toString();
    QColor color = QColor::fromRgba(CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.editor + ids_.color)->value.toUInt());
    result.color = QString("#%1").arg(color.rgba(), 8, 16, QLatin1Char('0'));

    result.config.networkingIsSet = true;
    result.config.networking.id = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.id)->value.toInt();
    result.config.networking.acceptPort = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.acceptPort)->value.toInt();
    result.config.networking.keepAliveSec = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.keepAliveSec)->value.toInt();
    result.config.networking.timeClient = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.timeClient)->value.toBool();
    result.config.networking.networkThreads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.networkThreads)->value.toInt();
    result.config.networking.broadcastThreads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.broadcastThreads)->value.toInt();
    result.config.networking.clientsThreads = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.clientsThreads)->value.toInt();
    result.config.networking.notifyReadyClients = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.notifyReadyClients)->value.toBool();
    result.config.networking.notifyReadyServers = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.notifyReadyServers)->value.toBool();

    int count = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect)->value.toInt();
    for (int i = 0; i < count; i++)
    {
        CubesXml::Connect connect{};
        connect.port = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.port)->value.toInt();
        connect.ip = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.ip)->value.toString();
        result.config.networking.connects.push_back(connect);
    }

    result.config.logIsSet = true;
    result.config.log.loggingLevel = static_cast<LoggingLevel>(CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.loggingLevel)->value.toInt());
    result.config.log.totalLogLimit = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.totalLogLimitMb)->value.toInt();
    result.config.log.logDir = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.log + ids_.logDir)->value.toString();

    const auto includes = GetIncludes();
    for (const auto& kvp : includes.toStdMap())
    {
        CubesXml::Include include{};
        include.name = kvp.second;
        include.fileName = GetIncludePath(kvp.first);
        CubesUnit::VariableIdVariables variables;
        if (!GetIncludeVariables(kvp.first, variables))
            return {};
        for (const auto& v : variables.values())
            include.variables.push_back(v);
        result.includes.push_back(include);
    }

    return result;
}

QVector<CubesAnalysis::File> FileItem::GetAnalysisFiles()
{
    QVector<CubesAnalysis::File> result;

    {
        CubesAnalysis::File file;
        file.path = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.base + ids_.path)->value.toString();
        file.is_include = false;
        file.main.fileId = GetFileId();
        file.main.name = GetName();
        file.main.id = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.id)->value.toInt();
        file.main.port = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.acceptPort)->value.toInt();

        int count = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect)->value.toInt();
        for (int i = 0; i < count; i++)
        {
            CubesAnalysis::Endpoint endpoint{};
            endpoint.host = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.ip)->value.toString();
            endpoint.port = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.parameters + ids_.networking + ids_.connect + ids_.Item(i) + ids_.port)->value.toInt();
            file.main.connect.push_back(endpoint);
        }

        result.push_back(file);
    }

    const auto pm = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes);
    if (pm == nullptr)
        return result;

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        CubesAnalysis::File file;
        file.path = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
        file.is_include = true;
        file.include.includeId = CubesUnit::Helper::Common::GetParameterModel(parameterModels_, ids_.includes + ids_.Item(i))->key;
        file.include.name = GetIncludeName(file.include.includeId);
        result.push_back(file);
    }

    return result;
}

CubesUnit::ParameterModels FileItem::GetParameterModels()
{
    return parameterModels_;
}

CubesUnit::ParameterModels* FileItem::GetParameterModelsRef()
{
    return &parameterModels_;
}

void FileItem::UpdateIncludesArrayModel(const CubesXml::File* xmlFile, CubesUnit::ParameterModel& model, int& count)
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
                includeName = CubesUnit::Helper::Common::GetUniqueName(QString::fromLocal8Bit("Файл"), " ", includeNames);

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
            CubesUnit::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnit::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.key = ++uniqueNumber_;

            CubesUnit::ParameterModel name;
            name.editorSettings.type = CubesUnit::EditorType::String;
            name.id = group_model.id + ids_.name;
            name.name = QString::fromLocal8Bit("Имя");
            name.value = includeName;
            name.editorSettings.type = CubesUnit::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            CubesUnit::ParameterModel file_path;
            file_path.editorSettings.type = CubesUnit::EditorType::String;
            file_path.id = group_model.id + ids_.filePath;
            file_path.name = QString::fromLocal8Bit("Имя файла");
            if (xmlFile != nullptr && i < xmlFile->includes.size())
                file_path.value = xmlFile->includes[i].fileName;
            else
                file_path.value = QString::fromLocal8Bit("include.xml");;
            file_path.editorSettings.type = CubesUnit::EditorType::String;
            file_path.editorSettings.isExpanded = false;
            group_model.parameters.push_back(std::move(file_path));

            {
                CubesUnit::ParameterModel variables;
                variables.id = group_model.id + ids_.variables;
                variables.name = QString::fromLocal8Bit("Переменные");
                variables.editorSettings.type = CubesUnit::EditorType::SpinInterger;
                variables.editorSettings.isExpanded = true;
                variables.editorSettings.spinIntergerMin = 0;
                variables.editorSettings.spinIntergerMax = 1000; // !!! TODO: make a define for a const

                if (xmlFile != nullptr && i < xmlFile->includes.size())
                {
                    int xmlCount = xmlFile->includes[i].variables.size();
                    variables.value = xmlCount;
                    UpdateVariablesArrayModel(&xmlFile->includes[i], group_model.key, variables, xmlCount);
                }

                group_model.parameters.push_back(std::move(variables));
            }

            model.parameters.push_back(std::move(group_model));
        }
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
    }

    CubesUnit::IncludeIdNames includeNamesMap;
    for (const auto& pm : model.parameters)
    {
        for (const auto& si : pm.parameters)
        {
            if (si.id.endsWith(ids_.name))
            {
                includeNamesMap[pm.key] = si.value.toString();
                break;
            }
        }
    }

    // Информируем, что удалили
    if (notifyManager_)
        fileItemsManager_->AfterIncludesListChanged(fileId_, includeNamesMap);
}

void FileItem::UpdateVariablesArrayModel(const CubesXml::Include* xmlInclude, CubesUnit::IncludeId includeId,
    CubesUnit::ParameterModel& model, int& count)
{
    // Сначала добавляем
    if (model.parameters.size() < count)
    {
        // Добавляем
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Создаем
            CubesUnit::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnit::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.key = ++uniqueNumber_;

            CubesUnit::ParameterModel name;
            name.editorSettings.type = CubesUnit::EditorType::String;
            name.id = group_model.id + ids_.name;
            name.name = QString::fromLocal8Bit("Имя");
            if (xmlInclude != nullptr && i < xmlInclude->variables.size())
                name.value = xmlInclude->variables[i].first;
            else
                name.value = QString::fromLocal8Bit("BoxId").arg(i);
            name.editorSettings.type = CubesUnit::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            CubesUnit::ParameterModel variable;
            variable.editorSettings.type = CubesUnit::EditorType::String;
            variable.id = group_model.id + ids_.value;
            variable.name = QString::fromLocal8Bit("Значение");
            if (xmlInclude != nullptr && i < xmlInclude->variables.size())
                variable.value = xmlInclude->variables[i].second;
            else
                variable.value = QString::fromLocal8Bit("");
            variable.editorSettings.type = CubesUnit::EditorType::String;
            variable.editorSettings.isExpanded = false;
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
    CubesUnit::VariableIdVariables variables;
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
        const auto variableId = v.key;
        if (!name.isEmpty())
            variables[variableId] = { name, value };
    }

    // Информируем, что создали
    if (notifyManager_)
        fileItemsManager_->AfterVariablesListChanged(fileId_, includeId, variables);
}

void FileItem::UpdateConnectArrayModel(const CubesXml::Networking* xmlNetworking, CubesUnit::ParameterModel& model, int& count)
{
    // Сначала добавляем
    if (model.parameters.size() < count)
    {
        // Добавляем
        for (int i = model.parameters.size(); i < count; ++i)
        {
            // Создаем
            CubesUnit::ParameterModel group_model;
            group_model.editorSettings.type = CubesUnit::EditorType::None;
            group_model.id = model.id + ids_.Item(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            CubesUnit::ParameterModel port;
            port.editorSettings.type = CubesUnit::EditorType::String;
            port.id = group_model.id + ids_.port;
            port.name = QString::fromLocal8Bit("Порт");
            if (xmlNetworking != nullptr && i < xmlNetworking->connects.size())
                port.value = xmlNetworking->connects[i].port;
            else
                port.value = CubesXml::Connect::Defaults().port;
            port.editorSettings.type = CubesUnit::EditorType::SpinInterger;
            port.editorSettings.spinIntergerMin = 1000;
            port.editorSettings.spinIntergerMax = 65535;
            group_model.parameters.push_back(std::move(port));

            CubesUnit::ParameterModel ip;
            ip.editorSettings.type = CubesUnit::EditorType::String;
            ip.id = group_model.id + ids_.ip;
            ip.name = QString::fromLocal8Bit("Хост");
            if (xmlNetworking != nullptr && i < xmlNetworking->connects.size())
                ip.value = xmlNetworking->connects[i].ip;
            else
                ip.value = CubesXml::Connect::Defaults().ip;
            ip.editorSettings.type = CubesUnit::EditorType::String;
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

void FileItem::RegisterProperty(const QtProperty* property, const CubesUnit::ParameterModelId& id)
{
    propertyToId_[property] = id;
    idToProperty_[id] = property;
}

void FileItem::UnregisterProperty(const CubesUnit::ParameterModelId& id)
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

QtProperty* FileItem::GetProperty(const CubesUnit::ParameterModelId& id)
{
    auto it = idToProperty_.find(id);
    if (it != idToProperty_.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;
}

CubesUnit::ParameterModelId FileItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return QString();
}

CubesUnit::ParameterModel* FileItem::GetParameterModel(const QtProperty* property)
{
    const auto id = GetPropertyId(property);
    if (id.empty())
        return nullptr;

    return CubesUnit::Helper::Common::GetParameterModel(parameterModels_, id);
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
        pe->setExpanded(index, pm->editorSettings.isExpanded);
}

void FileItem::ApplyExpandState()
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> indexes = pe->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}
