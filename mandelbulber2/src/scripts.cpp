/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
 */

#include "scripts.h"

#include <QDebug>
#include <QJSEngine>
#include <QRegularExpression>

#include "container_selector.hpp"
#include "fractal_container.hpp"
#include "parameters.hpp"

cScripts::cScripts()
{
	// TODO Auto-generated constructor stub
}

double cScripts::EvaluateScript(
	const QString &script, const QString &parameterName, QString &error, QString &evaluation)
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
		evaluation += QString("value: %1\n").arg(scriptValue.toNumber());
		return scriptValue.toNumber();
	}
}

QString cScripts::EvaluateParameter(const std::shared_ptr<cParameterContainer> &params,
	const std::shared_ptr<cFractalContainer> fractal, const QString &parameterName,
	cOneParameter &parameter, QString &error, QString &evaluation)
{
	QString script = parameter.GetScript();
	if (script.length() > 0)
	{
		evaluation += QString("Evaluation of script for parameter %1_%2\n")
										.arg(parameter.GetOriginalContainerName())
										.arg(parameterName);
		evaluation += QString("Script: %1\n").arg(script);

		if (script.left(1) == "#")
		{
			evaluation += "Skipping evaluation\n";
			return script;
		}

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
			QString scriptFullParameterName = script.mid(firstQuote + 1, lastQuote - firstQuote - 1);

			if (scriptFullParameterName.left(4) != "main" && scriptFullParameterName.left(7) != "fractal")
			{
				// can be used parameter name without "main" prefix
				scriptFullParameterName = QString("main_") + scriptFullParameterName;
			}

			QChar vectorComponent = QChar::Null;
			if (scriptFullParameterName.at(scriptFullParameterName.length() - 2) == '.')
			{
				vectorComponent = scriptFullParameterName.at(scriptFullParameterName.length() - 1);
				scriptFullParameterName =
					scriptFullParameterName.left(scriptFullParameterName.length() - 2);
			}

			const std::shared_ptr<cParameterContainer> container =
				parameterContainer::ContainerSelector(scriptFullParameterName, params, fractal);

			int firstDashIndex = scriptFullParameterName.indexOf("_");
			QString scriptParameterName = scriptFullParameterName.mid(firstDashIndex + 1);

			if (container->IfExists(scriptParameterName))
			{
				cOneParameter oneParameter = container->GetAsOneParameter(scriptParameterName);

				// recursive calculation of parameters
				EvaluateParameter(params, fractal, scriptParameterName, oneParameter, error, evaluation);

				QString value = oneParameter.Get<QString>(parameterContainer::valueActual);

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
				evaluation +=
					QString("Value of parameter '%1': %2\n").arg(scriptFullParameterName).arg(value);

				value.replace(',', '.');

				script.replace(firstQuote, lastQuote - firstQuote + 1, value);
				// correction of cursor by replacement length difference
				lastQuote += value.length() - (lastQuote - firstQuote + 1);

				evaluation += QString("Script after substitution: '%1'\n").arg(script);
			}
			else
			{
				error += QString("In the script for parameter '%1' there is unknown parameter '%2'\n")
									 .arg(parameterName)
									 .arg(scriptFullParameterName);
				hasError = true;
			}
			i = lastQuote + 1;
		}

		script.replace("pi()", "3.14159265359");

		// regex (?<!Math\.)\b(sin|cos|tan)\b - any fuction without Math. prefix
		QRegularExpression re(
			"(?<!Math\\.)\\b(abs|acos|asin|atan|atan2|ceil|cos|exp|floor|log|max|min|pow|random|round|"
			"sin|sqrt|tan)\\b");
		script.replace(re, "Math.\\1");
		evaluation += QString("Adding Math. prefixes: %1\n").arg(script);

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
						QString subScript = script.mid(firstPosition + 1, secondPosition - firstPosition);

						evaluation += QString("Evaluation of component '%1' with script: %2\n")
														.arg(component)
														.arg(subScript);

						double value = EvaluateScript(
							subScript, QString("%1.%2").arg(parameterName).arg(component), error, evaluation);

						if (!qIsNaN(value))
						{
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
						}

						i = secondPosition + 1;
					}

					break;
				}
				default:
				{
					double value = EvaluateScript(script, parameterName, error, evaluation);
					if (!qIsNaN(value))
					{
						parameter.Set(value, parameterContainer::valueActual);
					}
					break;
				}
			}

			const std::shared_ptr<cParameterContainer> containerOut =
				parameterContainer::ContainerSelector(
					parameter.GetOriginalContainerName() + "_" + parameterName, params, fractal);
			containerOut->SetFromOneParameter(parameterName, parameter);
		}

		evaluation += QString("Finished evaluation of script for parameter %1_%2\n")
										.arg(parameter.GetOriginalContainerName())
										.arg(parameterName);
	}
	return script;
}

QString cScripts::EvaluateAll(const std::shared_ptr<cParameterContainer> params,
	const std::shared_ptr<cFractalContainer> fractal)
{
	QString error;
	QString evaluation;

	QList<QString> listOfParametrs = params->GetListOfParameters();

	for (const QString &parameterName : listOfParametrs)
	{
		cOneParameter parameter = params->GetAsOneParameter(parameterName);
		QString script =
			EvaluateParameter(params, fractal, parameterName, parameter, error, evaluation);
	}

	QList<QString> listOfFractalParametrs = fractal->at(0)->GetListOfParameters();
	for (int f = 0; f < NUMBER_OF_FRACTALS; f++)
	{
		for (const QString &parameterName : listOfFractalParametrs)
		{
			cOneParameter parameter = fractal->at(f)->GetAsOneParameter(parameterName);
			QString script =
				EvaluateParameter(params, fractal, parameterName, parameter, error, evaluation);
		}
	}

	if (error.length() > 0)
	{
		qInfo().noquote() << error;
	}
	return error;
}
