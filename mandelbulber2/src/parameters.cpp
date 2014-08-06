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
#include <QtCore>
#include <QtAlgorithms>
#include <algorithm>

//#define _PARAM_DEBUG

cParameterContainer *gPar;
cParameterContainer *gParFractal;

cParameterContainer::cParameterContainer()
{
	myMap.clear();
}

cParameterContainer::~cParameterContainer()
{
	myMap.clear();
}

//defining of params without limits
template<class T>
void cParameterContainer::addParam(QString name, T defaultVal, enumMorphType morphType, enumParameterType parType)
{
	sRecord newRecord;
	sMultiVal multi;
	newRecord.type = Assigner(multi, null, defaultVal);
	newRecord.actualVal = multi;
	newRecord.defaultVal = multi;
	newRecord.morphType = morphType;
	newRecord.limitsDefined = false;
	newRecord.parType = parType;

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<QString>(QString name, QString defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<bool>(QString name, bool defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<cColorPalette>(QString name, cColorPalette defaultVal, enumMorphType morphType, enumParameterType parType);

//defining of params with limits
template<class T>
void cParameterContainer::addParam(QString name, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType)
{
	sRecord newRecord;
	sMultiVal multi;
	newRecord.type = Assigner(multi, null, defaultVal);
	newRecord.actualVal = multi;
	newRecord.defaultVal = multi;

	Assigner(newRecord.minVal, newRecord.type, minVal);
	Assigner(newRecord.maxVal, newRecord.type, maxVal);
	newRecord.morphType = morphType;
	newRecord.limitsDefined = true;
	newRecord.parType = parType;

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal, double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int defaultVal, int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal, sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

//defining of params without limits and with index
template<class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, enumMorphType morphType, enumParameterType parType)
{
	if (index >= 0)
	{
		sRecord newRecord;
		sMultiVal multi;
		newRecord.type = Assigner(multi, null, defaultVal);
		newRecord.actualVal = multi;
		newRecord.defaultVal = multi;
		newRecord.morphType = morphType;
		newRecord.limitsDefined = false;
		newRecord.parType = parType;

		QString indexName = nameWithIndex(&name, index);
		if(myMap.find(indexName) != myMap.end())
		{
			qWarning() << "addParam(): element '" << indexName << "' already existed" << endl;
		}
		else
		{
			myMap.insert(indexName, newRecord);
		}
	}
	else
	{
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<QString>(QString name, int index, QString defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<bool>(QString name, int index, bool defaultVal, enumMorphType morphType, enumParameterType parType);

//defining of params with limits and index
template<class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType)
{
	if (index >= 0)
	{
		sRecord newRecord;
		sMultiVal multi;
		newRecord.type = Assigner(multi, null, defaultVal);
		newRecord.actualVal = multi;
		newRecord.defaultVal = multi;

		Assigner(newRecord.minVal, newRecord.type, minVal);
		Assigner(newRecord.maxVal, newRecord.type, maxVal);
		newRecord.morphType = morphType;
		newRecord.limitsDefined = true;
		newRecord.parType = parType;

		QString indexName = nameWithIndex(&name, index);
		if(myMap.find(indexName) != myMap.end())
		{
			qWarning() << "addParam(): element '" << indexName << "' already existed" << endl;
		}
		else
		{
			myMap.insert(indexName, newRecord);
		}
	}
	else
	{
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal, double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal, int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal, sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, double val)
{
	(void)defaultType;
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%.16lg", val);
	multi.sVal = QString(sbuff);
	return typeDouble;
}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, int val)
{
	(void)defaultType;
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	multi.sVal = QString(sbuff);
	return typeInt;
}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, QString val)
{
	clearMultiVal(multi);
	switch (defaultType)
	{
		case null:
		case typeVector3:
		case typeString:
			sscanf(val.toUtf8(), "%lf %lf %lf %lf", &multi.dVal[0], &multi.dVal[1], &multi.dVal[2], &multi.dVal[3]);
			sscanf(val.toUtf8(), "%d %d %d %d", &multi.iVal[0], &multi.iVal[1], &multi.iVal[2], &multi.iVal[3]);
			break;

		case typeInt:
		case typeDouble:
		case typeBool:
			sscanf(val.toUtf8(), "%lf", &multi.dVal[0]);
			sscanf(val.toUtf8(), "%d", &multi.iVal[0]);
			break;

		case typeRgb:
			sscanf(val.toUtf8(), "%x %x %x %x", &multi.iVal[0], &multi.iVal[1], &multi.iVal[2], &multi.iVal[3]);
			multi.dVal[0] = multi.iVal[0];
			multi.dVal[1] = multi.iVal[1];
			multi.dVal[2] = multi.iVal[2];
			multi.dVal[3] = multi.iVal[3];
			break;

		case typeColorPalette:
			//*********************** to do **********************
			break;
	}
	multi.sVal = val;
	return typeString;

}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, CVector3 val)
{
	(void)defaultType;
	clearMultiVal(multi);
	multi.dVal[0] = val.x;
	multi.dVal[1] = val.y;
	multi.dVal[2] = val.z;
	multi.iVal[0] = val.x;
	multi.iVal[1] = val.y;
	multi.iVal[2] = val.z;
	char sbuff[100];
	sprintf(sbuff, "%.16lg %.16lg %.16lg", val.x, val.y, val.z);
	multi.sVal = QString(sbuff);
	return typeVector3;
}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, sRGB val)
{
	(void)defaultType;
	clearMultiVal(multi);
	multi.dVal[0] = val.R;
	multi.dVal[1] = val.G;
	multi.dVal[2] = val.B;
	multi.iVal[0] = val.R;
	multi.iVal[1] = val.G;
	multi.iVal[2] = val.B;
	char sbuff[100];
	sprintf(sbuff, "%x %x %x", val.R, val.G, val.B);
	multi.sVal = QString(sbuff);
	return typeRgb;
}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, bool val)
{
	(void)defaultType;
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	multi.sVal = QString(sbuff);
	return typeBool;
}

cParameterContainer::enumVarType cParameterContainer::Assigner(sMultiVal &multi, enumVarType defaultType, cColorPalette val)
{
	(void)defaultType;
	clearMultiVal(multi);
  multi.sVal = MakePaletteString(val);
	return typeColorPalette;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, double &val) const
{
	val = multi.dVal[0];
	return typeDouble;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, int &val) const
{
	val = multi.iVal[0];
	return typeInt;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, CVector3 &val) const
{
	val = CVector3(multi.dVal[0], multi.dVal[1], multi.dVal[2]);
	return typeVector3;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, QString &val) const
{
	val = multi.sVal;
	return typeString;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, sRGB &val) const
{
	val = sRGB(multi.iVal[0], multi.iVal[1], multi.iVal[2]);
	return typeRgb;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, bool &val) const
{
	val = multi.iVal[0];
	return typeBool;
}

cParameterContainer::enumVarType cParameterContainer::Getter(sMultiVal multi, cColorPalette &val) const
{
	val = GetPaletteFromString(multi.sVal);
	return typeColorPalette;
}

//set parameter value by name
template<class T>
void cParameterContainer::Set(QString name, T val)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sMultiVal multi;
		enumVarType type = Assigner(multi, it->type, val);
		it->actualVal = multi;

#ifdef _PARAM_DEBUG
		if (it->type != type)
		{
			qWarning() << "Set(): element '" << name << "' got value of not default type" << endl;
			DebugPrintf(name);
		}
#else
		(void)type;
#endif
	}
	else
	{
		qWarning() << "Set(): element '" << name << "' doesn't exists" << endl;
	}
}
template void cParameterContainer::Set<double>(QString name, double val);
template void cParameterContainer::Set<int>(QString name, int val);
template void cParameterContainer::Set<QString>(QString name, QString val);
template void cParameterContainer::Set<CVector3>(QString name, CVector3 val);
template void cParameterContainer::Set<sRGB>(QString name, sRGB val);
template void cParameterContainer::Set<bool>(QString name, bool val);
template void cParameterContainer::Set<cColorPalette>(QString name, cColorPalette val);

