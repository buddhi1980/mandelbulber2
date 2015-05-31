/*
 * multi_val.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: krzysztof
 */

#include "multi_val.hpp"
#include <QtCore>

using namespace parameterContainer;

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
	sVal = QString::number(val, 'g', 16);

	if(!typeDefined) type = typeDouble;
	return typeDouble;
}

enumVarType cMultiVal::Store(int val)
{
	dVal[0] = val;
	iVal[0] = val;
	sVal = QString::number(val);

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
		{
			QStringList split = val.split(' ');
			int size = split.size();
			if(size >= 4) size = 4;
			for(int i = 0; i < size; i++)
			{
				dVal[i] = split[i].toDouble();
				iVal[i] = split[i].toInt();
			}
			break;
		}
		case typeInt:
		case typeDouble:
		case typeBool:
			dVal[0] = val.toDouble();
			iVal[0] = val.toInt();
			break;

		case typeRgb:
		{
			QStringList split = val.split(' ');
			int size = split.size();
			if(size >= 4) size = 4;
			for(int i = 0; i < size; i++)
			{
				dVal[i] = iVal[i] = split[i].toInt(NULL, 16);
			}
			break;
		}

		case typeColorPalette:
			sVal = val;
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
	sVal =  QString::number(val.x, 'g', 16) + " " + QString::number(val.y, 'g', 16) + " " + QString::number(val.z, 'g', 16);

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
	sVal = QString::number(val.R, 16) + " " + QString::number(val.G, 16) + " " + QString::number(val.B, 16);

	if(!typeDefined) type = typeRgb;
	return typeRgb;
}

enumVarType cMultiVal::Store(bool val)
{
	dVal[0] = val;
	iVal[0] = val;
	sVal = QString::number(val);

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
	QString paletteString;
	for (int i = 0; i < palette.GetSize(); i++)
	{
		sRGB colorRGB = palette.GetColor(i);
		int colour = colorRGB.R * 65536 + colorRGB.G * 256 + colorRGB.B;
		colour = colour & 0x00FFFFFF;
		paletteString += QString::number(colour, 16) + " ";
	}
	return paletteString;
}

cColorPalette cMultiVal::GetPaletteFromString(const QString &paletteString) const
{
	cColorPalette colorPalette;
	QStringList split = paletteString.split(" ");

	for (int i = 0; i < split.size(); i++)
	{
		if(split[i].size() > 0)
		{
			unsigned int colour = split[i].toInt(NULL, 16);
			sRGB rgbColour;
			rgbColour.R = colour / 65536;
			rgbColour.G = (colour / 256) % 256;
			rgbColour.B = colour % 256;
			colorPalette.AppendColor(rgbColour);
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
