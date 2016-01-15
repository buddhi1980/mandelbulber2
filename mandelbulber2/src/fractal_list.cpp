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
	//list of fractal formulas (name visible in UI,
	//													internal name,
	//													internal ID,
	//													DE type,
	//													DE function
	//													enable CPixel addition
	//													bailout)

	//----------------------- complete formulas ------------------------------
	fractalList->append(sFractalDescription("None",
																					"",
																					none,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("AboxMod1",
																					"aboxMod1",
																					aboxMod1,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

  fractalList->append(sFractalDescription("AboxMod2",
                                          "aboxMod2",
                                          aboxMod2,
                                          analyticDEType,
                                          linearDEFunction,
                                          cpixelEnabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("AboxModKali",
																					"aboxModKali",
																					aboxModKali,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

  fractalList->append(sFractalDescription("AboxVS icen1",
                                          "aboxVS_icen1",
                                          aboxVSIcen1,
                                          analyticDEType,
                                          linearDEFunction,
                                          cpixelEnabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Aexion",
																					"aexion",
																					aexion,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10000));

	fractalList->append(sFractalDescription("Aexion_Octopus",
																					"aexionOctopus",
																					aexionOctopus,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Amazing_Surf",
																					"amazing_surf",
																					amazingSurf,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
                                          100));

  fractalList->append(sFractalDescription("Amazing_Surf_Mod1",
                                          "amazing_surf_mod1",
                                          amazingSurfMod1,
                                          analyticDEType,
                                          linearDEFunction,
                                          cpixelEnabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Benesi Transforms DeltaDE",
																					"benesi_transforms",
																					benesiTransforms,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Benesi",
																					"benesi",
																					benesi,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Benesi_Pine_Tree",
																					"benesi_pine_tree",
																					benesiPineTree,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Benesi_T1_Pine_Tree",
																					"benesi_T1_pine_tree",
																					benesiT1PineTree,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2",
																					"boxfold_bulbpow2",
																					boxFoldBulbPow2,
																					deltaDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					10000));

	fractalList->append(sFractalDescription("Bristorbrot",
																					"bristorbrot",
																					bristorbrot,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Buffalo",
																					"buffalo",
																					buffalo,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Eiffie's MsltoeJulia",
																					"eiffie_msltoe",
																					eiffieMsltoe,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

  fractalList->append(sFractalDescription("Fold_Box_Mod1",
                                          "fold_box_mod1",
                                          foldBoxMod1,
																					analyticDEType,
																					linearDEFunction,
                                          cpixelEnabledByDefault,
																					100));
	fractalList->append(sFractalDescription("Generalized Fold Box",
																					"generalized_fold_box",
																					generalizedFoldBox,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Hypercomplex",
																					"hypercomplex",
																					hypercomplex,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Ides 1",
																					"ides",
																					ides,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Ides 2",
																					"ides2",
																					ides2,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Kaleidoscopic IFS",
																					"ifs",
																					kaleidoscopicIFS,
																					analyticDEType,
																					linearDEFunction,
																					cpixelDisabledByDefault,
																					10));

  fractalList->append(sFractalDescription("Kalisets1 Mod",
																					"kalisets1",
																					kalisets1,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Lkmitch",
																					"lkmitch",
																					lkmitch,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Makin3D-2",
																					"makin3d2",
																					makin3d2,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbox",
																					"mandelbox",
																					mandelbox,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Mandelbox - extended",
																					"mandelbox103",
																					mandelbox103,
																					analyticDEType,
																					linearDEFunction,
																					cpixelAlreadyHas,
																					100));

	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D",
																					"mandelbox_vary_scale_4d",
																					mandelboxVaryScale4D,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Mandelbulb",
																					"mandelbulb",
																					mandelbulb,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb - extended",
																					"mandelbulb5",
																					mandelbulb5,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb 2",
																					"mandelbulb2",
																					mandelbulb2,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb 3",
																					"mandelbulb3",
																					mandelbulb3,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb 4",
																					"mandelbulb4",
																					mandelbulb4,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb Classic Power 2",
																					"mandelbulb_power_2",
																					fast_mandelbulb_power2,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Mandelbulb6 beta",
																					"mandelbulb6_beta",
																					mandelbulb6Beta,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

  fractalList->append(sFractalDescription("Mandelbulb_Multi",
																					"mandelbulb_multi",
																					mandelbulbMulti,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

  fractalList->append(sFractalDescription("Mandelbulb_Vary_PowerV1",
                                          "mandelbulb_vary_powerV1",
                                          mandelbulbVaryPowerV1,
                                          analyticDEType,
                                          logarithmicDEFunction,
                                          cpixelEnabledByDefault,
                                          10));

	fractalList->append(sFractalDescription("Menger Sponge - extended",
																					"menger_sponge105",
																					mengerSponge105,
																					analyticDEType,
																					linearDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Menger sponge",
																					"menger_sponge",
																					menger_sponge,
																					analyticDEType,
																					linearDEFunction,
																					cpixelDisabledByDefault,
																					10));

  fractalList->append(sFractalDescription("Menger-Mod1",
                                          "menger-mod1",
                                          mengerMod1,
																					analyticDEType,
																					linearDEFunction,
																					cpixelDisabledByDefault,
																					10));

	fractalList->append(sFractalDescription("MsltoeSym2",
																					"msltoesym2",
																					msltoesym2,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("MsltoeSym3",
																					"msltoesym3",
																					msltoesym3,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("MsltoeSym4",
																					"msltoesym4",
																					msltoesym4,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelAlreadyHas,
																					10));

	fractalList->append(sFractalDescription("Quaternion",
																					"quaternion",
																					quaternion,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Quaternion - extended",
																					"quaternion104",
																					quaternion104,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Quaternion3D",
																					"quaternion3D",
																					quaternion3D,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Quick-Dudley",
																					"quickdudley",
																					quickdudley,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Riemann Sphere Msltoe",
																					"riemann_sphere_msltoe",
																					riemannSphereMsltoe,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelDisabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Riemann Sphere Msltoe V1",
																					"riemann_sphere_msltoeV1",
																					riemannSphereMsltoeV1,
																					deltaDEType,
																					logarithmicDEFunction,
																					cpixelDisabledByDefault,
																					10));

	fractalList->append(sFractalDescription("Smooth Mandelbox",
																					"smooth_mandelbox",
																					smoothMandelbox,
																					analyticDEType,
																					linearDEFunction,
																					cpixelEnabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Xenodreambuie's formula",
																					"xenodreambuie",
																					xenodreambuie,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelEnabledByDefault,
																					10));

	//-------------------------- transforms -----------------------------------
	fractalList->append(sFractalDescription("Transform - Addition Constant",
																					"transf_addition_constant",
																					transfAdditionConstant,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

  fractalList->append(sFractalDescription("Transform - Addition Constant VaryV1",
                                          "transf_addition_constant_varyV1",
                                          transfAdditionConstantVaryV1,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Transform - Add Cpixel",
																					"transf_add_Cpixel",
																					transfAddCpixel,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelAlreadyHas,
																					100));

	fractalList->append(sFractalDescription("Transform - Add Cpixel_Axis_Swap",
																					"transf_add_Cpixel_axis_swap",
																					transfAddCpixelAxisSwap,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelAlreadyHas,
																					100));

	fractalList->append(sFractalDescription("Transform - Add Cpixel_Pos_Neg",
																					"transf_add_Cpixel_pos_neg",
																					transfAddCpixelPosNeg,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelAlreadyHas,
																					100));

  fractalList->append(sFractalDescription("Transform - Add Cpixel_VaryV1",
                                          "transf_add_Cpixel_varyV1",
                                          transfAddCpixelVaryV1,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelAlreadyHas,
                                          100));

	fractalList->append(sFractalDescription("Transform - Benesi_T1",
																					"transf_benesi_T1",
																					transfBenesiT1,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_T1Mod",
																					"transf_benesi_T1Mod",
																					transfBenesiT1Mod,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_T2",
																					"transf_benesi_T2",
																					transfBenesiT2,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_T3",
																					"transf_benesi_T3",
																					transfBenesiT3,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_T4",
																					"transf_benesi_T4",
																					transfBenesiT4,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_T5b",
																					"transf_benesi_T5b",
																					transfBenesiT5b,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_Mag_Forward",
																					"transf_benesi_mag_forward",
																					transfBenesiMagForward,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_Mag_Backward",
																					"transf_benesi_mag_backward",
																					transfBenesiMagBackward,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_Cube_Sphere",
																					"transf_benesi_cube_sphere",
																					transfBenesiCubeSphere,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Benesi_Sphere_Cube",
																					"transf_benesi_sphere_cube",
																					transfBenesiSphereCube,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Box_Fold",
																					"transf_box_fold",
																					transfBoxFold,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

  fractalList->append(sFractalDescription("Transform - Box_Fold XYZ",
                                          "transf_box_fold_XYZ",
                                          transfBoxFoldXYZ,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Transform - Box_Offset",
																					"transf_box_offset",
																					transfBoxOffset,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Multiple angle",
																					"transf_multiple_angle",
																					transfMultipleAngle,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Fabs_Add_Constant",
																					"transf_fabs_add_constant",
																					transfFabsAddConstant,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Fabs_Add_ConstantV2",
																					"transf_fabs_add_constantV2",
																					transfFabsAddConstantV2,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

  fractalList->append(sFractalDescription("Transform - Fabs_Add_Multi",
                                          "transf_fabs_add_multi",
                                          transfFabsAddMulti,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

  fractalList->append(sFractalDescription("Transform - Iteration Weight",
                                          "transf_iteration_weight",
                                          transfIterationWeight,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Transform - Neg_Fabs_Add_Constant",
																					"transf_neg_fabs_add_constant",
																					transfNegFabsAddConstant,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Platonic Solid",
																					"platonic_solid",
																					platonicSolid,
																					deltaDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Rotation",
																					"transf_rotation",
																					transfRotation,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - R^Power",
																					"transf_rpower",
																					transfRPower,
																					analyticDEType,
																					logarithmicDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Scale",
																					"transf_scale",
																					transfScale,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

  fractalList->append(sFractalDescription("Transform - Scale VaryV1",
                                          "transf_scale_varyV1",
                                          transfScaleVaryV1,
                                          analyticDEType,
                                          withoutDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Transform - Scale 3D",
																					"transf_scale_3D",
																					transfScale3D,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

  fractalList->append(sFractalDescription("Transform - Spherical_Fold",
                                          "transf_spherical_fold",
                                          transfSphericalFold,
                                          analyticDEType,
                                          linearDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

  fractalList->append(sFractalDescription("Transform - Spherical_Pwr_Fold",
                                          "transf_spherical_pwr_fold",
                                          transfSphericalPwrFold,
                                          analyticDEType,
                                          linearDEFunction,
                                          cpixelDisabledByDefault,
                                          100));

	fractalList->append(sFractalDescription("Transform - Spherical_Offset",
																					"transf_spherical_offset",
																					transfSphericalOffset,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

	fractalList->append(sFractalDescription("Transform - Zvector_Axis_Swap",
																					"transf_zVector_axis_swap",
																					transfZvectorAxisSwap,
																					analyticDEType,
																					withoutDEFunction,
																					cpixelDisabledByDefault,
																					100));

}
