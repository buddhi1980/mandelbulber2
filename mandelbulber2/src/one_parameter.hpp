/*
 * one_parameter.hpp
 *
 *  Created on: Dec 20, 2014
 *      Author: krzysztof
 */

#ifndef SRC_ONE_PARAMETER_HPP_
#define SRC_ONE_PARAMETER_HPP_

#include "multi_val.hpp"

namespace parameterContainer
{
	enum enumParameterType
	{
		paramStandard, paramApp, paramNoSave
	};

	enum enumMorphType
	{
		morphNone, morphLinear, morphCatMullRom, morphCatMullRomAngle
	};

	enum enumValueSelection
	{
		valueDefault,	valueActual, valueMin, valueMax
	};
}
using namespace parameterContainer;

class cOneParameter
{
public:
	cOneParameter();
	~cOneParameter();
	enumVarType GetValueType() const {return defaultVal.GetDefaultype();}
	enumParameterType GetParameterType() const {return parType;}
	enumMorphType GetMorphType() const {return morphType;}
	QString GetOriginalContainerName() const {return originalContainer;}
	void SetParameterType(enumParameterType _parType) {parType = _parType;}
	void SetMorphType(enumMorphType _morphType) {morphType = _morphType;}
	void SetOriginalContainerName(const QString &containerName) {originalContainer = containerName;}
	bool isDefaultValue() const;
	cMultiVal GetMultival(enumValueSelection selection);
	void SetMultival(cMultiVal multi, enumValueSelection selection);

	template <class T> void Set(T val, enumValueSelection selection);
	template <class T> T Get(enumValueSelection selection) const;

private:
	//parameter data
	enumMorphType morphType;
	enumParameterType parType;
	cMultiVal actualVal;
	cMultiVal defaultVal;
	cMultiVal minVal;
	cMultiVal maxVal;
	QString originalContainer;
	bool limitsDefined;
};

#endif /* SRC_ONE_PARAMETER_HPP_ */
