<?php
/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.       __           _       _       
 * Copyright (C) 2017 Mandelbulber Team   _>]|=||i=i<,    / _\ ___ _ __(_)_ __ | |_ ___ 
 *                                        \><||i|=>>%)    \ \ / __| '__| | '_ \| __/ __|
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    _\ \ (__| |  | | |_) | |_\__ \
 * The project is licensed under GPLv3,   -<>>=|><|||`    \__/\___|_|  |_| .__/ \__|___/
 * see also COPYING file in this folder.    ~+{i%+++                     |_|  
 *
 * This file contains example code to work with the MbSettings class.
 */

require_once('mbSettings.php');

/// ########################################################################
/// ######## FLIGHT ANIMATION: "CHASING THE LIGHTS" - EFFECT       #########
// Create settings from fract file
$settings = new MbSettings('../examples/flight_anim_menger sponge_3.fract');
// Get the sequence count to iterate over the flight frames
$sequenceCount = $settings->GetSequenceCount(MbSettings::CATEGORY_FRAMES);
// Iterate over every frame
for($i = 0; $i < $sequenceCount; $i++){
	// Get the location of the camera
	$vect = $settings->Get(MbSettings::CATEGORY_FRAMES, 
		'main_camera', $i, MbSettings::PARAMETER_XYZ);
	// For each of the four lights ...
	for($l = 0; $l < 4; $l++){
		// Copy the camera position
		$light = $vect; 
		// Add some randomness
		$light->add(new Vector(MbSettings::Rand01() * 0.1,
			MbSettings::Rand01() * 0.1, 
			MbSettings::Rand01() * 0.1));
		// Save the position of the camera to the flight list
		// note: The index is given with ($i - 1), this applies the modified
		// position of the current camera position to the previous frame
		// which causes the lights to be "ahead" of the camera.
		$settings->Set($light, MbSettings::CATEGORY_FRAMES, 
			'main_aux_light_position_' . ($l + 1), 
			max(0, $i - 1), MbSettings::PARAMETER_XYZ);
	}
}
// store the settings
$settings->SaveSettings('../examples/flight_anim_menger sponge_3.fract');
/// ######## FLIGHT ANIMATION: "CHASING THE LIGHTS" - EFFECT       #########
/// ########################################################################

?>
