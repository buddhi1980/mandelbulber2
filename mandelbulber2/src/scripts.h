/*
 * scripts.h
 *
 *  Created on: 23 wrz 2023
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_SCRIPTS_H_
#define MANDELBULBER2_SRC_SCRIPTS_H_

#include <memory>
#include <QString>

class cParameterContainer;
class cFractalContainer;
class cOneParameter;

class cScripts
{
public:
	cScripts();
	static QString EvaluateAll(const std::shared_ptr<cParameterContainer> params,
		const std::shared_ptr<cFractalContainer> fractal);

private:
	static QString EvaluateParameter(const std::shared_ptr<cParameterContainer> &params,
		const QString &parameterName, cOneParameter &parameter, QString &error);
};

#endif /* MANDELBULBER2_SRC_SCRIPTS_H_ */
