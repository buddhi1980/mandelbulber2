/*
 * scripts.cpp
 *
 *  Created on: 23 wrz 2023
 *      Author: krzysztof
 */

#include <QJSEngine>
#include <QDebug>

#include "scripts.h"
#include "fractal_container.hpp"
#include "parameters.hpp"

cScripts::cScripts()
{
	// TODO Auto-generated constructor stub
}

QString cScripts::EvaluateParameter(const std::shared_ptr<cParameterContainer> &params,
	const QString &parameterName, cOneParameter &parameter, QString &error)
{
	QString script = parameter.GetScript();
	if (script.length() > 0)
	{
		qDebug() << script;
		int i = 0;
		bool hasError = false;

		while (i < script.length())
		{
			int firstQuote = script.indexOf('\'', i);
			if (firstQuote < 0) break; // quote not found

			int lastQuote = script.indexOf('\'', firstQuote + 1);
			QString scriptParameterName = script.mid(firstQuote + 1, lastQuote - firstQuote - 1);

			QChar vectorComponent = QChar::Null;
			if (scriptParameterName.at(scriptParameterName.length() - 2) == '.')
			{
				vectorComponent = scriptParameterName.at(scriptParameterName.length() - 1);
				scriptParameterName = scriptParameterName.left(scriptParameterName.length() - 2);
			}

			if (params->IfExists(scriptParameterName))
			{
				QString value = params->Get<QString>(scriptParameterName);

				if (vectorComponent != QChar::Null)
				{
					QStringList split = value.split(' ');
					switch (vectorComponent.toLatin1())
					{
						case 'x': value = split.at(0); break;
						case 'y': value = split.at(1); break;
						case 'z': value = split.at(2); break;
						case 'w': value = split.at(3); break;
						default: value = "";
					}
				}

				script.replace(firstQuote, lastQuote - firstQuote + 1, value);
				qDebug() << script;
			}
			else
			{
				error += QString("In the script for parameter '%1' there is unknown parameter '%2'\n")
									 .arg(parameterName)
									 .arg(scriptParameterName);
				hasError = true;
			}
			i = lastQuote + 1;
		}
		if (!hasError)
		{
			QJSEngine myEngine;
			QJSValue scriptValue = myEngine.evaluate(script);
			if (scriptValue.isError())
			{
				error += QString("In the script for parameter '%1' there is an error:\n%2\n")
									 .arg(parameterName)
									 .arg(scriptValue.toString());
			}
			else
			{
				qDebug() << scriptValue.toNumber();
				parameter.Set(scriptValue.toNumber(), parameterContainer::valueActual);
				params->SetFromOneParameter(parameterName, parameter);
			}
		}
	}
	return script;
}

QString cScripts::EvaluateAll(const std::shared_ptr<cParameterContainer> params,
	const std::shared_ptr<cFractalContainer> fractal)
{
	QString error;

	QList<QString> listOfParametrs = params->GetListOfParameters();

	for (const QString &parameterName : listOfParametrs)
	{
		cOneParameter parameter = params->GetAsOneParameter(parameterName);
		QString script = EvaluateParameter(params, parameterName, parameter, error);
	}
	if (error.length() > 0)
	{
		qDebug().noquote() << error;
	}
	return error;
}
