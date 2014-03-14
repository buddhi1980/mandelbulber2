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

#ifndef SHADERS_HPP_
#define SHADERS_HPP_

#include <string>
#include <map>
#include "algebra.hpp"
#include "cimage.hpp"

namespace parameters
{
enum varType
{
	typeInt, typeDouble, typeString, typeVector3, typeRgb, typeBool, typeColorPalette
};

struct sMultiVal
{
	double dVal[4];
	int iVal[4];
	std::string sVal;
};

struct sRecord
{
	varType type;
	sMultiVal actualVal;
	sMultiVal defaultVal;
	sMultiVal minVal;
	sMultiVal maxVal;
	bool morphable;
	bool limitsDefined;
	bool toSave;
	bool appParam;
};

class container
{
public:
	container();
	~container();
	template <class T> void addParam(std::string name, T defaultVal, bool morphable);
	template <class T> void addParam(std::string name, T defaultVal, T minVal, T maxVal, bool morphable);
	template <class T> void addParam(std::string name, int index, T defaultVal, bool morphable);
	template <class T> void addParam(std::string name, int index, T defaultVal, T minVal, T maxVal, bool morphable);

	template <class T> void Set(std::string name, T val);
	template <class T> void Set(std::string name, int index, T val);

	template <class T> T Get(std::string name);
	template <class T> T Get(std::string name, int index);

	void SetToSave(std::string, bool toSave);
	void SetToSave(std::string, int index, bool toSave);
	void SetAsAppParam(std::string name, bool asAppParam);
	void SetAsAppParam(std::string name, int index, bool asAppParam);

	void Copy(std::string name, container *sourceContainer);

	void DebugPrintf(std::string name);

private:
	varType Assigner(sMultiVal &multi, double val);
	varType Assigner(sMultiVal &multi, int val);
	varType Assigner(sMultiVal &multi, std::string val);
	varType Assigner(sMultiVal &multi, CVector3 val);
	varType Assigner(sMultiVal &multi, sRGB val);
	varType Assigner(sMultiVal &multi, bool val);
	varType Assigner(sMultiVal &multi, sRGB val[256]);
	varType Getter(sMultiVal multi, double &val);
	varType Getter(sMultiVal multi, int &val);
	varType Getter(sMultiVal multi, std::string &val);
	varType Getter(sMultiVal multi, CVector3 &val);
	varType Getter(sMultiVal multi, sRGB &val);
	varType Getter(sMultiVal multi, bool &val);
	varType Getter(sMultiVal multi, sRGB val[256]);
	std::string nameWithIndex(std::string *str, int index);
	void clearMultiVal(sMultiVal &multiVal);
	std::string MakePaletteString(sRGB palette[256]);

	//std::map contairer
	std::map<std::string, sRecord> myMap;
};

}

extern parameters::container gPar;
#endif /*SHADERS_HPP_*/
