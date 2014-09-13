/*
 * progress_text.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: krzysztof
 */

#ifndef PROGRESS_TEXT_HPP_
#define PROGRESS_TEXT_HPP_

#include <QtCore>

class cProgressText
{
public:
	cProgressText();
	void ResetTimer();
	QString getText(double progress);

private:
	QElapsedTimer timer;
	QString TimeString(qint64 time);
	qint64 lastTimeForETA;
	double lastProgressForETA;
	double renderingSpeed;
	double lastSpeed;
	double renderingAcceleration;
};



#endif /* PROGRESS_TEXT_HPP_ */
