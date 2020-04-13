/*
 * write_log.hpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#include <QString>

#ifndef MANDELBULBER2_SRC_WRITE_LOG_HPP_
#define MANDELBULBER2_SRC_WRITE_LOG_HPP_

void WriteLog(const QString &text, int verbosityLevel);
void WriteLogCout(const QString &text, int verbosityLevel);
void WriteLogDouble(const QString &text, double value, int verbosityLevel);
void WriteLogInt(const QString &text, int value, int verbosityLevel);
void WriteLogSizeT(const QString &text, size_t value, int verbosityLevel);
void WriteLogString(const QString &text, const QString &value, int verbosityLevel);

#endif /* MANDELBULBER2_SRC_WRITE_LOG_HPP_ */
