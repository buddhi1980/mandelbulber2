/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * list of fractal formulas
 */

#include "fractal_list.hpp"

QList<sFractalDescription> fractalList;

using namespace fractal;
void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	// list of fractal formulas (name visible in UI,
	//													internal name,
	//													internal ID,
	//													DE type,
	//													DE function
	//													enable CPixel addition
	//													bailout)

	//----------------------- complete formulas ------------------------------
	fractalList->append(sFractalDescription(
		"None", "", none, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 1));

	// Abox formulas
	fractalList->append(sFractalDescription("Abox - Mod 1", "abox_mod1", aboxMod1, analyticDEType,
		linearDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Abox - Mod 2", "abox_mod2", aboxMod2, analyticDEType,
		linearDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Abox - Mod Kali", "abox_mod_kali", aboxModKali, analyticDEType,
		linearDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Abox - Mod Kali-Eiffie", "abox_mod_kali_eiffie",
		aboxModKaliEiffie, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));
	fractalList->append(sFractalDescription("Abox - VS icen1", "abox_vs_icen1", aboxVSIcen1,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	// Aexion formulas
	fractalList->append(sFractalDescription(
		"Aexion", "aexion", aexion, deltaDEType, logarithmicDEFunction, cpixelAlreadyHas, 10000));
	fractalList->append(sFractalDescription("Aexion Octopus Mod", "aexion_octopus_mod",
		aexionOctopusMod, deltaDEType, logarithmicDEFunction, cpixelAlreadyHas, 10));

	// Amazing Surf formulas
	fractalList->append(sFractalDescription("Amazing Surf", "amazing_surf", amazingSurf,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 1", "amazing_surf_mod1", amazingSurfMod1,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Amazing Surf - Multi", "amazing_surf_multi",
		amazingSurfMulti, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	// Benesi formulas
	fractalList->append(sFractalDescription("Benesi", "benesi", benesi, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 100));
	fractalList->append(sFractalDescription("Benesi - Pine Tree", "benesi_pine_tree", benesiPineTree,
		analyticDEType, logarithmicDEFunction, cpixelAlreadyHas, 10));
	fractalList->append(sFractalDescription("Benesi - T1 Pine Tree", "benesi_t1_pine_tree",
		benesiT1PineTree, analyticDEType, logarithmicDEFunction, cpixelAlreadyHas, 10));
	fractalList->append(sFractalDescription("Benesi - Mag Transforms", "benesi_mag_transforms",
		benesiMagTransforms, deltaDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Benesi - Pwr2 Mandelbulb", "benesi_pwr2_mandelbulb", benesiPwr2Mandelbulb,
		analyticDEType, logarithmicDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", "box_fold_bulb_pow2",
		boxFoldBulbPow2, deltaDEType, linearDEFunction, cpixelEnabledByDefault, 10000));

	fractalList->append(sFractalDescription("Bristorbrot", "bristorbrot", bristorbrot, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Buffalo", "buffalo", buffalo, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Collatz formulas
	fractalList->append(sFractalDescription("Collatz", "collatz", collatz, analyticDEType,
		linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Collatz - Mod ", "collatz_mod", collatzMod,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("Fold Box - Mod 1", "fold_box_mod1", foldBoxMod1,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Generalized Fold Box", "generalized_fold_box",
		generalizedFoldBox, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Hypercomplex", "hypercomplex", hypercomplex,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Ides formulas
	fractalList->append(sFractalDescription(
		"Ides", "ides", ides, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription(
		"Ides 2", "ides2", ides2, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("IQ Bulb", "iq_bulb", iqBulb, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Kaleidoscopic IFS", "kaleidoscopic_ifs", kaleidoscopicIFS,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("Kalisets1 Mod", "kalisets1", kalisets1, analyticDEType,
		linearDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription(
		"Lkmitch", "lkmitch", lkmitch, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Makin3D-2", "makin3d2", makin3d2, deltaDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Mandelbox formulas
	fractalList->append(sFractalDescription("Mandelbox", "mandelbox", mandelbox, analyticDEType,
		linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Mandelbox - Menger", "mandelbox_menger", mandelboxMenger,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	// Mandelbulb formulas
	fractalList->append(sFractalDescription("Mandelbulb", "mandelbulb", mandelbulb, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb 2", "mandelbulb2", mandelbulb2,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb 3", "mandelbulb3", mandelbulb3,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb 4", "mandelbulb4", mandelbulb4,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Bermarte", "mandelbulb_bermarte",
		mandelbulbBermarte, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Power 2", "mandelbulb_power2",
		mandelbulbPower2, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Kali", "mandelbulb_kali", mandelbulbKali,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Kali Multi", "mandelbulb_kali_multi",
		mandelbulbKaliMulti, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Multi", "mandelbulb_multi", mandelbulbMulti,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Mandelbulb - Vary Power V1", "mandelbulb_vary_power_v1",
		mandelbulbVaryPowerV1, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Menger formulas
	fractalList->append(sFractalDescription("Menger sponge", "menger_sponge", mengerSponge,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Cross KIFS", "menger_cross_kifs", mengerCrossKIFS,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Cross Mod1", "menger_cross_mod1", mengerCrossMod1,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Pwr2 Polynomial", "menger_pwr2_poly",
		mengerPwr2Poly, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Mod 1", "menger_mod1", mengerMod1, analyticDEType,
		linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Middle Mod", "menger_middle_mod", mengerMiddleMod,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Octo", "menger_octo", mengerOcto,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Prism Shape", "menger_prism_shape",
		mengerPrismShape, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Prism Shape2", "menger_prism_shape2",
		mengerPrismShape2, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Smooth", "menger_smooth", mengerSmooth,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Menger - Smooth Mod1", "menger_smooth_mod1",
		mengerSmoothMod1, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	// Msltoe formulas
	fractalList->append(sFractalDescription("Msltoe - Donut", "msltoe_donut", msltoeDonut, deltaDEType,
		linearDEFunction, cpixelDisabledByDefault, 4));
	fractalList->append(sFractalDescription("Msltoe - Sym2 Mod", "msltoe_sym2_mod", msltoeSym2Mod,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Sym3 Mod", "msltoe_sym3_mod", msltoeSym3Mod,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Eiffie", "eiffie_msltoe", eiffieMsltoe,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Mod2", "msltoe_sym3_mod2",
		msltoeSym3Mod2, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Mod3", "msltoe_sym3_mod3",
		msltoeSym3Mod3, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Sym4 Mod", "msltoe_sym4_mod", msltoeSym4Mod,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Msltoe - Toroidal Bulb", "msltoe_toroidal", msltoeToroidal,
		analyticDEType, // TODO fix??
		logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(
		sFractalDescription("Msltoe - Toroidal Multi", "msltoe_toroidal_multi", msltoeToroidalMulti,
			analyticDEType, // TODO fix??
			logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Pseudo Kleinian formulas
	fractalList->append(sFractalDescription("Pseudo Kleinian - stdDE", "pseudo_kleinian1",
		pseudoKleinian1, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));
	fractalList->append(sFractalDescription("Pseudo Kleinian", "pseudo_kleinian2", pseudoKleinian2,
		analyticDEType, pseudoKleinianDEFunction, cpixelDisabledByDefault, 100));
	fractalList->append(sFractalDescription("Pseudo Kleinian - Mod 1", "pseudo_kleinian3",
		pseudoKleinian3, analyticDEType, pseudoKleinianDEFunction, cpixelDisabledByDefault, 100));

	// Quaternion formulas
	fractalList->append(sFractalDescription("Quaternion", "quaternion", quaternion, analyticDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Quaternion3D", "quaternion3d", quaternion3d,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Quick Dudley formulas
	fractalList->append(sFractalDescription("Quick Dudley", "quick_dudley", quickDudley, deltaDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10));
	fractalList->append(sFractalDescription("Quick Dudley - Mod", "quick_dudley_mod", quickDudleyMod,
		deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// Riemann formulas
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe", "riemann_sphere_msltoe",
		riemannSphereMsltoe, deltaDEType, logarithmicDEFunction, cpixelDisabledByDefault, 1.5));
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe V1", "riemann_sphere_msltoe_v1",
		riemannSphereMsltoeV1, deltaDEType, logarithmicDEFunction, cpixelDisabledByDefault, 10));
	fractalList->append(sFractalDescription("Riemann - Bulb Msltoe Mod2", "riemann_bulb_msltoe_mod2",
		riemannBulbMsltoeMod2, deltaDEType, logarithmicDEFunction, cpixelDisabledByDefault, 1.5));

	fractalList->append(sFractalDescription("Sierpinski 3D", "sierpinski3d", sierpinski3d,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("Smooth Mandelbox", "smooth_mandelbox", smoothMandelbox,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Xenodreambuie", "xenodreambuie", xenodreambuie,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Imaginary scator Power 2", "imaginary_scator_power2",
		imaginaryScatorPower2, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	// ------------------ 4D -----------------------

	fractalList->append(sFractalDescription(
		"Abox 4D", "abox_4D", abox4D, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Bristorbrot4D", "bristorbrot4D", bristorbrot4D,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Quaternion_4D", "quaternion_4D", quaternion4D,
		deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10));

	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D", "mandelbox_vary_scale_4d",
		mandelboxVaryScale4D, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100));

	fractalList->append(sFractalDescription("Menger 4D", "menger_4D", menger4D, analyticDEType,
		linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("Menger 4D Mod1", "menger_4D_mod1", menger4Dmod1,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("MixPinski 4D", "mixPinski_4D", mixPinski4D,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("Sierpinski 4D", "sierpinski_4D", sierpinski4D,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	//-------------------------- transforms -----------------------------------
	fractalList->append(
		sFractalDescription("T>Addition Constant", "transf_addition_constant",
			transfAdditionConstant, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Addition Constant VaryV1",
		"transf_addition_constant_varyV1", transfAdditionConstantVaryV1, analyticDEType,
		withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Add Cpixel", "transf_add_Cpixel",
		transfAddCpixel, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(
		sFractalDescription("T>Add Cpixel_Axis_Swap", "transf_add_Cpixel_axis_swap",
			transfAddCpixelAxisSwap, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(
		sFractalDescription("T>Add Cpixel_CxCy_Axis_Swap", "transf_add_Cpixel_CxCy_axis_swap",
			transfAddCpixelCxCyAxisSwap, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(
		sFractalDescription("T>Add Cpixel_Symmetrical", "transf_add_Cpixel_symmetrical",
			transfAddCpixelPosNeg, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(
		sFractalDescription("T>Add Cpixel_VaryV1", "transf_add_Cpixel_varyV1",
			transfAddCpixelVaryV1, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(sFractalDescription("T>Add exp2(z)", "transf_add_exp2_Z",
		transfAddExp2Z, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T1", "transf_benesi_T1",
		transfBenesiT1, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T1Mod", "transf_benesi_T1Mod",
		transfBenesiT1Mod, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T2", "transf_benesi_T2",
		transfBenesiT2, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T3", "transf_benesi_T3",
		transfBenesiT3, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T4", "transf_benesi_T4",
		transfBenesiT4, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Benesi_T5b", "transf_benesi_T5b",
		transfBenesiT5b, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Benesi_Mag_Forward", "transf_benesi_mag_forward",
			transfBenesiMagForward, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Benesi_Mag_Backward", "transf_benesi_mag_backward",
			transfBenesiMagBackward, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Benesi_Cube_Sphere", "transf_benesi_cube_sphere",
			transfBenesiCubeSphere, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Benesi_Sphere_Cube", "transf_benesi_sphere_cube",
			transfBenesiSphereCube, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Box_Fold", "transf_box_fold", transfBoxFold,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Box_Fold VaryV1", "transf_box_fold_varyV1",
		transfBoxFoldVaryV1, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Box_Fold XYZ", "transf_box_fold_XYZ",
		transfBoxFoldXYZ, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Box_Offset", "transf_box_offset",
		transfBoxOffset, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Constant", "transf_fabs_add_constant",
			transfFabsAddConstant, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Tglad fold", "transf_fabs_add_constantV2",
			transfFabsAddConstantV2, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Conditional", "transf_fabs_add_conditional",
			transfFabsAddConditional, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Fabs_Add_Multi", "transf_fabs_add_multi",
		transfFabsAddMulti, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Neg", "transf_neg_fabs_add_constant",
			transfNegFabsAddConstant, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Folding Tetra3D", "transf_folding_tetra3D",
		transfFoldingTetra3D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Iteration Weight", "transf_iteration_weight",
		transfIterationWeight, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Inverse Cylindrical", "transf_inv_cylindrical",
			transfInvCylindrical, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Linear Combine Cxyz", "transf_lin_combine_Cxyz",
			transfLinCombineCxyz, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(sFractalDescription("T>Menger Fold", "transf_menger_fold",
		transfMengerFold, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("T>Multiple angle", "transf_multiple_angle",
		transfMultipleAngle, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Octo Fold", "transf_octo_fold",
		transfOctoFold, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("T>Platonic Solid", "platonic_solid",
		platonicSolid, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Power2 Polynomial", "transf_pwr2_polynomial",
		transfPwr2Polynomial, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10));

	fractalList->append(sFractalDescription("T>Quaternion Fold", "transf_quaternion_fold",
		transfQuaternionFold, analyticDEType, logarithmicDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Reciprocal3", "transf_reciprocal3",
		transfReciprocal3, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Rotation", "transf_rotation", transfRotation,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Rotation_VaryV1", "transf_rotation_varyV1",
		transfRotationVaryV1, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Rotation Folding Plane", "transf_rotation_folding_plane",
			transfRotationFoldingPlane, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Rotated Folding", "transf_rotated_folding",
		transfRotatedFolding, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>R^Power", "transf_rpower", transfRPower,
		analyticDEType, logarithmicDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Rpow3", "transf_rPow3", transfRpow3,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Scale", "transf_scale", transfScale,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Scale VaryV1", "transf_scale_varyV1",
		transfScaleVaryV1, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Scale VaryVCL", "transf_scale_varyVCL",
		transfScaleVaryVCL, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Scale 3D", "transf_scale_3D", transfScale3D,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Sphere_Invert", "transf_sphere_inv",
		transfSphereInv, analyticDEType, linearDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(sFractalDescription("T>Sphere_InvertC", "transf_sphere_invC",
		transfSphereInvC, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100));

	fractalList->append(sFractalDescription("T>Spherical_Fold", "transf_spherical_fold",
		transfSphericalFold, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_Fold ABox", "transf_spherical_fold_abox",
			transfSphericalFoldAbox, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_Fold VaryV1", "transf_spherical_fold_varyV1",
			transfSphericalFoldVaryV1, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_Fold_VaryVCL", "transf_spherFold_varyVCL",
			transfSpherFoldVaryVCL, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_Pwr_Fold", "transf_spherical_pwr_fold",
			transfSphericalPwrFold, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Spherical_Offset", "transf_spherical_offset",
		transfSphericalOffset, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_OffsetVCL", "transf_spherical_offsetVCL",
			transfSphericalOffsetVCL, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("Transform -Surf Box_Fold", "transf_surf_box_fold",
		transfSurfBoxFold, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Surf Fold Multi", "transf_surf_fold_multi",
		transfSurfFoldMulti, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Zvector_Axis_Swap", "transf_zVector_axis_swap",
			transfZvectorAxisSwap, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	// 4D
	fractalList->append(
		sFractalDescription("T>Addition Constant_4D", "transf_addition_constant_4D",
			transfAdditionConstant4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Box_Fold_4D", "transf_box_fold_4D",
		transfBoxFold4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Conditional4D", "transf_fabs_add_conditional4D",
			transfFabsAddConditional4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Constant_4D", "transf_fabs_add_constant_4D",
			transfFabsAddConstant4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Fabs_Add_Tglad fold_4D", "transf_fabs_add_constantV2_4D",
			transfFabsAddConstantV24D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Iteration Weight_4D", "transf_iteration_weight_4D",
			transfIterationWeight4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Reciprocal4D", "transf_reciprocal4D",
		transfReciprocal4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("Transfrom Rotation 4D", "transf_rotation_4D",
		transfRotation4D, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(sFractalDescription("T>Scale_4D", "transf_scale_4D", transfScale4D,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100));

	fractalList->append(
		sFractalDescription("T>Spherical_Fold_4D", "transf_spherical_fold_4D",
			transfSphericalFold4D, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100));
}
