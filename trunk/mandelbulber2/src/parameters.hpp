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

#include <QtCore>
#include "algebra.hpp"
#include "color_structures.hpp"
#include "color_palette.hpp"

namespace parameterContainer
{
enum enumParameterType
{
	paramStandard, paramApp, paramNoSave
};

enum enumMorphType
{
	morphNone, morphLinear, morphcatMullRom
};
}

using namespace parameterContainer;
class cParameterContainer
{
public:
	enum enumVarType
	{
		null, typeInt, typeDouble, typeString, typeVector3, typeRgb, typeBool, typeColorPalette
	};

	cParameterContainer();
	~cParameterContainer();
	template <class T> void addParam(QString name, T defaultVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, int index, T defaultVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, int index, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType);

	template <class T> void Set(QString name, T val);
	template <class T> void Set(QString name, int index, T val);

	template <class T> T Get(QString name) const;
	template <class T> T Get(QString name, int index) const;

	enumVarType GetVarType(QString name) const;
	enumParameterType GetParameterType(QString name) const;
	bool isDefaultValue(QString name) const;

	void Copy(QString name, cParameterContainer *sourceContainer);
	QList<QString> GetListOfParameters(void) const;

	void DebugPrintf(QString name);



private:


	struct sMultiVal
	{
		double dVal[4];
		int iVal[4];
		QString sVal;
	};

	struct sRecord
	{
		enumVarType type;
		enumMorphType morphType;
		enumParameterType parType;
		sMultiVal actualVal;
		sMultiVal defaultVal;
		sMultiVal minVal;
		sMultiVal maxVal;
		bool limitsDefined;
	};

	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, double val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, int val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, QString val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, CVector3 val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, sRGB val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, bool val);
	enumVarType Assigner(sMultiVal &multi, enumVarType defaultType, cColorPalette val);
	enumVarType Getter(sMultiVal multi, double &val) const;
	enumVarType Getter(sMultiVal multi, int &val) const;
	enumVarType Getter(sMultiVal multi, QString &val) const;
	enumVarType Getter(sMultiVal multi, CVector3 &val) const;
	enumVarType Getter(sMultiVal multi, sRGB &val) const;
	enumVarType Getter(sMultiVal multi, bool &val) const;
	enumVarType Getter(sMultiVal multi, cColorPalette &val) const;
	QString nameWithIndex(QString *str, int index) const;
	void clearMultiVal(sMultiVal &multiVal);
	QString MakePaletteString(cColorPalette &palette);
	cColorPalette GetPaletteFromString(const QString &paletteString) const;

	static bool compareStrings(const QString &p1, const QString &p2)
	{
	    if(p1.toLower() < p2.toLower())
	    		return true;
	    else
	        return false;
	}


	//std::map contairer
	QMap<QString, sRecord> myMap;
};


extern cParameterContainer *gPar;
extern cParameterContainer *gParFractal;
#endif /*SHADERS_HPP_*/
