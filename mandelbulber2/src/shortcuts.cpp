/*
 * shortcuts.cpp
 *
 *  Created on: 6 sty 2022
 *      Author: krzysztof
 */

#include "shortcuts.h"
#include "parameters.hpp"
#include <QKeySequence>

enumShortcuts cShortcuts::getShortcut(cParameterContainer *settings, int key, int keymodifier)
{
	QKeySequence k = (key | keymodifier);

	if (k.matches(kSeq(settings, "key_move_forward"))) return enumShortcuts::moveForward;
	if (k.matches(kSeq(settings, "key_move_backward"))) return enumShortcuts::moveBackward;
	if (k.matches(kSeq(settings, "key_move_left"))) return enumShortcuts::moveLeft;
	if (k.matches(kSeq(settings, "key_move_right"))) return enumShortcuts::moveRight;
	if (k.matches(kSeq(settings, "key_move_up"))) return enumShortcuts::moveUp;
	if (k.matches(kSeq(settings, "key_move_down"))) return enumShortcuts::moveDown;
	if (k.matches(kSeq(settings, "key_rotate_up"))) return enumShortcuts::rotateUp;
	if (k.matches(kSeq(settings, "key_rotate_down"))) return enumShortcuts::rotateDown;
	if (k.matches(kSeq(settings, "key_rotate_left"))) return enumShortcuts::rotateLeft;
	if (k.matches(kSeq(settings, "key_rotate_right"))) return enumShortcuts::rotateRight;
	if (k.matches(kSeq(settings, "key_roll_left"))) return enumShortcuts::rollLeft;
	if (k.matches(kSeq(settings, "key_roll_right"))) return enumShortcuts::rollRight;
	if (k.matches(kSeq(settings, "key_render"))) return enumShortcuts::render;
	if (k.matches(kSeq(settings, "key_stop"))) return enumShortcuts::stop;
	if (k.matches(kSeq(settings, "key_add_keyframe"))) return enumShortcuts::addKeyframe;
	if (k.matches(kSeq(settings, "key_modify_keyframe"))) return enumShortcuts::modifyKeyframe;

	return enumShortcuts::none;
}

QKeySequence cShortcuts::kSeq(const cParameterContainer *params, const QString keyName)
{
	return QKeySequence(params->Get<QString>(keyName), QKeySequence::PortableText);
}
