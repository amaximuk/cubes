#pragma once

#include "unit_types.h"
#include "unit_parameter_model_id.h"
#include "unit_parameter_model.h"

namespace CubesUnit
{

	inline QString GetUniqueName(QString baseName, QString delimiter, QStringList busyNames)
	{
		QString name = baseName;
		int counter = 0;
		while (busyNames.contains(name))
			name = QString("%1%2%3").arg(baseName, delimiter).arg(++counter);
		return name;
	}

	inline CubesUnit::ParameterModel* GetParameterModel(CubesUnit::ParameterModels& models, const CubesUnit::ParameterModelId& id)
	{
		CubesUnit::ParameterModel* pm = nullptr;

		{
			auto sl = id.split();
			auto ql = &models;
			CubesUnit::ParameterModelId idt;
			while (sl.size() > 0)
			{
				idt += sl[0];
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
}
