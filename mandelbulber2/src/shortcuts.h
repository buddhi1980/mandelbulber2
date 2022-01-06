/*
 * shortcuts.h
 *
 *  Created on: 6 sty 2022
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_SHORTCUTS_H_
#define MANDELBULBER2_SRC_SHORTCUTS_H_

#include <QKeySequence>
#include <QString>

enum class enumShortcuts
{
	none,
	moveForward,
	moveBackward,
	moveUp,
	moveDown,
	moveLeft,
	moveRight,
	rotateLeft,
	rotateRight,
	rotateUp,
	rotateDown,
	rollLeft,
	rollRight,
	render,
	stop,
	terminate,
	addKeyframe,
	modifyKeyframe
};

class cParameterContainer;

class cShortcuts
{
public:
	cShortcuts() = default;
	static enumShortcuts getShortcut(cParameterContainer *settings, int key, int keymodifier);

private:
	static QKeySequence kSeq(const cParameterContainer *params, const QString keyName);
};

#endif /* MANDELBULBER2_SRC_SHORTCUTS_H_ */
