/*
 * primitive_item.h
 *
 *  Created on: 22 paź 2022
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_PRIMITIVE_ITEM_H_
#define MANDELBULBER2_SRC_PRIMITIVE_ITEM_H_

#include <algorithm>
#include <QString>
#include "object_types.hpp"

struct sPrimitiveItem
{
	sPrimitiveItem(
		fractal::enumObjectType _type, int _id, const QString &_name, const QString &_typeName)
			: type(_type), id(_id), fullName(std::move(_name)), typeName(std::move(_typeName))
	{
	}

	sPrimitiveItem() : type(fractal::objNone), id(0), fullName(QString()), typeName(QString()) {}

	fractal::enumObjectType type;
	int id;
	QString fullName;
	QString typeName;

	QString Name(const QString &subParameterName) const { return fullName + "_" + subParameterName; }
};

#endif /* MANDELBULBER2_SRC_PRIMITIVE_ITEM_H_ */
