/*********************************************************
 /                   MANDELBULBER
 / class for parameter handling
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/
#include "parameters.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//#define _PARAM_DEBUG

parameters::container gPar;

using namespace parameters;

container::container()
{
	myMap.clear();
}

container::~container()
{
	myMap.clear();
}

//defining of params without limits
template<class T>
void container::addParam(std::string name, T defaultVal, bool morphable)
{
	sRecord newRecord;
	sMultiVal multi;
	newRecord.type = Assigner(multi, defaultVal);
	newRecord.actualVal = multi;
	newRecord.defaultVal = multi;
	newRecord.morphable = morphable;
	newRecord.limitsDefined = false;
	newRecord.toSave = true;

	std::pair<std::map<std::string, sRecord>::iterator, bool> ret;
	ret = myMap.insert(std::pair<std::string, sRecord>(name, newRecord));

	if (ret.second == false)
	{
		std::cerr << "addParam(): element '" << name << "' already existed" << std::endl;
	}
}
template void container::addParam<double>(std::string name, double defaultVal, bool morphable);
template void container::addParam<int>(std::string name, int defaultVal, bool morphable);
template void container::addParam<std::string>(std::string name, std::string defaultVal, bool morphable);
template void container::addParam<CVector3>(std::string name, CVector3 defaultVal, bool morphable);
template void container::addParam<sRGB>(std::string name, sRGB defaultVal, bool morphable);
template void container::addParam<bool>(std::string name, bool defaultVal, bool morphable);
template void container::addParam<sRGB*>(std::string name, sRGB *defaultVal, bool morphable);

//defining of params with limits
template<class T>
void container::addParam(std::string name, T defaultVal, T minVal, T maxVal, bool morphable)
{
	sRecord newRecord;
	sMultiVal multi;
	newRecord.type = Assigner(multi, defaultVal);
	newRecord.actualVal = multi;
	newRecord.defaultVal = multi;

	Assigner(newRecord.minVal, minVal);
	Assigner(newRecord.maxVal, maxVal);
	newRecord.morphable = morphable;
	newRecord.limitsDefined = true;
	newRecord.toSave = true;

	std::pair<std::map<std::string, sRecord>::iterator, bool> ret;
	ret = myMap.insert(std::pair<std::string, sRecord>(name, newRecord));

	if (ret.second == false)
	{
		std::cerr << "addParam(): element '" << name << "' already existed" << std::endl;
	}
}
template void container::addParam<double>(std::string name, double defaultVal, double minVal, double maxVal, bool morphable);
template void container::addParam<int>(std::string name, int defaultVal, int minVal, int maxVal, bool morphable);
template void container::addParam<CVector3>(std::string name, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, bool morphable);
template void container::addParam<sRGB>(std::string name, sRGB defaultVal, sRGB minVal, sRGB maxVal, bool morphable);

//defining of params without limits and with index
template<class T>
void container::addParam(std::string name, int index, T defaultVal, bool morphable)
{
	if (index >= 0)
	{
		sRecord newRecord;
		sMultiVal multi;
		newRecord.type = Assigner(multi, defaultVal);
		newRecord.actualVal = multi;
		newRecord.defaultVal = multi;
		newRecord.morphable = morphable;
		newRecord.limitsDefined = false;
		newRecord.toSave = true;

		std::string indexName = nameWithIndex(&name, index);
		std::pair<std::map<std::string, sRecord>::iterator, bool> ret;
		ret = myMap.insert(std::pair<std::string, sRecord>(indexName, newRecord));

		if (ret.second == false)
		{
			std::cerr << "addParam(): element '" << indexName << "' already existed" << std::endl;
		}
	}
	else
	{
		std::cerr << "addParam(): element '" << name << "' has negative index (" << index << ")" << std::endl;
	}
}
template void container::addParam<double>(std::string name, int index, double defaultVal, bool morphable);
template void container::addParam<int>(std::string name, int index, int defaultVal, bool morphable);
template void container::addParam<std::string>(std::string name, int index, std::string defaultVal, bool morphable);
template void container::addParam<CVector3>(std::string name, int index, CVector3 defaultVal, bool morphable);
template void container::addParam<sRGB>(std::string name, int index, sRGB defaultVal, bool morphable);
template void container::addParam<bool>(std::string name, int index, bool defaultVal, bool morphable);

//defining of params with limits and index
template<class T>
void container::addParam(std::string name, int index, T defaultVal, T minVal, T maxVal, bool morphable)
{
	if (index >= 0)
	{
		sRecord newRecord;
		sMultiVal multi;
		newRecord.type = Assigner(multi, defaultVal);
		newRecord.actualVal = multi;
		newRecord.defaultVal = multi;

		Assigner(newRecord.minVal, minVal);
		Assigner(newRecord.maxVal, maxVal);
		newRecord.morphable = morphable;
		newRecord.limitsDefined = true;
		newRecord.toSave = true;

		std::string indexName = nameWithIndex(&name, index);
		std::pair<std::map<std::string, sRecord>::iterator, bool> ret;
		ret = myMap.insert(std::pair<std::string, sRecord>(indexName, newRecord));

		if (ret.second == false)
		{
			std::cerr << "addParam(): element '" << indexName << "' already existed" << std::endl;
		}
	}
	else
	{
		std::cerr << "addParam(): element '" << name << "' has negative index (" << index << ")" << std::endl;
	}
}
template void container::addParam<double>(std::string name, int index, double defaultVal, double minVal, double maxVal, bool morphable);
template void container::addParam<int>(std::string name, int index, int defaultVal, int minVal, int maxVal, bool morphable);
template void container::addParam<CVector3>(std::string name, int index, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, bool morphable);
template void container::addParam<sRGB>(std::string name, int index, sRGB defaultVal, sRGB minVal, sRGB maxVal, bool morphable);

varType container::Assigner(sMultiVal &multi, double val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	snprintf(sbuff, 100, "%.16lg", val);
	multi.sVal = std::string(sbuff);
	return typeDouble;
}

varType container::Assigner(sMultiVal &multi, int val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	snprintf(sbuff, 100, "%d", val);
	multi.sVal = std::string(sbuff);
	return typeInt;
}

varType container::Assigner(sMultiVal &multi, std::string val)
{
	clearMultiVal(multi);
	sscanf(val.c_str(), "%lf %lf %lf %lf", &multi.dVal[0], &multi.dVal[1], &multi.dVal[2], &multi.dVal[3]);
	sscanf(val.c_str(), "%d %d %d %d", &multi.iVal[0], &multi.iVal[1], &multi.iVal[2], &multi.iVal[3]);
	multi.sVal = val;
	return typeString;
}

varType container::Assigner(sMultiVal &multi, CVector3 val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val.x;
	multi.dVal[1] = val.y;
	multi.dVal[2] = val.z;
	multi.iVal[0] = val.x;
	multi.iVal[1] = val.y;
	multi.iVal[2] = val.z;
	char sbuff[100];
	snprintf(sbuff, 100, "%.16lg %.16lg %.16lg", val.x, val.y, val.z);
	multi.sVal = std::string(sbuff);
	return typeVector3;
}

varType container::Assigner(sMultiVal &multi, sRGB val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val.R;
	multi.dVal[1] = val.G;
	multi.dVal[2] = val.B;
	multi.iVal[0] = val.R;
	multi.iVal[1] = val.G;
	multi.iVal[2] = val.B;
	char sbuff[100];
	snprintf(sbuff, 100, "%x %x %x", val.R, val.G, val.B);
	multi.sVal = std::string(sbuff);
	return typeRgb;
}

varType container::Assigner(sMultiVal &multi, bool val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	snprintf(sbuff, 100, "%d", val);
	multi.sVal = std::string(sbuff);
	return typeBool;
}

varType container::Assigner(sMultiVal &multi, sRGB val[256])
{
	clearMultiVal(multi);
//	multi.sVal = MakePaletteString(val);
	return typeBool;
}

varType container::Getter(sMultiVal multi, double &val)
{
	val = multi.dVal[0];
	return typeDouble;
}

varType container::Getter(sMultiVal multi, int &val)
{
	val = multi.iVal[0];
	return typeInt;
}

varType container::Getter(sMultiVal multi, CVector3 &val)
{
	val = CVector3(multi.dVal[0], multi.dVal[1], multi.dVal[2]);
	return typeVector3;
}

varType container::Getter(sMultiVal multi, std::string &val)
{
	val = multi.sVal;
	return typeString;
}

varType container::Getter(sMultiVal multi, sRGB &val)
{
	val = (sRGB){multi.iVal[0], multi.iVal[1], multi.iVal[2]};
	return typeRgb;
}

varType container::Getter(sMultiVal multi, bool &val)
{
	val = multi.iVal[0];
	return typeBool;
}

//set parameter value by name
template<class T>
void container::Set(std::string name, T val)
{
	std::map<std::string, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sMultiVal multi;
		varType type = Assigner(multi, val);
		if (it->second.type == type)
		{
			it->second.actualVal = multi;
		}
#ifdef _PARAM_DEBUG
		else
		{
			std::cerr << "Set(): element '" << name << "' got value of not default type" << std::endl;
			DebugPrintf(name);
		}
#endif
	}
	else
	{
		std::cerr << "Set(): element '" << name << "' doesn't exists" << std::endl;
	}
}
template void container::Set<double>(std::string name, double val);
template void container::Set<int>(std::string name, int val);
template void container::Set<std::string>(std::string name, std::string val);
template void container::Set<CVector3>(std::string name, CVector3 val);
template void container::Set<sRGB>(std::string name, sRGB val);
template void container::Set<bool>(std::string name, bool val);

//set parameter value by name and index
template<class T>
void container::Set(std::string name, int index, T val)
{
	if (index >= 0)
	{
		std::string indexName = nameWithIndex(&name, index);
		std::map<std::string, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sMultiVal multi;
			varType type = Assigner(multi, val);
			if (it->second.type == type)
			{
				it->second.actualVal = multi;
			}
#ifdef _PARAM_DEBUG
			else
			{
				std::cerr << "Set(): element '" << name << "' got value of not default type" << std::endl;
				DebugPrintf(name);
			}
#endif
		}
		else
		{
			std::cerr << "Set(): element '" << indexName << "' doesn't exists" << std::endl;
		}
	}
	else
	{
		std::cerr << "Set(): element '" << name << "' has negative index (" << index << ")" << std::endl;
	}
}
template void container::Set<double>(std::string name, int index,double val);
template void container::Set<int>(std::string name, int index,int val);
template void container::Set<std::string>(std::string name, int index,std::string val);
template void container::Set<CVector3>(std::string name, int index,CVector3 val);
template void container::Set<sRGB>(std::string name, int index,sRGB val);
template void container::Set<bool>(std::string name, int index,bool val);

//get parameter value by name
template<class T>
T container::Get(std::string name)
{
	std::map<std::string, sRecord>::iterator it;
	it = myMap.find(name);
	T val;
	if (it != myMap.end())
	{
		sRecord rec = it->second;
		sMultiVal multi = rec.actualVal;
		varType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
		if (it->second.type != type)
		{
			std::cerr << "Get(): element '" << name << "' gave value of not default type" << std::endl;
			DebugPrintf(name);
		}
#endif
	}
	else
	{
		std::cerr << "Get(): element '" << name << "' doesn't exists" << std::endl;
	}
	return val;
}
template double container::Get<double>(std::string name);
template int container::Get<int>(std::string name);
template std::string container::Get<std::string>(std::string name);
template CVector3 container::Get<CVector3>(std::string name);
template sRGB container::Get<sRGB>(std::string name);
template bool container::Get<bool>(std::string name);

//get parameter value by name and index
template<class T>
T container::Get(std::string name, int index)
{
	T val;
	if (index >= 0)
	{
		std::string indexName = nameWithIndex(&name, index);
		std::map<std::string, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sRecord rec = it->second;
			sMultiVal multi = rec.actualVal;
			varType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
			if (it->second.type != type)
			{
				std::cerr << "Get(): element '" << indexName << "' gave value of not default type" << std::endl;
				DebugPrintf(indexName);
			}
#endif
		}
		else
		{
			std::cerr << "Get(): element '" << indexName << "' doesn't exists" << std::endl;
		}
	}
	else
	{
		std::cerr << "Get(): element '" << name << "' has negative index (" << index << ")" << std::endl;
	}
	return val;
}
template double container::Get<double>(std::string name, int index);
template int container::Get<int>(std::string name, int index);
template std::string container::Get<std::string>(std::string name, int index);
template CVector3 container::Get<CVector3>(std::string name, int index);
template sRGB container::Get<sRGB>(std::string name, int index);
template bool container::Get<bool>(std::string name, int index);

void container::DebugPrintf(std::string name)
{
	std::map<std::string, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sRecord rec = it->second;
		printf("Actual value of variable '%s'\n", name.c_str());
		printf("int = %d; %d; %d; %d\n", rec.actualVal.iVal[0], rec.actualVal.iVal[1], rec.actualVal.iVal[2], rec.actualVal.iVal[3]);
		printf("double = %.16lg; %.16lg; %.16lg; %.16lg\n", rec.actualVal.dVal[0], rec.actualVal.dVal[1], rec.actualVal.dVal[2], rec.actualVal.dVal[3]);
		printf("string = %s\n", rec.actualVal.sVal.c_str());
		switch (rec.type)
		{
			case typeInt:
				printf("variable type 'int'\n");
				break;
			case typeDouble:
				printf("variable type 'double'\n");
				break;
			case typeVector3:
				printf("variable type 'vector3'\n");
				break;
			case typeString:
				printf("variable type 'string'\n");
				break;
			case typeRgb:
				printf("variable type 'rgb'\n");
				break;
			case typeBool:
				printf("variable type 'bool'\n");
				break;
			case typeColorPalette:
				printf("variable type 'colorPalette'\n");
				break;
		}
	}
	else
	{
		std::cerr << "DebugPrintf(): element '" << name << "' doesn't exists" << std::endl;
	}
}

std::string container::nameWithIndex(std::string *str, int index)
{
	char name[256];
	sprintf(name, "%s_%d", str->c_str(), index);
	std::string out(name);
	return out;
}

void container::clearMultiVal(sMultiVal &multiVal)
{
	multiVal.iVal[0] = 0;
	multiVal.iVal[1] = 0;
	multiVal.iVal[2] = 0;
	multiVal.iVal[3] = 0;
	multiVal.dVal[0] = 0.0;
	multiVal.dVal[1] = 0.0;
	multiVal.dVal[2] = 0.0;
	multiVal.dVal[3] = 0.0;
	multiVal.sVal.clear();
}

std::string container::MakePaletteString(sRGB palette[256])
{
	int length;
	int pointer = 0;
	char *paletteString = new char[257 * 7];
	for (int i = 0; i < 256; i++)
	{
		int colour = palette[i].R * 65536 + palette[i].G * 256 + palette[i].B;
		colour = colour & 0x00FFFFFF;
		length = sprintf(&paletteString[pointer], "%x ", colour);
		pointer += length;
	}
	std::string out(paletteString);
	return out;
}

void container::SetToSave(std::string name, bool toSave)
{
	std::map<std::string, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it->second.toSave = toSave;
	}
	else
	{
		std::cerr << "SetToSave(): element '" << name << "' doesn't exists" << std::endl;
	}
}

void container::SetToSave(std::string name, int index, bool toSave)
{
	if (index >= 0)
	{
		std::string indexName = nameWithIndex(&name, index);
		std::map<std::string, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			it->second.toSave = toSave;
		}
		else
		{
			std::cerr << "SetToSave(): element '" << indexName << "' doesn't exists" << std::endl;
		}
	}
	else
	{
		std::cerr << "SetToSave(): element '" << name << "' has negative index (" << index << ")" << std::endl;
	}
}

void container::Copy(std::string name, container *sourceContainer)
{
	std::map<std::string, sRecord>::iterator itSource, itDest;
	itDest = myMap.find(name);
	if (itDest != myMap.end())
	{
		itSource = sourceContainer->myMap.find(name);
		if (itDest != myMap.end())
		{
			itDest->second = itSource->second;
		}
		else
		{
			std::cerr << "CopyPar(): source element '" << name << "' doesn't exists" << std::endl;
		}
	}
	else
	{
		std::cerr << "CopyPar(): destination element '" << name << "' doesn't exists" << std::endl;
	}
}
