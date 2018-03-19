/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "fractal_formulas.hpp"

QList<sFractalDescription> fractalList;

using namespace fractal;
void DefineFractalList(QList<sFractalDescription> *fractalList)
{
	// list of fractal formulas (name visible in UI,
	//													internal name,
	//													internal ID,
	//													function pointer,
	//													DE type,
	//													DE function
	//													enable CPixel addition
	//													bailout
	//													analytic DE function
	//													coloring function)

	//----------------------- complete formulas ------------------------------
	fractalList->append(sFractalDescription("None", "", none, nullptr, deltaDEType, withoutDEFunction,
		cpixelDisabledByDefault, 1, analyticFunctionNone, coloringFunctionDefault));

	// Abox formulas
	fractalList->append(
		sFractalDescription("Abox - Mod 1", "abox_mod1", aboxMod1, AboxMod1Iteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionIFS));

	fractalList->append(sFractalDescription("Abox - Mod 2", "abox_mod2", aboxMod2, AboxMod2Iteration,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear,
		coloringFunctionABox2));

	fractalList->append(sFractalDescription("Abox - Mod 11", "abox_mod11", aboxMod11,
		AboxMod11Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox2));

	fractalList->append(sFractalDescription("Abox - Mod 12", "abox_mod12", aboxMod12,
		AboxMod12Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Abox - Mod 13", "abox_mod13", aboxMod13,
		AboxMod13Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Abox - Mod Kali", "abox_mod_kali", aboxModKali,
		AboxModKaliIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionIFS));

	fractalList->append(sFractalDescription("Abox - Mod Kali-Eiffie", "abox_mod_kali_eiffie",
		aboxModKaliEiffie, AboxModKaliEiffieIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox2));

	fractalList->append(sFractalDescription("Abox - VS icen1", "abox_vs_icen1", aboxVSIcen1,
		AboxVSIcen1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionDefault));

	// Aexion formulas
	fractalList->append(sFractalDescription("Aexion", "aexion", aexion, AexionIteration, deltaDEType,
		logarithmicDEFunction, cpixelAlreadyHas, 10000, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Aexion Octopus Mod", "aexion_octopus_mod",
		aexionOctopusMod, AexionOctopusModIteration, deltaDEType, logarithmicDEFunction,
		cpixelAlreadyHas, 10, analyticFunctionNone, coloringFunctionDefault));

	// Amazing Surf formulas
	fractalList->append(sFractalDescription("Amazing Surf", "amazing_surf", amazingSurf,
		AmazingSurfIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionAmazingSurf));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 1", "amazing_surf_mod1",
		amazingSurfMod1, AmazingSurfMod1Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox2));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 2", "amazing_surf_mod2",
		amazingSurfMod2, AmazingSurfMod2Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Amazing Surf - Multi", "amazing_surf_multi",
		amazingSurfMulti, AmazingSurfMultiIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox2));

	// Benesi formulas
	fractalList->append(sFractalDescription("Benesi", "benesi", benesi, BenesiIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - Pine Tree", "benesi_pine_tree", benesiPineTree,
		BenesiPineTreeIteration, analyticDEType, logarithmicDEFunction, cpixelAlreadyHas, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - T1 Pine Tree", "benesi_t1_pine_tree",
		benesiT1PineTree, BenesiT1PineTreeIteration, analyticDEType, logarithmicDEFunction,
		cpixelAlreadyHas, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - Mag Transforms", "benesi_mag_transforms",
		benesiMagTransforms, BenesiMagTransformsIteration, deltaDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - Pwr2 Mandelbulb", "benesi_pwr2_mandelbulb",
		benesiPwr2Mandelbulb, BenesiPwr2MandelbulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// boxBulb formulas
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", "box_fold_bulb_pow2",
		boxFoldBulbPow2, BoxFoldBulbPow2Iteration, deltaDEType, linearDEFunction,
		cpixelEnabledByDefault, 10000, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2 V2", "box_fold_bulb_pow2_v2",
		boxFoldBulbPow2V2, BoxFoldBulbPow2V2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2 V3", "box_fold_bulb_pow2_v3",
		boxFoldBulbPow2V3, BoxFoldBulbPow2V3Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Quat", "box_fold_quat", boxFoldQuat,
		BoxFoldQuatIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Bristorbrot", "bristorbrot", bristorbrot,
		BristorbrotIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Buffalo", "buffalo", buffalo, BuffaloIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	// Nested sin() formulas
	fractalList->append(sFractalDescription("Coastalbrot", "coastalbrot", coastalbrot,
		CoastalbrotIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// Modulus formulas
	fractalList->append(sFractalDescription("Modulus Menger Sponge", "modulus_menger_sponge",
		modulusMengerSponge, ModulusMengerSpongeIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Modulus Mandelbulb", "modulus_mandelbulb",
		modulusMandelbulb, ModulusMandelbulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// Collatz formulas
	fractalList->append(
		sFractalDescription("Collatz", "collatz", collatz, CollatzIteration, analyticDEType,
			linearDEFunction, cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Collatz - Mod ", "collatz_mod", collatzMod,
		CollatzModIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Fold Box - Mod 1", "fold_box_mod1", foldBoxMod1,
		FoldBoxMod1Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Generalized Fold Box", "generalized_fold_box",
		generalizedFoldBox, GeneralizedFoldBoxIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Hypercomplex", "hypercomplex", hypercomplex,
		HypercomplexIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// Ides formulas
	fractalList->append(
		sFractalDescription("Ides", "ides", ides, IdesIteration, deltaDEType, logarithmicDEFunction,
			cpixelEnabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Ides 2", "ides2", ides2, Ides2Iteration, deltaDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("IQ Bulb", "iq_bulb", iqBulb, IqBulbIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("JosLeys-Kleinian", "jos_kleinian", josKleinian,
		JosKleinianIteration, analyticDEType, josKleinianDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionJosKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Kaleidoscopic IFS", "kaleidoscopic_ifs",
		kaleidoscopicIfs, KaleidoscopicIfsIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Kalisets1 Mod", "kalisets1", kalisets1,
		Kalisets1Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Lkmitch", "lkmitch", lkmitch, LkmitchIteration,
		deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Makin3D-2", "makin3d2", makin3d2, Makin3d2Iteration,
		deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionNone,
		coloringFunctionDefault));

	// Mandelbox formulas
	fractalList->append(
		sFractalDescription("Mandelbox", "mandelbox", mandelbox, MandelboxIteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Mandelbox - Menger", "mandelbox_menger", mandelboxMenger,
		MandelboxMengerIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox2));
	fractalList->append(sFractalDescription("Mandelbox - Smooth", "mandelbox_smooth", mandelboxSmooth,
		MandelboxSmoothIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));

	// Mandelbulb formulas
	fractalList->append(sFractalDescription("Mandelbulb", "mandelbulb", mandelbulb,
		MandelbulbIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb 2", "mandelbulb2", mandelbulb2,
		Mandelbulb2Iteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb 3", "mandelbulb3", mandelbulb3,
		Mandelbulb3Iteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb 4", "mandelbulb4", mandelbulb4,
		Mandelbulb4Iteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Bermarte", "mandelbulb_bermarte",
		mandelbulbBermarte, MandelbulbBermarteIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Power 2", "mandelbulb_power2",
		mandelbulbPower2, MandelbulbPower2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Juliabulb", "mandelbulb_juliabulb",
		mandelbulbJuliabulb, MandelbulbJuliabulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kali", "mandelbulb_kali", mandelbulbKali,
		MandelbulbKaliIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kali Multi", "mandelbulb_kali_multi",
		mandelbulbKaliMulti, MandelbulbKaliMultiIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Multi", "mandelbulb_multi", mandelbulbMulti,
		MandelbulbMultiIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Multi2", "mandelbulb_multi2",
		mandelbulbMulti2, MandelbulbMulti2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Quat", "mandelbulb_quat", mandelbulbQuat,
		MandelbulbQuatIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Vary Power V1", "mandelbulb_vary_power_v1",
		mandelbulbVaryPowerV1, MandelbulbVaryPowerV1Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// Menger formulas
	fractalList->append(sFractalDescription("Menger sponge", "menger_sponge", mengerSponge,
		MengerSpongeIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Cross KIFS", "menger_cross_kifs",
		mengerCrossKIFS, MengerCrossKIFSIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Cross Mod1", "menger_cross_mod1",
		mengerCrossMod1, MengerCrossMod1Iteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Menger - Pwr2 Polynomial", "menger_pwr2_poly",
		mengerPwr2Poly, MengerPwr2PolyIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));

	fractalList->append(sFractalDescription("Menger - Mod 1", "menger_mod1", mengerMod1,
		MengerMod1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS,  coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Middle Mod", "menger_middle_mod",
		mengerMiddleMod, MengerMiddleModIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Octo", "menger_octo", mengerOcto,
		MengerOctoIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Prism Shape", "menger_prism_shape",
		mengerPrismShape, MengerPrismShapeIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Prism Shape2", "menger_prism_shape2",
		mengerPrismShape2, MengerPrismShape2Iteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Smooth", "menger_smooth", mengerSmooth,
		MengerSmoothIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Smooth Mod1", "menger_smooth_mod1",
		mengerSmoothMod1, MengerSmoothMod1Iteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));

	// Msltoe formulas
	fractalList->append(sFractalDescription("Msltoe - Donut", "msltoe_donut", msltoeDonut,
		MsltoeDonutIteration, deltaDEType, linearDEFunction, cpixelDisabledByDefault, 4,
		analyticFunctionNone, coloringFunctionDonut));
	fractalList->append(sFractalDescription("Msltoe - Sym2 Mod", "msltoe_sym2_mod", msltoeSym2Mod,
		MsltoeSym2ModIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Sym3 Mod", "msltoe_sym3_mod", msltoeSym3Mod,
		MsltoeSym3ModIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Eiffie", "eiffie_msltoe",
		eiffieMsltoe, EiffieMsltoeIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Mod2", "msltoe_sym3_mod2",
		msltoeSym3Mod2, MsltoeSym3Mod2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Julia Bulb Mod3", "msltoe_sym3_mod3",
		msltoeSym3Mod3, MsltoeSym3Mod3Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Sym4 Mod", "msltoe_sym4_mod", msltoeSym4Mod,
		MsltoeSym4ModIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Toroidal Bulb", "msltoe_toroidal",
		msltoeToroidal, MsltoeToroidalIteration, analyticDEType, // TODO fix??
		logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Toroidal Multi", "msltoe_toroidal_multi",
		msltoeToroidalMulti, MsltoeToroidalMultiIteration, analyticDEType, // TODO fix??
		logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	// Pseudo Kleinian formulas
	fractalList->append(sFractalDescription("Pseudo Kleinian - stdDE", "pseudo_kleinian_std_de",
		pseudoKleinianStdDE, PseudoKleinianStdDEIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Pseudo Kleinian", "pseudo_kleinian", pseudoKleinian,
		PseudoKleinianIteration, analyticDEType, pseudoKleinianDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionPseudoKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Pseudo Kleinian - Mod 1", "pseudo_kleinian_mod1",
		pseudoKleinianMod1, PseudoKleinianMod1Iteration, analyticDEType, pseudoKleinianDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionPseudoKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Pseudo Kleinian - Mod 2", "pseudo_kleinian_mod2",
		pseudoKleinianMod2, PseudoKleinianMod2Iteration, analyticDEType, pseudoKleinianDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionPseudoKleinian, coloringFunctionDefault));

	// Quaternion formulas
	fractalList->append(sFractalDescription("Quaternion", "quaternion", quaternion,
		QuaternionIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion3D", "quaternion3d", quaternion3d,
		Quaternion3dIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// Quick Dudley formulas
	fractalList->append(sFractalDescription("Quick Dudley", "quick_dudley", quickDudley,
		QuickDudleyIteration, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quick Dudley - Mod", "quick_dudley_mod", quickDudleyMod,
		QuickDudleyModIteration, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionNone, coloringFunctionDefault));

	// Riemann formulas
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe", "riemann_sphere_msltoe",
		riemannSphereMsltoe, RiemannSphereMsltoeIteration, deltaDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 1.5, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe V1", "riemann_sphere_msltoe_v1",
		riemannSphereMsltoeV1, RiemannSphereMsltoeV1Iteration, deltaDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Bulb Msltoe Mod2", "riemann_bulb_msltoe_mod2",
		riemannBulbMsltoeMod2, RiemannBulbMsltoeMod2Iteration, deltaDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 1.5, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Sierpinski 3D", "sierpinski3d", sierpinski3d,
		Sierpinski3dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Xenodreambuie", "xenodreambuie", xenodreambuie,
		XenodreambuieIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Imaginary scator Power 2", "imaginary_scator_power2",
		imaginaryScatorPower2, ImaginaryScatorPower2Iteration, deltaDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));

	// ------------------ 4D -----------------------

	fractalList->append(sFractalDescription("Abox 4D", "abox4d", abox4d, Abox4dIteration,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear,
		coloringFunctionABox2));
	fractalList->append(sFractalDescription("Bristorbrot 4D", "bristorbrot4d", bristorbrot4d,
		Bristorbrot4dIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbox Vary Scale 4D", "mandelbox_vary_scale4d",
		mandelboxVaryScale4d, MandelboxVaryScale4dIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));

	// Menger 4D formulas
	fractalList->append(
		sFractalDescription("Menger 4D", "menger4d", menger4d, Menger4dIteration, analyticDEType,
			linearDEFunction, cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger 4D Mod1", "menger4d_mod1", menger4dMod1,
		Menger4dMod1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mix Pinski 4D", "mix_pinski4d", mixPinski4d,
		MixPinski4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Sierpinski 4D", "sierpinski4d", sierpinski4d,
		Sierpinski4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion 4D", "quaternion4d", quaternion4d,
		Quaternion4dIteration, deltaDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion Cubic 4D", "quaternion_cubic4d",
		quaternionCubic4d, QuaternionCubic4dIteration, deltaDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	//-------------------------- transforms -----------------------------------

	// Abs Transforms
	fractalList->append(sFractalDescription("T>Abs Add Constant", "transf_abs_add_constant",
		transfAbsAddConstant, TransfAbsAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Tglad Fold", "transf_abs_add_tglad_fold",
		transfAbsAddTgladFold, TransfAbsAddTgladFoldIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Conditional", "transf_abs_add_conditional",
		transfAbsAddConditional, TransfAbsAddConditionalIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Multi", "transf_abs_add_multi",
		transfAbsAddMulti, TransfAbsAddMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Neg", "transf_neg_abs_add_constant",
		transfNegAbsAddConstant, TransfNegAbsAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Add Transforms
	fractalList->append(sFractalDescription("T>Add Constant", "transf_add_constant",
		transfAddConstant, TransfAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant VaryV1", "transf_add_constant_vary_v1",
		transfAddConstantVaryV1, TransfAddConstantVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add exp2(z)", "transf_add_exp2_z", transfAddExp2Z,
		TransfAddExp2ZIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant Mod1", "transf_add_constant_mod1",
		transfAddConstantMod1, TransfAddConstantMod1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Add Cpixel Transforms
	fractalList->append(sFractalDescription("T>Add Cpixel", "transf_add_cpixel", transfAddCpixel,
		TransfAddCpixelIteration, analyticDEType, withoutDEFunction, cpixelAlreadyHas, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Cpixel - Axis Swap", "transf_add_cpixel_axis_swap",
		transfAddCpixelAxisSwap, TransfAddCpixelAxisSwapIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Cpixel - Rotated", "transf_add_cpixel_rotated",
		transfAddCpixelRotated, TransfAddCpixelRotatedIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("T>Add Cpixel - CxCy Axis Swap", "transf_add_cpixel_cx_cy_axis_swap",
			transfAddCpixelCxCyAxisSwap, TransfAddCpixelCxCyAxisSwapIteration, analyticDEType,
			withoutDEFunction, cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("T>Add Cpixel - Symmetrical", "transf_add_cpixel_symmetrical",
			transfAddCpixelSymmetrical, TransfAddCpixelSymmetricalIteration, analyticDEType,
			withoutDEFunction, cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Cpixel - VaryV1", "transf_add_cpixel_vary_v1",
		transfAddCpixelVaryV1, TransfAddCpixelVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));


	// Benesi Transforms
	fractalList->append(sFractalDescription("T>Benesi T1", "transf_benesi_t1", transfBenesiT1,
		TransfBenesiT1Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T1 Mod", "transf_benesi_t1_mod",
		transfBenesiT1Mod, TransfBenesiT1ModIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T2", "transf_benesi_t2", transfBenesiT2,
		TransfBenesiT2Iteration, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T3", "transf_benesi_t3", transfBenesiT3,
		TransfBenesiT3Iteration, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T4", "transf_benesi_t4", transfBenesiT4,
		TransfBenesiT4Iteration, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T5b", "transf_benesi_t5b", transfBenesiT5b,
		TransfBenesiT5bIteration, deltaDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Mag Forward", "transf_benesi_mag_forward",
		transfBenesiMagForward, TransfBenesiMagForwardIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Mag Backward", "transf_benesi_mag_backward",
		transfBenesiMagBackward, TransfBenesiMagBackwardIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Cube>Sphere", "transf_benesi_cube_sphere",
		transfBenesiCubeSphere, TransfBenesiCubeSphereIteration, deltaDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Sphere>Cube", "transf_benesi_sphere_cube",
		transfBenesiSphereCube, TransfBenesiSphereCubeIteration, deltaDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Box Fold Transforms
	fractalList->append(sFractalDescription("T>Box Fold", "transf_box_fold", transfBoxFold,
		TransfBoxFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold VaryV1", "transf_box_fold_vary_v1",
		transfBoxFoldVaryV1, TransfBoxFoldVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold XYZ", "transf_box_fold_xyz", transfBoxFoldXYZ,
		TransfBoxFoldXYZIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Offset", "transf_box_offset", transfBoxOffset,
		TransfBoxOffsetIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Folding Tetra 3D", "transf_folding_tetra3d",
		transfFoldingTetra3d, TransfFoldingTetra3dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Iteration Weight", "transf_iteration_weight",
		transfIterationWeight, TransfIterationWeightIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Inverse Cylindrical", "transf_inv_cylindrical",
		transfInvCylindrical, TransfInvCylindricalIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Linear Combine CXYZ", "transf_lin_combine_cxyz",
		transfLinCombineCXYZ, TransfLinCombineCXYZIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Menger Fold", "transf_menger_fold", transfMengerFold,
		TransfMengerFoldIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Multiple angle", "transf_multiple_angle",
		transfMultipleAngle, TransfMultipleAngleIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Octo Fold", "transf_octo_fold", transfOctoFold,
		TransfOctoFoldIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Parab Fold", "transf_parab_fold", transfParabFold,
		TransfParabFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Platonic Solid", "transf_platonic_solid",
		transfPlatonicSolid, TransfPlatonicSolidIteration, deltaDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Power2 Polynomial", "transf_pwr2_polynomial",
		transfPwr2Polynomial, TransfPwr2PolynomialIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Quaternion Fold", "transf_quaternion_fold",
		transfQuaternionFold, TransfQuaternionFoldIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Reciprocal3", "transf_reciprocal3", transfReciprocal3,
		TransfReciprocal3Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	// Rotation Transforms
	fractalList->append(sFractalDescription("T>Rotation", "transf_rotation", transfRotation,
		TransfRotationIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation VaryV1", "transf_rotation_vary_v1",
		transfRotationVaryV1, TransfRotationVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation Folding Plane",
		"transf_rotation_folding_plane", transfRotationFoldingPlane,
		TransfRotationFoldingPlaneIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation Folding", "transf_rotation_folding",
		transfRotationFolding, TransfRotationFoldingIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// R Power Transforms
	fractalList->append(sFractalDescription("T>R^Power", "transf_r_power", transfRPower,
		TransfRPowerIteration, analyticDEType, logarithmicDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rpow3", "transf_rpow3", transfRpow3,
		TransfRpow3Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	// Scale Transforms
	fractalList->append(sFractalDescription("T>Scale", "transf_scale", transfScale,
		TransfScaleIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale Vary V2.12", "transf_scale_vary_v212",
		transfScaleVaryV212, TransfScaleVaryV212Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Scale Vary Multi", "transf_scale_vary_multi",
		transfScaleVaryMulti, TransfScaleVaryMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Scale VaryV1", "transf_scale_vary_v1",
		transfScaleVaryV1, TransfScaleVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale VaryVCL", "transf_scale_vary_vcl",
		transfScaleVaryVCL, TransfScaleVaryVCLIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale 3D", "transf_scale3d", transfScale3d,
		TransfScale3dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	// Spherical Transforms
	fractalList->append(sFractalDescription("T>Spherical Invert", "transf_spherical_inv",
		transfSphericalInv, TransfSphericalInvIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Invert C", "transf_spherical_inv_c",
		transfSphericalInvC, TransfSphericalInvCIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold", "transf_spherical_fold",
		transfSphericalFold, TransfSphericalFoldIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold ABox", "transf_spherical_fold_abox",
		transfSphericalFoldAbox, TransfSphericalFoldAboxIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold CHS", "transf_spherical_fold_chs",
		transfSphericalFoldCHS, TransfSphericalFoldCHSIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold Cuboid", "transf_spherical_fold_cuboid",
		transfSphericalFoldCuboid, TransfSphericalFoldCuboidIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold Parab", "transf_spherical_fold_parab",
		transfSphericalFoldParab, TransfSphericalFoldParabIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold VaryV1",
		"transf_spherical_fold_vary_v1", transfSphericalFoldVaryV1, TransfSphericalFoldVaryV1Iteration,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold VaryVCL",
		"transf_spherical_fold_vary_vcl", transfSphericalFoldVaryVCL,
		TransfSphericalFoldVaryVCLIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold XYZ Bias",
		"transf_spherical_fold_xyz_bias", transfSphericalFoldXYZBias,
		TransfSphericalFoldXYZBiasIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Pwr Fold", "transf_spherical_pwr_fold",
		transfSphericalPwrFold, TransfSphericalPwrFoldIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Offset", "transf_spherical_offset",
		transfSphericalOffset, TransfSphericalOffsetIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical OffsetVCL", "transf_spherical_offset_vcl",
		transfSphericalOffsetVCL, TransfSphericalOffsetVCLIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Surf Transforms
	fractalList->append(sFractalDescription("T>Surf Box_Fold", "transf_surf_box_fold",
		transfSurfBoxFold, TransfSurfBoxFoldIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Surf Fold Multi", "transf_surf_fold_multi",
		transfSurfFoldMulti, TransfSurfFoldMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Zvector Axis Swap", "transf_zvector_axis_swap",
		transfZvectorAxisSwap, TransfZvectorAxisSwapIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// ------------------ 4D -----------------------

	fractalList->append(sFractalDescription("T>Add Constant 4D", "transf_add_constant4d",
		transfAddConstant4d, TransfAddConstant4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold 4D", "transf_box_fold4d", transfBoxFold4d,
		TransfBoxFold4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Box Fold 4D Tglad", "transf_box_fold4d_tglad",
		transfBoxFold4dTglad, TransfBoxFold4dTgladIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Abs Add Conditional 4D",
		"transf_abs_add_conditional4d", transfAbsAddConditional4d, TransfAbsAddConditional4dIteration,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Constant 4D", "transf_abs_add_constant4d",
		transfAbsAddConstant4d, TransfAbsAddConstant4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Tglad Fold 4D", "transf_abs_add_tglad_fold4d",
		transfAbsAddTgladFold4d, TransfAbsAddTgladFold4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Iteration Weight 4D", "transf_iteration_weight4d",
		transfIterationWeight4d, TransfIterationWeight4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Reciprocal 4D", "transf_reciprocal4d",
		transfReciprocal4d, TransfReciprocal4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation 4D", "transf_rotation4d", transfRotation4d,
		TransfRotation4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale 4D", "transf_scale4d", transfScale4d,
		TransfScale4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold 4D", "transf_spherical_fold4d",
		transfSphericalFold4d, TransfSphericalFold4dIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// ------------------ hybrid color trial -----------------------

	fractalList->append(sFractalDescription("T>Hybrid Color", "transf_hybrid_color",
		transfHybridColor, TransfHybridColorIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Hybrid Color2", "transf_hybrid_color2",
		transfHybridColor2, TransfHybridColor2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Testing", "testing", testing, TestingIteration,
		analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear,
		coloringFunctionDefault));

	fractalList->append(sFractalDescription("Testing4d", "testing4d", testing4d, Testing4dIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLogarithmic,
		coloringFunctionDefault));
}
