/*
 * error_message.hpp
 *
 *  Created on: Jul 10, 2014
 *      Author: krzysztof
 */

#ifndef ERROR_MESSAGE_HPP_
#define ERROR_MESSAGE_HPP_

#include <QtCore>
#include "system.hpp"

class cErrorMessage
{
public:
	enum enumMessageType
	{
		warningMessage,	errorMessage,	infoMessage
	};

	static void showMessage(QString text, enumMessageType messageType, QWidget *parent = NULL);
};



#endif /* ERROR_MESSAGE_HPP_ */


