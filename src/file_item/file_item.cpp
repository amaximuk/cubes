#include <QPainter>
#include <QDebug>
#include <QRegularExpression>

#include "../diagram/diagram_scene.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "../main_window.h"
#include "../parameters_compiler/base64.h"
#include "../diagram/diagram_item.h"
#include "../parameters_compiler/parameters_compiler_helper.h"
#include "file_item.h"

using namespace CubesFile;

FileItem::FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor)
{
    fileItemsManager_ = fileItemsManager;
    editor_ = editor;
    model_ = {};
    ignoreEvents_ = false;

    CreateParametersModel();
    CreateProperties();
}

void FileItem::CreateParametersModel()
{
    // BASE
    // BASE/NAME
    // BASE/PLATFORM
    // BASE/FILE_PATH
    // INCLUDES
    // INCLUDES/ITEM_0
    // INCLUDES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VALUE
    // PARAMETERS
    // PARAMETERS/CONNECTION
    // PARAMETERS/CONNECTION/HOST
    // PARAMETERS/CONNECTION/PORT
    // PARAMETERS/LOG
    // EDITOR
    // EDITOR/COLOR

    {
        unit_types::ParameterModel base_group;
        base_group.id = "BASE";
        base_group.name = QString::fromLocal8Bit("Базовые");
        base_group.value = "";
        base_group.valueType = "none";
        //base_group.parameterInfoId = "";
        base_group.editorSettings.type = unit_types::EditorType::None;
        base_group.editorSettings.is_expanded = true;

        unit_types::ParameterModel name;
        name.id = "BASE/NAME";
        name.name = QString::fromLocal8Bit("Имя");
        name.value = QString::fromLocal8Bit("АРМ");
        name.valueType = "string";
        //name.parameterInfoId = "";
        name.editorSettings.type = unit_types::EditorType::String;
        name.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(name));

        unit_types::ParameterModel platform;
        platform.id = "BASE/PLATFORM";
        platform.name = QString::fromLocal8Bit("Платформа");
        platform.value = "";
        platform.valueType = "string";
        //platform.parameterInfoId = "";
        platform.editorSettings.type = unit_types::EditorType::ComboBox;
        platform.editorSettings.is_expanded = false;
        for (const auto& pl : unit_types::platform_names_)
            platform.editorSettings.ComboBoxValues.push_back(QString::fromStdString(pl));
        if (unit_types::platform_names_.size() > 0)
            platform.value = QString::fromStdString(unit_types::platform_names_[0]);
        base_group.parameters.push_back(std::move(platform));

        unit_types::ParameterModel file_path;
        file_path.id = "BASE/FILE_PATH";
        file_path.name = QString::fromLocal8Bit("Имя файла");
        file_path.value = QString::fromLocal8Bit("config.xml");
        file_path.valueType = "string";
        //file_path.parameterInfoId = "";
        file_path.editorSettings.type = unit_types::EditorType::String;
        file_path.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(file_path));
        model_.parameters.push_back(std::move(base_group));
    }

    {
        unit_types::ParameterModel includes;
        includes.id = "INCLUDES";
        includes.name = QString::fromLocal8Bit("Включаемые файлы");
        includes.value = 0;
        includes.valueType = "int";
        includes.editorSettings.type = unit_types::EditorType::SpinInterger;
        includes.editorSettings.is_expanded = true;
        includes.editorSettings.SpinIntergerMin = 0;
        includes.editorSettings.SpinIntergerMax = 100;
        model_.parameters.push_back(std::move(includes));
    }

    {
        unit_types::ParameterModel properties_group;
        properties_group.id = "PARAMETERS";
        properties_group.name = QString::fromLocal8Bit("Параметры");
        properties_group.value = "";
        properties_group.valueType = "none";
        //properties_group.parameterInfoId = "";
        properties_group.editorSettings.type = unit_types::EditorType::None;
        properties_group.editorSettings.is_expanded = false;

        {
            unit_types::ParameterModel pm_connection;
            pm_connection.id = "PARAMETERS/CONNECTION";
            pm_connection.name = QString::fromLocal8Bit("Соединение");
            pm_connection.value = "";
            pm_connection.valueType = "none";
            //pm_connection.parameterInfoId = "";
            pm_connection.editorSettings.type = unit_types::EditorType::None;

            unit_types::ParameterModel pm_host;
            pm_host.id = "PARAMETERS/CONNECTION/HOST";
            pm_host.name = QString::fromLocal8Bit("Хост");
            pm_host.value = "";
            pm_host.valueType = "string";
            //pm_host.parameterInfoId = "";
            pm_host.editorSettings.type = unit_types::EditorType::String;
            pm_host.value = "127.0.0.1";
            pm_connection.parameters.push_back(std::move(pm_host));

            unit_types::ParameterModel pm_port;
            pm_port.id = "PARAMETERS/CONNECTION/PORT";
            pm_port.name = QString::fromLocal8Bit("Порт");
            pm_port.value = 50000;
            pm_port.valueType = "int";
            //pm_port.parameterInfoId = "";
            pm_port.editorSettings.type = unit_types::EditorType::SpinInterger;
            pm_port.editorSettings.SpinIntergerMin = 1000;
            pm_port.editorSettings.SpinIntergerMin = 65535;
            pm_connection.parameters.push_back(std::move(pm_port));

            properties_group.parameters.push_back(std::move(pm_connection));
        }

        {
            unit_types::ParameterModel pm;
            pm.id = "PARAMETERS/LOG";
            pm.name = QString::fromLocal8Bit("Логирование");
            pm.value = "";
            pm.valueType = "none";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = unit_types::EditorType::None;
            properties_group.parameters.push_back(pm);
        }

        model_.parameters.push_back(std::move(properties_group));
    }

    {
        unit_types::ParameterModel editor_group;
        editor_group.id = "EDITOR";
        editor_group.name = QString::fromLocal8Bit("Редактор");
        editor_group.value = "";
        editor_group.valueType = "none";
        //editor_group.parameterInfoId = "";
        editor_group.editorSettings.type = unit_types::EditorType::None;
        editor_group.editorSettings.is_expanded = false;

        {
            unit_types::ParameterModel pm;
            pm.id = "EDITOR/COLOR";
            pm.name = QString::fromLocal8Bit("Цвет");
            pm.value = QColor("Red").rgba();
            pm.valueType = "string";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = unit_types::EditorType::Color;
            editor_group.parameters.push_back(std::move(pm));
        }

        model_.parameters.push_back(std::move(editor_group));
    }
}

