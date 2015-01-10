/*
 * multi_val.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: krzysztof
 */

#include "multi_val.hpp"
#include <QtCore>

using namespace parameterContainer;

//TODO replace all sscanf and sprintf with QString functions (problems with dots/colons)

cMultiVal::cMultiVal()
{
	iVal[0] = 0;
	iVal[1] = 0;
	iVal[2] = 0;
	iVal[3] = 0;
	dVal[0] = 0.0;
	dVal[1] = 0.0;
	dVal[2] = 0.0;
	dVal[3] = 0.0;
	sVal.clear();
	typeDefined = false;
	type = typeNull;
}

cMultiVal::~cMultiVal()
{
	//nothing
}

enumVarType cMultiVal::Store(double val)
{
	dVal[0] = val;
	iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%.16lg", val);
	sVal = QString(sbuff);

	if(!typeDefined) type = typeDouble;
	return typeDouble;
}

enumVarType cMultiVal::Store(int val)
{
	dVal[0] = val;
	iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	sVal = QString(sbuff);
	if(!typeDefined) type = typeInt;
	return typeInt;
}

enumVarType cMultiVal::Store(QString val)
{
	switch (type)
	{
		case typeNull:
		case typeVector3:
		case typeString:
			sscanf(val.toUtf8(), "%lf %lf %lf %lf", &dVal[0], &dVal[1], &dVal[2], &dVal[3]);
			sscanf(val.toUtf8(), "%d %d %d %d", &iVal[0], &iVal[1], &iVal[2], &iVal[3]);
			break;

		case typeInt:
		case typeDouble:
		case typeBool:
			sscanf(val.toUtf8(), "%lf", &dVal[0]);
			sscanf(val.toUtf8(), "%d", &iVal[0]);
			break;

		case typeRgb:
			sscanf(val.toUtf8(), "%x %x %x %x", &iVal[0], &iVal[1], &iVal[2], &iVal[3]);
			dVal[0] = iVal[0];
			dVal[1] = iVal[1];
			dVal[2] = iVal[2];
			dVal[3] = iVal[3];
			break;

		case typeColorPalette:
			//*********************** to do **********************
			break;
	}
	sVal = val;

	if(!typeDefined) type = typeString;
	return typeString;
}

enumVarType cMultiVal::Store(CVector3 val)
{
	dVal[0] = val.x;
	dVal[1] = val.y;
	dVal[2] = val.z;
	iVal[0] = val.x;
	iVal[1] = val.y;
	iVal[2] = val.z;
	char sbuff[100];
	sprintf(sbuff, "%.16lg %.16lg %.16lg", val.x, val.y, val.z);
	sVal = QString(sbuff);

	if(!typeDefined) type = typeVector3;
	return type;
}

enumVarType cMultiVal::Store(sRGB val)
{
	dVal[0] = val.R;
	dVal[1] = val.G;
	dVal[2] = val.B;
	iVal[0] = val.R;
	iVal[1] = val.G;
	iVal[2] = val.B;
	char sbuff[100];
	sprintf(sbuff, "%x %x %x", val.R, val.G, val.B);
	sVal = QString(sbuff);

	if(!typeDefined) type = typeRgb;
	return typeRgb;
}

enumVarType cMultiVal::Store(bool val)
{
	dVal[0] = val;
	iVal[0] = val;
	char sbuff[100];
	sprintf(sbuff, "%d", val);
	sVal = QString(sbuff);

	if(!typeDefined) type = typeBool;
	return typeBool;
}

enumVarType cMultiVal::Store(cColorPalette val)
{
  sVal = MakePaletteString(val);

	if(!typeDefined) type = typeColorPalette;
	return typeColorPalette;
}


enumVarType cMultiVal::Get(double &val) const
{
	val = dVal[0];
	return typeDouble;
}

enumVarType cMultiVal::Get(int &val) const
{
	val = iVal[0];
	return typeInt;
}

enumVarType cMultiVal::Get(CVector3 &val) const
{
	val = CVector3(dVal[0], dVal[1], dVal[2]);
	return typeVector3;
}

enumVarType cMultiVal::Get(QString &val) const
{
	val = sVal;
	return typeString;
}

enumVarType cMultiVal::Get(sRGB &val) const
{
	val = sRGB(iVal[0], iVal[1], iVal[2]);
	return typeRgb;
}

enumVarType cMultiVal::Get(bool &val) const
{
	val = iVal[0];
	return typeBool;
}

enumVarType cMultiVal::Get(cColorPalette &val) const
{
	val = GetPaletteFromString(sVal);
	return typeColorPalette;
}

QString cMultiVal::MakePaletteString(cColorPalette &palette)
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

cColorPalette cMultiVal::GetPaletteFromString(const QString &paletteString) const
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

bool cMultiVal::isEqual(const cMultiVal &m) const
{
	bool isEqual = true;
	switch (type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			for (int i = 0; i < 4; i++)
			{
				if (iVal[i] != m.iVal[i])
				{
					isEqual = false;
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
				if (dVal[i] != m.dVal[i])
				{
					isEqual = false;
					break;
				}
			}
			break;
		}
		case typeColorPalette:
		case typeString:
		{
			if (sVal != m.sVal)
			{
				isEqual = false;
			}
			break;
		}
		case typeNull:
		{
			qCritical() << "cMultiVal::isEqual(const cMultiVal &m): undefined type of variable";
			break;
		}
	}
	return isEqual;
}

bool cMultiVal::operator==(const cMultiVal &m) const
{
	return isEqual(m);
}
