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

class container
{
public:
	container();
	~container();
	template <class T> void addParam(QString name, T defaultVal, bool morphable);
	template <class T> void addParam(QString name, T defaultVal, T minVal, T maxVal, bool morphable);
	template <class T> void addParam(QString name, int index, T defaultVal, bool morphable);
	template <class T> void addParam(QString name, int index, T defaultVal, T minVal, T maxVal, bool morphable);

	template <class T> void Set(QString name, T val);
	template <class T> void Set(QString name, int index, T val);

	template <class T> T Get(QString name);
	template <class T> T Get(QString name, int index);

	void SetToSave(QString, bool toSave);
	void SetToSave(QString, int index, bool toSave);
	void SetAsAppParam(QString name, bool asAppParam);
	void SetAsAppParam(QString name, int index, bool asAppParam);

	void Copy(QString name, container *sourceContainer);

	void DebugPrintf(QString name);

private:
	varType Assigner(sMultiVal &multi, double val);
	varType Assigner(sMultiVal &multi, int val);
	varType Assigner(sMultiVal &multi, QString val);
	varType Assigner(sMultiVal &multi, CVector3 val);
	varType Assigner(sMultiVal &multi, sRGB val);
	varType Assigner(sMultiVal &multi, bool val);
	varType Assigner(sMultiVal &multi, sRGB val[256]);
	varType Getter(sMultiVal multi, double &val);
	varType Getter(sMultiVal multi, int &val);
	varType Getter(sMultiVal multi, QString &val);
	varType Getter(sMultiVal multi, CVector3 &val);
	varType Getter(sMultiVal multi, sRGB &val);
	varType Getter(sMultiVal multi, bool &val);
	varType Getter(sMultiVal multi, sRGB val[256]);
	QString nameWithIndex(QString *str, int index);
	void clearMultiVal(sMultiVal &multiVal);
	QString MakePaletteString(sRGB palette[256]);

	//std::map contairer
	QMap<QString, sRecord> myMap;
};

}

extern parameters::container gPar;
#endif /*SHADERS_HPP_*/