void FileItem::CreateProperties()
{
    QMap<QString, const QtProperty*> idToProperty;
    for (auto& pm : model_.parameters)
        topLevelProperties_.push_back(editor_->GetPropertyForModel(pm, idToProperty));
    for (const auto& kvp : idToProperty.toStdMap())
        RegisterProperty(kvp.second, kvp.first);
}

void FileItem::ValueChanged(QtProperty* property, const QVariant& value)
{
    // BASE
    // BASE/NAME
    // BASE/PLATFORM
    // BASE/FILE_PATH
    // INCLUDES
    // INCLUDES/ITEM_0
    // INCLUDES/ITEM_0/NAME
    // INCLUDES/ITEM_0/VALUE
    // PARAMETERS
    // PARAMETERS/CONNECTION
    // PARAMETERS/CONNECTION/HOST
    // PARAMETERS/CONNECTION/PORT
    // PARAMETERS/LOG
    // EDITOR
    // EDITOR/COLOR

    qDebug() << "ValueChanged value = " << value;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith("BASE"))
    {
        if (pm->id == "BASE/NAME")
        {
            //QString oldName = pm->value.toString();
            //pm->value = value;
            //file_items_manager_->InformNameChanged(value.toString(), oldName);
        }
        else if (pm->id == "BASE/PLATFORM")
        {
            pm->value = property->valueText();
        }
        else
            pm->value = value.toString();
    }
    else if (pm->id.startsWith("INCLUDES"))
    {
        if (pm->id.startsWith("INCLUDES/ITEM") && pm->id.endsWith("NAME"))
        {
            //QString oldName = pm->value.toString();
            //pm->value = value;
            //file_items_manager_->InformIncludeNameChanged(GetName(), value.toString(), oldName);
        }
        else if ((pm->id == "INCLUDES") || (pm->id.startsWith("INCLUDES/ITEM") && pm->id.endsWith("VARIABLES")))
        {
            int count = value.toInt();

            if (pm->id == "INCLUDES")
                UpdateIncludesArrayModel(*pm, count);
            else
                UpdateVariablesArrayModel(*pm, count);
            pm->value = count;
            editor_->SetIntValue(property, count);

            QMap<QString, const QtProperty*> idToProperty;
            for (int i = property->subProperties().size(); i < count; ++i)
                property->addSubProperty(editor_->GetPropertyForModel(pm->parameters[i], idToProperty));
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
                UnregisterProperty(p);

            ApplyExpandState();
        }
        else
            pm->value = property->valueText();
    }
    else if (pm->id.startsWith("PARAMETERS"))
    {
        if (pm->id == "PARAMETERS/CONNECTION/PORT")
        {
            pm->value = value.toInt();
        }
        else
            pm->value = value.toString();
    }
    else if (pm->id.startsWith("EDITOR"))
    {
        if (pm->id == "EDITOR/COLOR")
        {
            pm->value = value.toInt();
        }
        else
            pm->value = value.toString();
    }
}

