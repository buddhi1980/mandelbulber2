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
void container::addParam(QString name, T defaultVal, bool morphable)
{
	sRecord newRecord;
	sMultiVal multi;
	newRecord.type = Assigner(multi, defaultVal);
	newRecord.actualVal = multi;
	newRecord.defaultVal = multi;
	newRecord.morphable = morphable;
	newRecord.limitsDefined = false;
	newRecord.toSave = true;
	newRecord.appParam = false;

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void container::addParam<double>(QString name, double defaultVal, bool morphable);
template void container::addParam<int>(QString name, int defaultVal, bool morphable);
template void container::addParam<QString>(QString name, QString defaultVal, bool morphable);
template void container::addParam<CVector3>(QString name, CVector3 defaultVal, bool morphable);
template void container::addParam<sRGB>(QString name, sRGB defaultVal, bool morphable);
template void container::addParam<bool>(QString name, bool defaultVal, bool morphable);
template void container::addParam<sRGB*>(QString name, sRGB *defaultVal, bool morphable);

//defining of params with limits
template<class T>
void container::addParam(QString name, T defaultVal, T minVal, T maxVal, bool morphable)
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
	newRecord.appParam = false;

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void container::addParam<double>(QString name, double defaultVal, double minVal, double maxVal, bool morphable);
template void container::addParam<int>(QString name, int defaultVal, int minVal, int maxVal, bool morphable);
template void container::addParam<CVector3>(QString name, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, bool morphable);
template void container::addParam<sRGB>(QString name, sRGB defaultVal, sRGB minVal, sRGB maxVal, bool morphable);

//defining of params without limits and with index
template<class T>
void container::addParam(QString name, int index, T defaultVal, bool morphable)
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
		newRecord.appParam = false;

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
template void container::addParam<double>(QString name, int index, double defaultVal, bool morphable);
template void container::addParam<int>(QString name, int index, int defaultVal, bool morphable);
template void container::addParam<QString>(QString name, int index, QString defaultVal, bool morphable);
template void container::addParam<CVector3>(QString name, int index, CVector3 defaultVal, bool morphable);
template void container::addParam<sRGB>(QString name, int index, sRGB defaultVal, bool morphable);
template void container::addParam<bool>(QString name, int index, bool defaultVal, bool morphable);

//defining of params with limits and index
template<class T>
void container::addParam(QString name, int index, T defaultVal, T minVal, T maxVal, bool morphable)
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
		newRecord.appParam = false;

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
template void container::addParam<double>(QString name, int index, double defaultVal, double minVal, double maxVal, bool morphable);
template void container::addParam<int>(QString name, int index, int defaultVal, int minVal, int maxVal, bool morphable);
template void container::addParam<CVector3>(QString name, int index, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, bool morphable);
template void container::addParam<sRGB>(QString name, int index, sRGB defaultVal, sRGB minVal, sRGB maxVal, bool morphable);

varType container::Assigner(sMultiVal &multi, double val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%.16lg", val);
	multi.sVal = QString(sbuff);
	return typeDouble;
}

varType container::Assigner(sMultiVal &multi, int val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	multi.sVal = QString(sbuff);
	return typeInt;
}

varType container::Assigner(sMultiVal &multi, QString val)
{
	clearMultiVal(multi);
	sscanf(val.toUtf8(), "%lf %lf %lf %lf", &multi.dVal[0], &multi.dVal[1], &multi.dVal[2], &multi.dVal[3]);
	sscanf(val.toUtf8(), "%d %d %d %d", &multi.iVal[0], &multi.iVal[1], &multi.iVal[2], &multi.iVal[3]);
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
	sprintf(sbuff, "%.16lg %.16lg %.16lg", val.x, val.y, val.z);
	multi.sVal = QString(sbuff);
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
	sprintf(sbuff, "%x %x %x", val.R, val.G, val.B);
	multi.sVal = QString(sbuff);
	return typeRgb;
}

varType container::Assigner(sMultiVal &multi, bool val)
{
	clearMultiVal(multi);
	multi.dVal[0] = val;
	multi.iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	multi.sVal = QString(sbuff);
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

varType container::Getter(sMultiVal multi, QString &val)
{
	val = multi.sVal;
	return typeString;
}

varType container::Getter(sMultiVal multi, sRGB &val)
{
	val = sRGB(multi.iVal[0], multi.iVal[1], multi.iVal[2]);
	return typeRgb;
}

varType container::Getter(sMultiVal multi, bool &val)
{
	val = multi.iVal[0];
	return typeBool;
}

//set parameter value by name
template<class T>
void container::Set(QString name, T val)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		sMultiVal multi;
		varType type = Assigner(multi, val);
		if (it->type == type)
		{
			it->actualVal = multi;
		}
#ifdef _PARAM_DEBUG
		else
		{
			qWarning() << "Set(): element '" << name << "' got value of not default type" << endl;
			DebugPrintf(name);
		}
#endif
	}
	else
	{
		qWarning() << "Set(): element '" << name << "' doesn't exists" << endl;
	}
}
template void container::Set<double>(QString name, double val);
template void container::Set<int>(QString name, int val);
template void container::Set<QString>(QString name, QString val);
template void container::Set<CVector3>(QString name, CVector3 val);
template void container::Set<sRGB>(QString name, sRGB val);
template void container::Set<bool>(QString name, bool val);

