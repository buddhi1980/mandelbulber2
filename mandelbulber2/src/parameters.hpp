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

class cParameterContainer
{
public:
	cParameterContainer();
	~cParameterContainer();
	template <class T> void addParam(QString name, T defaultVal, bool morphable);
	template <class T> void addParam(QString name, T defaultVal, T minVal, T maxVal, bool morphable);
	template <class T> void addParam(QString name, int index, T defaultVal, bool morphable);
	template <class T> void addParam(QString name, int index, T defaultVal, T minVal, T maxVal, bool morphable);

	template <class T> void Set(QString name, T val);
	template <class T> void Set(QString name, int index, T val);

	template <class T> T Get(QString name) const;
	template <class T> T Get(QString name, int index) const;

	void SetToSave(QString, bool toSave);
	void SetToSave(QString, int index, bool toSave);
	void SetAsAppParam(QString name, bool asAppParam);
	void SetAsAppParam(QString name, int index, bool asAppParam);

	void Copy(QString name, cParameterContainer *sourceContainer);
	QList<QString> GetListOfParameters(void) const;

	void DebugPrintf(QString name);

private:
	enum varType
	{
		typeInt, typeDouble, typeString, typeVector3, typeRgb, typeBool, typeColorPalette
	};

	struct sMultiVal
	{
		double dVal[4];
		int iVal[4];
		QString sVal;
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

	varType Assigner(sMultiVal &multi, double val);
	varType Assigner(sMultiVal &multi, int val);
	varType Assigner(sMultiVal &multi, QString val);
	varType Assigner(sMultiVal &multi, CVector3 val);
	varType Assigner(sMultiVal &multi, sRGB val);
	varType Assigner(sMultiVal &multi, bool val);
	varType Assigner(sMultiVal &multi, cColorPalette *val);
	varType Getter(sMultiVal multi, double &val) const;
	varType Getter(sMultiVal multi, int &val) const;
	varType Getter(sMultiVal multi, QString &val) const;
	varType Getter(sMultiVal multi, CVector3 &val) const;
	varType Getter(sMultiVal multi, sRGB &val) const;
	varType Getter(sMultiVal multi, bool &val) const;
	varType Getter(sMultiVal multi, cColorPalette &val) const;
	QString nameWithIndex(QString *str, int index) const;
	void clearMultiVal(sMultiVal &multiVal);
	QString MakePaletteString(cColorPalette *palette);

	//std::map contairer
	QMap<QString, sRecord> myMap;
};


extern cParameterContainer *gPar;
extern cParameterContainer *gParFractal;
#endif /*SHADERS_HPP_*/