//set parameter value by name and index
template<class T>
void cParameterContainer::Set(QString name, int index, T val)
{
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sMultiVal multi;
			enumVarType type = Assigner(multi, it->type, val);
			it->actualVal = multi;

#ifdef _PARAM_DEBUG
			if (it->type != type)
			{
				qWarning() << "Set(): element '" << name << "' got value of not default type" << endl;
				DebugPrintf(name);
			}
#else
		(void)type;
#endif
		}
		else
		{
			qWarning() << "Set(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "Set(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::Set<double>(QString name, int index, double val);
template void cParameterContainer::Set<int>(QString name, int index, int val);
template void cParameterContainer::Set<QString>(QString name, int index, QString val);
template void cParameterContainer::Set<CVector3>(QString name, int index, CVector3 val);
template void cParameterContainer::Set<sRGB>(QString name, int index, sRGB val);
template void cParameterContainer::Set<bool>(QString name, int index, bool val);

//get parameter value by name
template<class T>
T cParameterContainer::Get(QString name) const
{
	QMap<QString, sRecord>::const_iterator it;
	it = myMap.find(name);
	T val = T();
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		sMultiVal multi = rec.actualVal;
		enumVarType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
		if (it->type != type)
		{
			qWarning() << "Get(): element '" << name << "' gave value of not default type" << endl;
			DebugPrintf(name);
		}
#else
		(void)type;
#endif
	}
	else
	{
		qWarning() << "Get(): element '" << name << "' doesn't exists" << endl;
	}
	return val;
}
template double cParameterContainer::Get<double>(QString name) const;
template int cParameterContainer::Get<int>(QString name) const;
template QString cParameterContainer::Get<QString>(QString name) const;
template CVector3 cParameterContainer::Get<CVector3>(QString name) const;
template sRGB cParameterContainer::Get<sRGB>(QString name) const;
template bool cParameterContainer::Get<bool>(QString name) const;
template cColorPalette cParameterContainer::Get<cColorPalette>(QString name) const;

//get parameter value by name and index
template<class T>
T cParameterContainer::Get(QString name, int index) const
{
	T val = T();
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::const_iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sRecord rec = it.value();
			sMultiVal multi = rec.actualVal;
			enumVarType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
			if (it->type != type)
			{
				qWarning() << "Get(): element '" << indexName << "' gave value of not default type" << endl;
				DebugPrintf(indexName);
			}
#else
		(void)type;
#endif
		}
		else
		{
			qWarning() << "Get(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "Get(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
	return val;
}
template double cParameterContainer::Get<double>(QString name, int index) const;
template int cParameterContainer::Get<int>(QString name, int index) const;
template QString cParameterContainer::Get<QString>(QString name, int index) const;
template CVector3 cParameterContainer::Get<CVector3>(QString name, int index) const;
template sRGB cParameterContainer::Get<sRGB>(QString name, int index) const;
template bool cParameterContainer::Get<bool>(QString name, int index) const;

void cParameterContainer::DebugPrintf(QString name)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		printf("Actual value of variable '%s'\n", name.toUtf8().constData());
		printf("int = %d; %d; %d; %d\n", rec.actualVal.iVal[0], rec.actualVal.iVal[1], rec.actualVal.iVal[2], rec.actualVal.iVal[3]);
		printf("double = %.16lg; %.16lg; %.16lg; %.16lg\n", rec.actualVal.dVal[0], rec.actualVal.dVal[1], rec.actualVal.dVal[2], rec.actualVal.dVal[3]);
		printf("string = %s\n", rec.actualVal.sVal.toUtf8().constData());
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
			case null:
				printf("undefined type\n");
		}
	}
	else
	{
		qWarning() << "DebugPrintf(): element '" << name << "' doesn't exists" << endl;
	}
}

