/*
 * one_parameter.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: krzysztof
 */

#include "one_parameter.hpp"

using namespace parameterContainer;

cOneParameter::cOneParameter()
{
	morphType = morphNone;
	parType = paramStandard;
	limitsDefined = false;
}

cOneParameter::~cOneParameter()
{
}

//set parameter value
template<class T>
void cOneParameter::Set(T val, enumValueSelection selection)
{
	switch(selection)
	{
		case valueActual:
			actualVal.Store(val);
			break;
		case valueDefault:
			defaultVal.Store(val);
			break;
		case valueMin:
			minVal.Store(val);
			limitsDefined = true;
			break;
		case valueMax:
			maxVal.Store(val);
			limitsDefined = true;
			break;
	}
}
template void cOneParameter::Set<double>(double val, enumValueSelection selection);
template void cOneParameter::Set<int>(int val, enumValueSelection selection);
template void cOneParameter::Set<QString>(QString val, enumValueSelection selection);
template void cOneParameter::Set<CVector3>(CVector3 val, enumValueSelection selection);
template void cOneParameter::Set<sRGB>(sRGB val, enumValueSelection selection);
template void cOneParameter::Set<bool>(bool val, enumValueSelection selection);
template void cOneParameter::Set<cColorPalette>(cColorPalette val, enumValueSelection selection);

//get parameter value
template<class T>
T cOneParameter::Get(enumValueSelection selection) const
{
	T val = T();
	actualVal.Get(val);

	switch(selection)
	{
		case valueActual:
			actualVal.Get(val);
			break;
		case valueDefault:
			defaultVal.Get(val);
			break;
		case valueMin:
			minVal.Get(val);
			break;
		case valueMax:
			maxVal.Get(val);
			break;
	}
	return val;
}
template double cOneParameter::Get<double>(enumValueSelection selection) const;
template int cOneParameter::Get<int>(enumValueSelection selection) const;
template QString cOneParameter::Get<QString>(enumValueSelection selection) const;
template CVector3 cOneParameter::Get<CVector3>(enumValueSelection selection) const;
template sRGB cOneParameter::Get<sRGB>(enumValueSelection selection) const;
template bool cOneParameter::Get<bool>(enumValueSelection selection) const;
template cColorPalette cOneParameter::Get<cColorPalette>(enumValueSelection selection) const;

bool cOneParameter::isDefaultValue() const
{
	return (actualVal == defaultVal);
}

cMultiVal cOneParameter::GetMultival(enumValueSelection selection)
{
	switch(selection)
	{
		case valueActual:
			return actualVal;
		case valueDefault:
			return defaultVal;
		case valueMin:
			return minVal;
		case valueMax:
			return maxVal;
	}
	return actualVal;
}

void cOneParameter::SetMultival(cMultiVal multi, enumValueSelection selection)
{
	switch(selection)
	{
		case valueActual:
			actualVal = multi;
			break;
		case valueDefault:
			defaultVal = multi;
			break;
		case valueMin:
			minVal = multi;
			break;
		case valueMax:
			maxVal = multi;
			break;
	}
}
