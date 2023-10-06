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

double cScripts::EvaluateScript(const QString &script, const QString &parameterName, QString &error)
{
	QJSEngine myEngine;
	QJSValue scriptValue = myEngine.evaluate(script);
	if (scriptValue.isError())
	{
		error += QString("In the script for parameter '%1' there is an error:\n%2\n")
							 .arg(parameterName)
							 .arg(scriptValue.toString());
		return 0.0;
	}
	else
	{
		qDebug() << scriptValue.toNumber();
		return scriptValue.toNumber();
	}
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

		// replace parameter names by values
		while (i < script.length())
		{
			int firstQuote = script.indexOf('\'', i);
			if (firstQuote < 0) break; // quote not found

			int lastQuote = script.indexOf('\'', firstQuote + 1);

			if (lastQuote < 0)
			{
				error += QString("In the script for parameter '%1' there is missing second quote\n")
									 .arg(parameterName);
				hasError = true;
				break;
			}
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
						default: value = ""; break;
					}
				}

				script.replace(firstQuote, lastQuote - firstQuote + 1, value);
				// correction of cursor by replacement length difference
				lastQuote += value.length() - (lastQuote - firstQuote + 1);

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

		// process scripts
		if (!hasError)
		{
			parameterContainer::enumVarType varType = parameter.GetValueType();

			switch (varType)
			{
				case parameterContainer::typeVector3:
				case parameterContainer::typeVector4:
				{
					i = 0;
					while (i < script.length())
					{
						int firstPosition = script.indexOf(':', i);
						if (firstPosition < 1) break; // it also cannot be first char

						int secondPosition = script.indexOf(':', firstPosition + 1);

						if (secondPosition < 0)
							secondPosition = script.length() - 1;
						else
							secondPosition = secondPosition - 2;

						QChar component = script.at(firstPosition - 1);
						QString subScript = script.mid(firstPosition+1, secondPosition - firstPosition);

						qDebug() << component << subScript;

						double value =
							EvaluateScript(subScript, QString("%1.%2").arg(parameterName).arg(component), error);

						CVector4 vector = parameter.Get<CVector4>(parameterContainer::valueActual);
						switch (component.toLatin1())
						{
							case 'x': vector.x = value; break;
							case 'y': vector.y = value; break;
							case 'z': vector.z = value; break;
							case 'w': vector.w = value; break;
							default:
							{
								hasError = true;
								error += QString("Unknown vector component (%1) of parameter '%2'\n")
													 .arg(component)
													 .arg(parameterName);
							}
						}
						parameter.Set(vector, parameterContainer::valueActual);

						i = secondPosition + 1;
					}

					break;
				}
				default:
				{
					double value = EvaluateScript(script, parameterName, error);
					parameter.Set(value, parameterContainer::valueActual);
					break;
				}
			}
			params->SetFromOneParameter(parameterName, parameter);
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
