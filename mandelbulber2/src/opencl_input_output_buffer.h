/*
 * opencl_input_output_buffer.h
 *
 *  Created on: 28 gru 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_INPUT_OUTPUT_BUFFER_H_
#define MANDELBULBER2_SRC_OPENCL_INPUT_OUTPUT_BUFFER_H_

#ifdef USE_OPENCL
#include "include_header_wrapper.hpp"
#include <QtCore>

struct sClInputOutputBuffer
{
	sClInputOutputBuffer(qint64 itemSize, qint64 length, QString name)
			: itemSize(itemSize), length(length), name(std::move(name))
	{
	}

	static void Deleter(char *charArray) { delete[] charArray; }

	qint64 size() const { return itemSize * length; }
	qint64 itemSize;
	qint64 length;
	QString name;
	QSharedPointer<char> ptr;
	QSharedPointer<cl::Buffer> clPtr;
};
#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_INPUT_OUTPUT_BUFFER_H_ */