//set parameter value by name and index
template<class T>
void container::Set(QString name, int index, T val)
{
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sMultiVal multi;
			varType type = Assigner(multi, val);
			if (it->type == type)
			{
				it->actualVal = multi;
			}
#ifdef _PARAM_DEBUG
			else
			{
				qWarning() << "Set(): element '" << name << "' got value of not default type" << endl;
				DebugPrintf(name);
			}
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
template void container::Set<double>(QString name, int index,double val);
template void container::Set<int>(QString name, int index,int val);
template void container::Set<QString>(QString name, int index,QString val);
template void container::Set<CVector3>(QString name, int index,CVector3 val);
template void container::Set<sRGB>(QString name, int index,sRGB val);
template void container::Set<bool>(QString name, int index,bool val);

//get parameter value by name
template<class T>
T container::Get(QString name)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	T val;
	if (it != myMap.end())
	{
		sRecord rec = it.value();
		sMultiVal multi = rec.actualVal;
		varType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
		if (it->type != type)
		{
			qWarning() << "Get(): element '" << name << "' gave value of not default type" << endl;
			DebugPrintf(name);
		}
#endif
	}
	else
	{
		qWarning() << "Get(): element '" << name << "' doesn't exists" << endl;
	}
	return val;
}
template double container::Get<double>(QString name);
template int container::Get<int>(QString name);
template QString container::Get<QString>(QString name);
template CVector3 container::Get<CVector3>(QString name);
template sRGB container::Get<sRGB>(QString name);
template bool container::Get<bool>(QString name);

//get parameter value by name and index
template<class T>
T container::Get(QString name, int index)
{
	T val;
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			sRecord rec = it.value();
			sMultiVal multi = rec.actualVal;
			varType type = Getter(multi, val);
#ifdef _PARAM_DEBUG
			if (it->type != type)
			{
				qWarning() << "Get(): element '" << indexName << "' gave value of not default type" << endl;
				DebugPrintf(indexName);
			}
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
template double container::Get<double>(QString name, int index);
template int container::Get<int>(QString name, int index);
template QString container::Get<QString>(QString name, int index);
template CVector3 container::Get<CVector3>(QString name, int index);
template sRGB container::Get<sRGB>(QString name, int index);
template bool container::Get<bool>(QString name, int index);

void container::DebugPrintf(QString name)
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
		}
	}
	else
	{
		qWarning() << "DebugPrintf(): element '" << name << "' doesn't exists" << endl;
	}
}

QString container::nameWithIndex(QString *str, int index)
{
	QString name = *str + "_" + QString::number(index);
	return name;
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

QString container::MakePaletteString(sRGB palette[256])
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
	QString out(paletteString);
	return out;
}

void container::SetToSave(QString name, bool toSave)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it->toSave = toSave;
	}
	else
	{
		qWarning() << "SetToSave(): element '" << name << "' doesn't exists" << endl;
	}
}

void container::SetToSave(QString name, int index, bool toSave)
{
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			it->toSave = toSave;
		}
		else
		{
			qWarning() << "SetToSave(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "SetToSave(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}

void container::SetAsAppParam(QString name, bool asAppParam)
{
	QMap<QString, sRecord>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it->appParam = asAppParam;
	}
	else
	{
		qWarning() << "SetAsAppParam(): element '" << name << "' doesn't exists" << endl;
	}
}

void container::SetAsAppParam(QString name, int index, bool asAppParam)
{
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, sRecord>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			it->appParam = asAppParam;
		}
		else
		{
			qWarning() << "SetAsAppParam(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "SetAsAppParam(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}

void container::Copy(QString name, container *sourceContainer)
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