void FileItem::StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue)
{
    qDebug() << "StringEditingFinished value = " << value << ", oldValue = " << oldValue;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id == "BASE/NAME")
    {
        bool cancel = false;
        fileItemsManager_->BeforeFileNameChanged(value, oldValue, cancel);

        if (!cancel)
        {
            QString oldName = pm->value.toString();
            pm->value = value;
            fileItemsManager_->AfterFileNameChanged(value, oldName);
        }
        else
        {
            // Отмена
            editor_->SetStringValue(property, oldValue);
        }
    }
    else if (pm->id.startsWith("INCLUDES/ITEM") && pm->id.endsWith("NAME"))
    {
        bool cancel = false;
        fileItemsManager_->BeforeIncludeNameChanged(GetName(), value, oldValue, cancel);

        if (!cancel)
        {
            QString oldName = pm->value.toString();
            pm->value = value;
            fileItemsManager_->AfterIncludeNameChanged(GetName(), value, oldName);
        }
        else
        {
            // Отмена
            editor_->SetStringValue(property, oldValue);
        }
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
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return "";

    return pm->value.toString();
};

QColor FileItem::GetColor()
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    if (pm == nullptr)
        return "";

    return QColor::fromRgba(pm->value.toInt());
};

QString FileItem::GetPropertyDescription(const QtProperty* property)
{
    QString id = GetPropertyId(property);
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
    auto pm = GetParameterModel("BASE/NAME");
    pm->value = name;

    auto pr = GetProperty(pm->id);
    editor_->SetStringValue(pr, name, setOldName, oldName);
}

void FileItem::SetColor(QColor color)
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    pm->value = color.rgba();

    auto pr = GetProperty(pm->id);
    editor_->SetColorValue(pr, color);
}

QStringList FileItem::GetIncludeNames()
{
    QStringList result;

    const auto pm = GetParameterModel("INCLUDES");
    if (pm == nullptr)
        return result;

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmi = GetParameterModel(QString("INCLUDES/ITEM_%1/NAME").arg(i));
        result.push_back(pmi->value.toString());
    }

    return result;
}

QList<QPair<QString, QString>> FileItem::GetIncludeVariables(const QString& includeName)
{
    QList<QPair<QString, QString>> result;

    const auto pm = GetParameterModel("INCLUDES");
    if (pm == nullptr)
        return result;

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmi = GetParameterModel(QString("INCLUDES/ITEM_%1/NAME").arg(i));
        if (pmi->value == includeName)
        {
            const auto pmiv = GetParameterModel(QString("INCLUDES/ITEM_%1/VARIABLES").arg(i));
            for (int j = 0; j < pmiv->value.toInt(); j++)
            {
                const auto pmivn = GetParameterModel(QString("INCLUDES/ITEM_%1/VARIABLES/ITEM_%2/NAME").arg(i).arg(j));
                const auto pmivv = GetParameterModel(QString("INCLUDES/ITEM_%1/VARIABLES/ITEM_%2/VALUE").arg(i).arg(j));
                result.push_back({ pmivn->value.toString(), pmivv->value.toString() });
            }
            break;
        }
    }

    return result;
}

