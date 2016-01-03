/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * list of fractal formulas
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "fractal_list.hpp"

QList<sFractalDescription> fractalList;

using namespace fractal;
void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	//list of fractal formulas (name visible in UI, internal name, internal ID)
	fractalList->append(sFractalDescription("None", 													"", 												none, 									deltaDEType, 		withoutDEFunction));
	fractalList->append(sFractalDescription("AboxMod1", 											"aboxMod1", 								aboxMod1, 							analyticDEType, linearDEFunction));
  fractalList->append(sFractalDescription("AboxModKali",								 		"aboxModKali", 							aboxModKali, 						analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Aexion", 												"aexion", 									aexion, 								deltaDEType, 		logarithmicDEFunction));
  fractalList->append(sFractalDescription("Aexion_Octopus", 								"aexionOctopus", 						aexionOctopus, 					deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Benesi Transforms DeltaDE", 			"benesi_transforms", 				benesiTransforms, 			deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Benesi", 												"benesi", 									benesi, 								analyticDEType,	logarithmicDEFunction));
  fractalList->append(sFractalDescription("Benesi_Pine_Tree", 							"benesi_pine_tree", 				benesiPineTree, 				analyticDEType, logarithmicDEFunction));
  fractalList->append(sFractalDescription("Benesi_T1_Pine_Tree", 						"benesi_T1_pine_tree", 			benesiT1PineTree, 			analyticDEType, logarithmicDEFunction));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", 						"boxfold_bulbpow2", 				boxFoldBulbPow2, 				deltaDEType, 		linearDEFunction));
	fractalList->append(sFractalDescription("Bristorbrot", 										"bristorbrot", 							bristorbrot, 						analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Buffalo", 												"buffalo", 									buffalo, 								analyticDEType,	logarithmicDEFunction));
  fractalList->append(sFractalDescription("Eiffie's MsltoeJulia",						"eiffie_msltoe", 						eiffieMsltoe,						analyticDEType,	logarithmicDEFunction));
  fractalList->append(sFractalDescription("FabsBoxMod", 										"fabsBoxMod", 							fabsBoxMod, 						analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Generalized Fold Box", 					"generalized_fold_box", 		generalizedFoldBox, 		analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Hypercomplex", 									"hypercomplex", 						hypercomplex, 					analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Ides 1", 												"ides", 										ides, 									deltaDEType,		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Ides 2", 												"ides2", 										ides2, 									deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Kaleidoscopic IFS", 							"ifs", 											kaleidoscopicIFS, 			analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Lkmitch", 												"lkmitch", 									lkmitch, 								deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Makin3D-2", 											"makin3d2", 								makin3d2, 							deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbox", 											"mandelbox", 								mandelbox, 							analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Mandelbox - extended", 					"mandelbox103", 						mandelbox103, 					analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D", 				"mandelbox_vary_scale_4d", 	mandelboxVaryScale4D, 	analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb", 										"mandelbulb", 							mandelbulb, 						analyticDEType, logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb - extended", 					"mandelbulb5", 							mandelbulb5, 						analyticDEType, logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb 2", 									"mandelbulb2", 							mandelbulb2, 						analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb 3", 									"mandelbulb3", 							mandelbulb3, 						analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb 4", 									"mandelbulb4", 							mandelbulb4, 						analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb Classic Power 2", 		"mandelbulb_power_2", 			fast_mandelbulb_power2, analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Mandelbulb6 beta", 							"mandelbulb6_beta", 				mandelbulb6Beta, 				analyticDEType, logarithmicDEFunction));
  fractalList->append(sFractalDescription("MandelbulbMulti", 								"mandelbulb_multi", 				mandelbulbMulti, 				analyticDEType, logarithmicDEFunction));
	fractalList->append(sFractalDescription("Menger Sponge - extended", 			"menger_sponge105", 				mengerSponge105, 				analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Menger sponge", 									"menger_sponge", 						menger_sponge, 					analyticDEType, linearDEFunction));
  fractalList->append(sFractalDescription("MengerMod", 											"mengerMod", 								mengerMod, 							analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("MsltoeSym2", 										"msltoesym2", 							msltoesym2, 						analyticDEType, logarithmicDEFunction));
  fractalList->append(sFractalDescription("MsltoeSym3", 										"msltoesym3", 							msltoesym3, 						analyticDEType, logarithmicDEFunction));
  fractalList->append(sFractalDescription("MsltoeSym4", 										"msltoesym4", 							msltoesym4, 						analyticDEType, logarithmicDEFunction));
	fractalList->append(sFractalDescription("Quaternion", 										"quaternion", 							quaternion, 						analyticDEType,	logarithmicDEFunction));
	fractalList->append(sFractalDescription("Quaternion - extended", 					"quaternion104", 						quaternion104, 					deltaDEType, 		logarithmicDEFunction));
  fractalList->append(sFractalDescription("Quaternion3D", 									"quaternion3D", 						quaternion3D, 					deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Quick-Dudley", 									"quickdudley", 							quickdudley, 						deltaDEType, 		logarithmicDEFunction));
  fractalList->append(sFractalDescription("Riemann Sphere Msltoe",					"riemann_sphere_msltoe",		riemannSphereMsltoe, 		deltaDEType, 		logarithmicDEFunction));
  fractalList->append(sFractalDescription("Riemann Sphere Msltoe V1",				"riemann_sphere_msltoeV1",	riemannSphereMsltoeV1, 	deltaDEType, 		logarithmicDEFunction));
	fractalList->append(sFractalDescription("Smooth Mandelbox", 							"smooth_mandelbox", 				smoothMandelbox, 				analyticDEType, linearDEFunction));
	fractalList->append(sFractalDescription("Xenodreambuie's formula", 				"xenodreambuie", 						xenodreambuie, 					analyticDEType,	logarithmicDEFunction));

  fractalList->append(sFractalDescription("Transform - Addition Constant",			"transf_addition_constant",			transfAdditionConstant,			analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Add Cpixel",	 						"transf_add_Cpixel", 						transfAddCpixel, 						analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Add Cpixel_Axis_Swap",	 	"transf_add_Cpixel_axis_swap", 	transfAddCpixelAxisSwap,		analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Add Cpixel_Pos_Neg",			"transf_add_Cpixel_pos_neg", 		transfAddCpixelPosNeg,			analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T1",							"transf_benesi_T1",							transfBenesiT1,							deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T1Mod",						"transf_benesi_T1Mod",					transfBenesiT1Mod,					deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T2",							"transf_benesi_T2",							transfBenesiT2,							deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T3",							"transf_benesi_T3",							transfBenesiT3,							deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T4",							"transf_benesi_T4",							transfBenesiT4,							deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_T5b",							"transf_benesi_T5b",						transfBenesiT5b,						deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_Mag_Forward",			"transf_benesi_mag_forward",		transfBenesiMagForward,			deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_Mag_Backward",		"transf_benesi_mag_backward",		transfBenesiMagBackward,		deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_Cube_Sphere",			"transf_benesi_cube_sphere",		transfBenesiCubeSphere,			deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Benesi_Sphere_Cube",			"transf_benesi_sphere_cube",		transfBenesiSphereCube,			deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Box_Offset",							"transf_box_offset",						transfBoxOffset,						analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Platonic Solid", 				"platonic_solid", 							platonicSolid, 							deltaDEType,		withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Rotation",								"transf_rotation", 							transfRotation, 						analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Scale",									"transf_scale", 								transfScale, 								analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Scale 3D",								"transf_scale_3D", 							transfScale3D, 							analyticDEType,	withoutDEFunction));
  fractalList->append(sFractalDescription("Transform - Spherical_Offset",				"transf_spherical_offset",			transfSphericalOffset,			analyticDEType,	withoutDEFunction));
}