QString cParameterContainer::nameWithIndex(QString *str, int index) const
{
	QString name = *str + "_" + QString::number(index);
	return name;
}

void cParameterContainer::clearMultiVal(sMultiVal &multiVal)
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

QString cParameterContainer::MakePaletteString(cColorPalette &palette)
{
	int length;
	int pointer = 0;
	char *paletteString = new char[(palette.GetSize()+2) * 7];
	for (int i = 0; i < palette.GetSize(); i++)
	{
		sRGB colorRGB = palette.GetColor(i);
		int colour = colorRGB.R * 65536 + colorRGB.G * 256 + colorRGB.B;
		colour = colour & 0x00FFFFFF;
		length = sprintf(&paletteString[pointer], "%x ", colour);
		pointer += length;
	}
	QString out(paletteString);
	delete[] paletteString;
	return out;
}

cColorPalette cParameterContainer::GetPaletteFromString(const QString &paletteString) const
{
	cColorPalette colorPalette;

	for (int i = 0; i < paletteString.length(); i++)
	{
		unsigned int colour = 0;
		sscanf(&paletteString.toLocal8Bit().constData()[i], "%x", &colour);
		sRGB rgbColour;
		rgbColour.R = colour / 65536;
		rgbColour.G = (colour / 256) % 256;
		rgbColour.B = colour % 256;
		colorPalette.AppendColor(rgbColour);
		while (i < paletteString.length() && paletteString[i] != ' ')
		{
			i++;
		}
	}
	return colorPalette;
}