void FileItem::UpdateIncludesArrayModel(unit_types::ParameterModel& pm, int& count)
{
    // Сначала добавляем
    if (pm.parameters.size() < count)
    {
        // Получаем список имеющихся имен включаемых файлов
        QStringList includeNames;
        for (const auto& i : pm.parameters)
        {
            for (const auto& si : i.parameters)
            {
                if (si.id.endsWith("/NAME"))
                {
                    includeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        // Получаем список новых имен файлов
        QStringList addingIncludeNames;
        for (int i = pm.parameters.size(); i < count; ++i)
        {
            // Получаем уникальное имя
            QString includeName = unit_types::GetUniqueName(QString::fromLocal8Bit("Файл"), " ", includeNames);

            // Добавляем в списки
            addingIncludeNames.push_back(includeName);
            includeNames.push_back(includeName);
        }

        // Проверяем, что можно создавать
        bool cancel = false;
        fileItemsManager_->BeforeIncludesAdd(GetName(), addingIncludeNames, cancel);
        if (cancel)
        {
            // Не сделали все, что просили. Возвращаем count, равный фактическому количеству элементов
            count = pm.parameters.count();
            return;
        }

        // Добавляем
        for (int i = pm.parameters.size(); i < count; ++i)
        {
            // Получаем уникальное имя
            QString includeName = includeNames[i];

            // Создаем
            unit_types::ParameterModel group_model;
            group_model.editorSettings.type = unit_types::EditorType::None;
            group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            unit_types::ParameterModel name;
            name.editorSettings.type = unit_types::EditorType::String;
            name.id = QString("%1/%2").arg(group_model.id, "NAME");
            name.name = QString::fromLocal8Bit("Имя");
            name.value = includeName;
            name.valueType = "string";
            name.editorSettings.type = unit_types::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            unit_types::ParameterModel file_path;
            file_path.editorSettings.type = unit_types::EditorType::String;
            file_path.id = QString("%1/%2").arg(group_model.id, "FILE_PATH");
            file_path.name = QString::fromLocal8Bit("Имя файла");
            file_path.value = QString::fromLocal8Bit("include.xml");
            file_path.valueType = "string";
            file_path.editorSettings.type = unit_types::EditorType::String;
            file_path.editorSettings.is_expanded = false;
            group_model.parameters.push_back(std::move(file_path));

            unit_types::ParameterModel variables;
            variables.id = QString("%1/%2").arg(group_model.id, "VARIABLES");
            variables.name = QString::fromLocal8Bit("Переменные");
            variables.value = 0;
            variables.valueType = "int";
            variables.editorSettings.type = unit_types::EditorType::SpinInterger;
            variables.editorSettings.is_expanded = true;
            variables.editorSettings.SpinIntergerMin = 0;
            variables.editorSettings.SpinIntergerMax = 100;
            group_model.parameters.push_back(std::move(variables));

            pm.parameters.push_back(std::move(group_model));
        }

        // Информируем, что создали
        fileItemsManager_->AfterIncludesListChanged(GetName(), includeNames);
    }

    // Теперь удаляем
    if (pm.parameters.size() > count)
    {
        // Получаем имена удаляемых включаемых файлов
        QStringList removingIncludeNames;
        for (int i = count; i < pm.parameters.count(); ++i)
        {
            for (const auto& si : pm.parameters[i].parameters)
            {
                if (si.id.endsWith("/NAME"))
                {
                    removingIncludeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        // Проверяем, что можно удалять
        bool cancel = false;
        fileItemsManager_->BeforeIncludesRemoved(GetName(), removingIncludeNames, cancel);
        if (cancel)
        {
            // Не сделали все, что просили. Возвращаем count, равный фактическому количеству элементов
            count = pm.parameters.size();
            return;
        }

        // Удаляем
        while (pm.parameters.size() > count)
        {
            // Удаляем параметр
            pm.parameters.pop_back();
        }

        // Получаем список имеющихся имен включаемых файлов
        QStringList includeNames;
        for (const auto& i : pm.parameters)
        {
            for (const auto& si : i.parameters)
            {
                if (si.id.endsWith("/NAME"))
                {
                    includeNames.push_back(si.value.toString());
                    break;
                }
            }
        }

        // Информируем, что удалили
        fileItemsManager_->AfterIncludesListChanged(GetName(), includeNames);
    }
}

void FileItem::UpdateVariablesArrayModel(unit_types::ParameterModel& pm, int& count)
{
    // Разделяем путь на части
    QStringList path = pm.id.split("/");
    if (path.size() != 3 || path[0] != "INCLUDES")
        return;

    // Получаем модель (INCLUDES/ITEM_X)
    const auto pmi = GetParameterModel(QString("%1/%2").arg(path[0], path[1]));
    if (pmi == nullptr)
        return;

    // Получаем имя
    QString includeName;
    for (const auto& si : pmi->parameters)
    {
        if (si.id.endsWith("/NAME"))
        {
            includeName = si.value.toString();
            break;
        }
    }

    // Сначала добавляем
    if (pm.parameters.size() < count)
    {
        // Добавляем
        for (int i = pm.parameters.size(); i < count; ++i)
        {
            // Создаем
            unit_types::ParameterModel group_model;
            group_model.editorSettings.type = unit_types::EditorType::None;
            group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            unit_types::ParameterModel name;
            name.editorSettings.type = unit_types::EditorType::String;
            name.id = QString("%1/%2").arg(group_model.id, "NAME");
            name.name = QString::fromLocal8Bit("Имя");
            name.value = QString::fromLocal8Bit("variable_%1").arg(i);
            name.valueType = "string";
            name.editorSettings.type = unit_types::EditorType::String;
            group_model.parameters.push_back(std::move(name));

            unit_types::ParameterModel variable;
            variable.editorSettings.type = unit_types::EditorType::String;
            variable.id = QString("%1/%2").arg(group_model.id, "VALUE");
            variable.name = QString::fromLocal8Bit("Значение");
            variable.value = "";
            variable.valueType = "string";
            variable.editorSettings.type = unit_types::EditorType::String;
            variable.editorSettings.is_expanded = false;
            group_model.parameters.push_back(std::move(variable));

            pm.parameters.push_back(std::move(group_model));
        }
    }

    // Теперь удаляем
    if (pm.parameters.size() > count)
    {
        // Удаляем
        while (pm.parameters.size() > count)
        {
            // Удаляем параметр
            pm.parameters.pop_back();
        }
    }

    // Получаем список имеющихся переменных
    QList<QPair<QString, QString>> variables;
    for (const auto& v : pm.parameters)
    {
        QString name;
        QString value;
        for (const auto& si : v.parameters)
        {
            if (si.id.endsWith("/NAME"))
                name = si.value.toString();
            else if (si.id.endsWith("/VALUE"))
                value = si.value.toString();
        }
        if (!name.isEmpty())
            variables.push_back({ name, value });
    }

    // Информируем, что создали
    fileItemsManager_->AfterVariableChanged(GetName(), includeName, variables);
}

//void file_item::AddArrayModelItem(unit_types::ParameterModel& pm)
//{
//    if (pm.id == "INCLUDES")
//    {
//        // Получаем уникальное имя
//        QStringList namesList;
//        for (const auto& i : pm.parameters)
//        {
//            for (const auto& si : pm.parameters)
//            {
//                if (si.id.endsWith("/NAME"))
//                {
//                    namesList.push_back(si.value.toString());
//                    break;
//                }
//            }
//        }
//        QString includeName = unit_types::GetUniqueName("Файл", " ", namesList);
//
//        // Проверяем, что можно создавать
//        bool cancel = false;
//        file_items_manager_->BeforeIncludesListChanged(GetName(), includeName, Operation::Add, cancel);
//        if (cancel) return;
//
//        // Добавляем 1 элемент
//        int i = pm.parameters.size();
//
//        unit_types::ParameterModel group_model;
//        group_model.editorSettings.type = unit_types::EditorType::None;
//        group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
//        group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
//
//        unit_types::ParameterModel name;
//        name.editorSettings.type = unit_types::EditorType::String;
//        name.id = QString("%1/%2").arg(group_model.id, "NAME");
//        name.name = QString::fromLocal8Bit("Имя");
//        name.value = QString::fromLocal8Bit("Файл %1").arg(i);
//        name.valueType = "string";
//        name.editorSettings.type = unit_types::EditorType::String;
//        group_model.parameters.push_back(name);
//
//        unit_types::ParameterModel file_path;
//        file_path.editorSettings.type = unit_types::EditorType::String;
//        file_path.id = QString("%1/%2").arg(group_model.id, "FILE_PATH");
//        file_path.name = QString::fromLocal8Bit("Имя файла");
//        file_path.value = QString::fromLocal8Bit("include_%1.xml").arg(i);
//        file_path.valueType = "string";
//        file_path.editorSettings.type = unit_types::EditorType::String;
//        file_path.editorSettings.is_expanded = false;
//        group_model.parameters.push_back(file_path);
//
//        unit_types::ParameterModel variables;
//        variables.id = QString("%1/%2").arg(group_model.id, "VARIABLES");
//        variables.name = QString::fromLocal8Bit("Переменные");
//        variables.value = 0;
//        variables.valueType = "int";
//        variables.editorSettings.type = unit_types::EditorType::SpinInterger;
//        variables.editorSettings.is_expanded = true;
//        variables.editorSettings.SpinIntergerMin = 0;
//        variables.editorSettings.SpinIntergerMax = 100;
//        group_model.parameters.push_back(std::move(variables));
//
//        pm.parameters.push_back(group_model);
//    }
//    else // VARIABLES
//    {
//        // Получаем уникальное имя
//        QStringList namesList;
//        for (const auto& i : pm.parameters)
//        {
//            for (const auto& si : pm.parameters)
//            {
//                if (si.id.endsWith("/NAME"))
//                {
//                    namesList.push_back(si.value.toString());
//                    break;
//                }
//            }
//        }
//        QString includeName = unit_types::GetUniqueName("Файл", " ", namesList);
//
//        //// Проверяем, что можно создавать
//        //bool cancel = false;
//        //file_items_manager_->BeforeIncludesListChanged(GetName(), includeName, Operation::Add, cancel);
//        //if (cancel) return;
//
//        // Добавляем 1 элемент
//        int i = pm.parameters.size();
//
//        unit_types::ParameterModel group_model;
//        group_model.editorSettings.type = unit_types::EditorType::None;
//        group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
//        group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
//
//        unit_types::ParameterModel name;
//        name.editorSettings.type = unit_types::EditorType::String;
//        name.id = QString("%1/%2").arg(group_model.id, "NAME");
//        name.name = QString::fromLocal8Bit("Имя");
//        group_model.parameters.push_back(name);
//
//        unit_types::ParameterModel value;
//        value.editorSettings.type = unit_types::EditorType::String;
//        value.id = QString("%1/%2").arg(group_model.id, "VALUE");
//        value.name = QString::fromLocal8Bit("Значение");
//        group_model.parameters.push_back(value);
//
//        pm.parameters.push_back(group_model);
//    }
//}

void FileItem::RegisterProperty(const QtProperty* property, const QString& id)
{
    propertyToId_[property] = id;
    idToProperty_[id] = property;
}

void FileItem::UnregisterProperty(const QString& id)
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

QtProperty* FileItem::GetProperty(const QString& id)
{
    auto it = idToProperty_.find(id);
    if (it != idToProperty_.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;
}

QString FileItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return QString();
}

unit_types::ParameterModel* FileItem::GetParameterModel(const QString& id)
{
    unit_types::ParameterModel* pm = nullptr;

    {
        QStringList sl = id.split("/");
        auto ql = &model_.parameters;
        QString idt;
        while (sl.size() > 0)
        {
            idt = idt == "" ? sl[0] : idt + "/" + sl[0];
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
                break;
        }
    }

    return pm;
}

unit_types::ParameterModel* FileItem::GetParameterModel(const QtProperty* property)
{
    QString id = GetPropertyId(property);
    if (id == "")
        return nullptr;

    return GetParameterModel(id);
}

//void file_item::SaveExpandState(QtBrowserItem* index)
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> children = index->children();
//    QListIterator<QtBrowserItem*> itChild(children);
//    while (itChild.hasNext())
//        SaveExpandState(itChild.next());
//    QtProperty* prop = index->property();
//
//    auto pm = GetParameterModel(prop);
//    if (pm != nullptr)
//        pm->editorSettings.is_expanded = propertyEditor_->isExpanded(index);
//
//
//
//    //if (pm != nullptr)
//    //    qDebug() << propertyEditor_->isExpanded(index);
//}
//
//void file_item::SaveExpandState()
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
//    QListIterator<QtBrowserItem*> itItem(indexes);
//    while (itItem.hasNext())
//        SaveExpandState(itItem.next());
//}

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
