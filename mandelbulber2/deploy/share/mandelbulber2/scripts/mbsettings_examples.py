#!/usr/bin/env python3
r"""
Mandelbulber v2, a 3D fractal generator  _%}}i*<.       __           _       _
Copyright (C) 2017 Mandelbulber Team   _>]|=||i=i<,    / _\ ___ _ __(_)_ __ | |_ ___
                                       \><||i|=>>%)    \ \ / __| '__| | '_ \| __/ __|
This file is part of Mandelbulber.     )<=i=]=|=i<>    _\ \ (__| |  | | |_) | |_\__ \
The project is licensed under GPLv3,   -<>>=|><|||`    \__/\___|_|  |_| .__/ \__|___/
see also COPYING file in this folder.    ~+{i%+++                     |_|

This file contains example code to work with the MbSettings class.
"""

import os

from mbsettings import MbSettings, Vector

# the example settings file, found relative to this script
settingsFile = os.path.join(os.path.dirname(os.path.abspath(__file__)),
	'..', 'examples', 'flight_anim_menger sponge_3.fract')

# ########################################################################
# ######## FLIGHT ANIMATION: "CHASING THE LIGHTS" - EFFECT       #########
# Create settings from fract file
settings = MbSettings(settingsFile)
# Get the sequence count to iterate over the flight frames
sequenceCount = settings.GetSequenceCount(MbSettings.CATEGORY_FRAMES)
# Iterate over every frame
for i in range(sequenceCount):
	# Get the location of the camera
	vect = settings.Get(MbSettings.CATEGORY_FRAMES,
		'main_camera', i, MbSettings.PARAMETER_XYZ)
	# For each of the four lights ...
	for l in range(4):
		# Copy the camera position
		# note: this is the same object, as in the PHP version, so the random
		# offsets of the lights add up
		light = vect
		# Add some randomness
		light.add(Vector(MbSettings.Rand01() * 0.1,
			MbSettings.Rand01() * 0.1,
			MbSettings.Rand01() * 0.1))
		# Save the position of the camera to the flight list
		# note: The index is given with (i - 1), this applies the modified
		# position of the current camera position to the previous frame
		# which causes the lights to be "ahead" of the camera.
		settings.Set(light, MbSettings.CATEGORY_FRAMES,
			'main_aux_light_position_' + str(l + 1),
			max(0, i - 1), MbSettings.PARAMETER_XYZ)
# store the settings
settings.SaveSettings(settingsFile)
# ######## FLIGHT ANIMATION: "CHASING THE LIGHTS" - EFFECT       #########
# ########################################################################