void cParameterContainer::Copy(QString name, cParameterContainer *sourceContainer)
{
	QMap<QString, sRecord>::iterator itSource, itDest;
	itDest = myMap.find(name);
	if (itDest != myMap.end())
	{
		itSource = sourceContainer->myMap.find(name);
		if (itDest != myMap.end())
		{
			itDest.value() = itSource.value();
		}
		else
		{
			qWarning() << "CopyPar(): source element '" << name << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "CopyPar(): destination element '" << name << "' doesn't exists" << endl;
	}
}

QList<QString> cParameterContainer::GetListOfParameters(void) const
{
	QList<QString> list = myMap.keys();
	std::sort(list.begin(), list.end(), compareStrings);
	return list;
}

cParameterContainer::enumVarType cParameterContainer::GetVarType(QString name) const
{
	enumVarType type = null;

	QMap<QString, sRecord>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		type = rec.type;
	}
	else
	{
		qWarning() << "GetVarType(): element '" << name << "' doesn't exists" << endl;
	}
	return type;
}

enumParameterType cParameterContainer::GetParameterType(QString name) const
{
	enumParameterType type = paramStandard;

	QMap<QString, sRecord>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		type = rec.parType;
	}
	else
	{
		qWarning() << "GetParameterType(): element '" << name << "' doesn't exists" << endl;
	}
	return type;
}

bool cParameterContainer::isDefaultValue(QString name) const
{
	enumVarType type = null;
	bool isDefault = true;

	QMap<QString, sRecord>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		type = rec.type;
		switch (type)
		{
			case typeBool:
			case typeInt:
			case typeRgb:
			{
				for (int i = 0; i < 4; i++)
				{
					if (rec.actualVal.iVal[i] != rec.defaultVal.iVal[i])
					{
						isDefault = false;
						break;
					}
				}
				break;
			}
			case typeDouble:
			case typeVector3:
			{
				for (int i = 0; i < 4; i++)
				{
					if (rec.actualVal.dVal[i] != rec.defaultVal.dVal[i])
					{
						isDefault = false;
						break;
					}
				}
				break;
			}
			case typeColorPalette:
			case typeString:
			{
				if (rec.actualVal.sVal != rec.defaultVal.sVal)
				{
					isDefault = false;
				}
				break;
			}
			case null:
			{
				qCritical() << "cParameterContainer::isDefaultValue(QString name): undefined type of variable";
				break;
			}
		}
	}
	else
	{
		qWarning() << "GetVarType(): element '" << name << "' doesn't exists" << endl;
	}
	return isDefault;
}

