/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
QList<cAbstractFractal *> newFractalList;

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
	fractalList->append(sFractalDescription("None", "", none, nullptr, analyticDEType,
		withoutDEFunction, cpixelDisabledByDefault, 1, analyticFunctionNone, coloringFunctionDefault));

	//------------------------ ANALYTIC DE FORMULAS ----------------------

	// ----- Logarithmic -------

	fractalList->append(sFractalDescription("Aexion", "aexion", aexion, AexionIteration,
		analyticDEType, logarithmicDEFunction, cpixelAlreadyHas, 10000, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	// Benesi formulas
	fractalList->append(sFractalDescription("Benesi - Pine Tree", "benesi_pine_tree", benesiPineTree,
		BenesiPineTreeIteration, analyticDEType, logarithmicDEFunction, cpixelAlreadyHas, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - Pwr2", "benesi", benesi, BenesiIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - Pwr2 Mandelbulb", "benesi_pwr2_mandelbulb",
		benesiPwr2Mandelbulb, BenesiPwr2MandelbulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Benesi - T1 Pine Tree", "benesi_t1_pine_tree",
		benesiT1PineTree, BenesiT1PineTreeIteration, analyticDEType, logarithmicDEFunction,
		cpixelAlreadyHas, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// boxBulb formulas

	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2 V2", "box_fold_bulb_pow2_v2",
		boxFoldBulbPow2V2, BoxFoldBulbPow2V2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2 V3", "box_fold_bulb_pow2_v3",
		boxFoldBulbPow2V3, BoxFoldBulbPow2V3Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Bulb Menger", "box_fold_bulb_menger",
		boxFoldBulbMenger, BoxFoldBulbMengerIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold Quat", "box_fold_quat", boxFoldQuat,
		BoxFoldQuatIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Box Fold 4D Bulb Pow 2", "box_fold4d_bulb_pow2",
		boxFold4dBulbPow2, BoxFold4dBulbPow2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Bristorbrot", "bristorbrot", bristorbrot,
		BristorbrotIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Bristorbrot V2", "bristorbrot2", bristorbrot2,
		Bristorbrot2Iteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Bristorbrot 4D", "bristorbrot4d", bristorbrot4d,
		Bristorbrot4dIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Buffalo", "buffalo", buffalo, BuffaloIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	// Nested sin() formulas
	fractalList->append(sFractalDescription("Coastalbrot", "coastalbrot", coastalbrot,
		CoastalbrotIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Hypercomplex", "hypercomplex", hypercomplex,
		HypercomplexIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("IQ Bulb", "iq_bulb", iqBulb, IqBulbIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	fractalList->append(sFractalDescription("Lkmitch", "lkmitch", lkmitch, LkmitchIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	fractalList->append(sFractalDescription("Makin3D-2", "makin3d2", makin3d2, Makin3d2Iteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionLogarithmic,
		coloringFunctionDefault));

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
	fractalList->append(sFractalDescription("Mandelbulb - Abs Power 2", "mandelbulb_abs_power2",
		mandelbulbAbsPower2, MandelbulbAbsPower2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Atan2 Power 2", "mandelbulb_atan2_power2",
		mandelbulbAtan2Power2, MandelbulbAtan2Power2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Bermarte", "mandelbulb_bermarte",
		mandelbulbBermarte, MandelbulbBermarteIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Eye", "mandelbulb_eye", mandelbulbEye,
		MandelbulbEyeIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 2,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Juliabulb", "mandelbulb_juliabulb",
		mandelbulbJuliabulb, MandelbulbJuliabulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kali", "mandelbulb_kali", mandelbulbKali,
		MandelbulbKaliIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kali Multi", "mandelbulb_kali_multi",
		mandelbulbKaliMulti, MandelbulbKaliMultiIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kosalos", "mandelbulb_kosalos",
		mandelbulbKosalos, MandelbulbKosalosIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Kosalos V2", "mandelbulb_kosalos_v2",
		mandelbulbKosalosV2, MandelbulbKosalosV2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Lambda", "mandelbulb_lambda",
		mandelbulbLambda, MandelbulbLambdaIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Modulus", "modulus_mandelbulb",
		modulusMandelbulb, ModulusMandelbulbIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Multi", "mandelbulb_multi", mandelbulbMulti,
		MandelbulbMultiIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Multi2", "mandelbulb_multi2",
		mandelbulbMulti2, MandelbulbMulti2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Power 1234", "mandelbulb_power1234",
		mandelbulbPower1234, MandelbulbPower1234Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Power 2", "mandelbulb_power2",
		mandelbulbPower2, MandelbulbPower2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Pow2 V2", "mandelbulb_pow2_v2",
		mandelbulbPow2V2, MandelbulbPow2V2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Quat", "mandelbulb_quat", mandelbulbQuat,
		MandelbulbQuatIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandelbulb - Vary Power V1", "mandelbulb_vary_power_v1",
		mandelbulbVaryPowerV1, MandelbulbVaryPowerV1Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mandeltorus", "mandeltorus", mandeltorus,
		MandeltorusIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// Msltoe formulas
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
		msltoeToroidal, MsltoeToroidalIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Msltoe - Toroidal Multi", "msltoe_toroidal_multi",
		msltoeToroidalMulti, MsltoeToroidalMultiIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// Quaternion formulas
	fractalList->append(sFractalDescription("Quaternion", "quaternion", quaternion,
		QuaternionIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion3D", "quaternion3d", quaternion3d,
		Quaternion3dIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion 4D", "quaternion4d", quaternion4d,
		Quaternion4dIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quaternion Cubic 4D", "quaternion_cubic4d",
		quaternionCubic4d, QuaternionCubic4dIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	// Quick Dudley formulas
	fractalList->append(sFractalDescription("Quick Dudley", "quick_dudley", quickDudley,
		QuickDudleyIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Quick Dudley - Mod", "quick_dudley_mod", quickDudleyMod,
		QuickDudleyModIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// Riemann formulas
	fractalList->append(sFractalDescription("Riemann - Sphere Hobold Pow4",
		"riemann_sphere_hobold_pow4", riemannSphereHoboldPow4, RiemannSphereHoboldPow4Iteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 2.5, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Hobold Pow8",
		"riemann_sphere_hobold_pow8", riemannSphereHoboldPow8, RiemannSphereHoboldPow8Iteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 2.5, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Hobold Multi",
		"riemann_sphere_hobold_multi", riemannSphereHoboldMulti, RiemannSphereHoboldMultiIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 2.5, analyticFunctionLogarithmic,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe V1", "riemann_sphere_msltoe_v1",
		riemannSphereMsltoeV1, RiemannSphereMsltoeV1Iteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe V2", "riemann_sphere_msltoe_v2",
		riemannSphereMsltoeV2, RiemannSphereMsltoeV2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Scator Power 2 StdR", "scator_power2_std_r",
		scatorPower2StdR, ScatorPower2StdRIteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Xenodreambuie", "xenodreambuie", xenodreambuie,
		XenodreambuieIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	// ----- Linear -------

	// Abox formulas
	fractalList->append(
		sFractalDescription("Abox - Mod 1", "abox_mod1", aboxMod1, AboxMod1Iteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionIFS));
	fractalList->append(
		sFractalDescription("Abox - Mod 2", "abox_mod2", aboxMod2, AboxMod2Iteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Abox - Mod 11", "abox_mod11", aboxMod11,
		AboxMod11Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Abox - Mod 12", "abox_mod12", aboxMod12,
		AboxMod12Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Abox - Mod 13", "abox_mod13", aboxMod13,
		AboxMod13Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Abox - Mod Kali", "abox_mod_kali", aboxModKali,
		AboxModKaliIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Abox - Mod Kali-Eiffie", "abox_mod_kali_eiffie",
		aboxModKaliEiffie, AboxModKaliEiffieIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Abox - Mod Kali-V2", "abox_mod_kali_v2", aboxModKaliV2,
		AboxModKaliV2Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Abox - Mod Kali-V3", "abox_mod_kali_v3", aboxModKaliV3,
		AboxModKaliV3Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionIFS));

	fractalList->append(sFractalDescription("Abox - Sphere 4D", "abox_sphere4d", aboxSphere4d,
		AboxSphere4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionLinear,  coloringFunctionABox));

	fractalList->append(sFractalDescription("Abox - Surf Box", "abox_surf_box", aboxSurfBox,
		AboxSurfBoxIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));

	fractalList->append(sFractalDescription("Abox - Tetra 4D", "abox_tetra4d", aboxTetra4d,
		AboxTetra4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionLinear,  coloringFunctionABox));

	fractalList->append(sFractalDescription("Abox - VS icen1", "abox_vs_icen1", aboxVSIcen1,
		AboxVSIcen1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("Abox 4D", "abox4d", abox4d, Abox4dIteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));

	// Amazing Surf formulas
	fractalList->append(sFractalDescription("Amazing Surf", "amazing_surf", amazingSurf,
		AmazingSurfIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionAmazingSurf));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 1", "amazing_surf_mod1",
		amazingSurfMod1, AmazingSurfMod1Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 2", "amazing_surf_mod2",
		amazingSurfMod2, AmazingSurfMod2Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 3", "amazing_surf_mod3",
		amazingSurfMod3, AmazingSurfMod3Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Amazing Surf - Mod 4", "amazing_surf_mod4",
		amazingSurfMod4, AmazingSurfMod4Iteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Amazing Surf - Multi", "amazing_surf_multi",
		amazingSurfMulti, AmazingSurfMultiIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));

	fractalList->append(sFractalDescription("Benesi - Mag Transforms", "benesi_mag_transforms",
		benesiMagTransforms, BenesiMagTransformsIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionLinear, coloringFunctionDefault));

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

	fractalList->append(sFractalDescription("Ifs - XY", "ifs_xy", ifsXY, IfsXYIteration,
		analyticDEType, linearDEFunction, cpixelDisabledByDefault, 100, analyticFunctionLinear,
		coloringFunctionDefault));

	fractalList->append(sFractalDescription("Kaleidoscopic IFS", "kaleidoscopic_ifs",
		kaleidoscopicIfs, KaleidoscopicIfsIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionIFS, coloringFunctionIFS));

	fractalList->append(sFractalDescription("Kalisets1 Mod", "kalisets1", kalisets1,
		Kalisets1Iteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLinear, coloringFunctionDefault));

	// Mandelbox formulas
	fractalList->append(
		sFractalDescription("Mandelbox", "mandelbox", mandelbox, MandelboxIteration, analyticDEType,
			linearDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Mandelbox - Menger", "mandelbox_menger", mandelboxMenger,
		MandelboxMengerIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Mandelbox - Smooth", "mandelbox_smooth", mandelboxSmooth,
		MandelboxSmoothIteration, analyticDEType, linearDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Mandelbox - Variable", "mandelbox_variable",
		mandelboxVariable, MandelboxVariableIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));
	fractalList->append(sFractalDescription("Mandelbox - Vary Scale 4D", "mandelbox_vary_scale4d",
		mandelboxVaryScale4d, MandelboxVaryScale4dIteration, analyticDEType, linearDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLinear, coloringFunctionABox));

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
	fractalList->append(sFractalDescription("Menger - Mod 1", "menger_mod1", mengerMod1,
		MengerMod1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Modulus", "modulus_menger_sponge",
		modulusMengerSponge, ModulusMengerSpongeIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));
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
	fractalList->append(sFractalDescription("Menger - Poly Fold", "menger_poly_fold", mengerPolyFold,
		MengerPolyFoldIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Pwr2 Polynomial", "menger_pwr2_poly",
		mengerPwr2Poly, MengerPwr2PolyIteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));
	fractalList->append(sFractalDescription("Menger - Smooth", "menger_smooth", mengerSmooth,
		MengerSmoothIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger - Smooth Mod1", "menger_smooth_mod1",
		mengerSmoothMod1, MengerSmoothMod1Iteration, analyticDEType, linearDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("Menger 4D", "menger4d", menger4d, Menger4dIteration, analyticDEType,
			linearDEFunction, cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger 4D Mod1", "menger4d_mod1", menger4dMod1,
		Menger4dMod1Iteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Mix Pinski 4D", "mix_pinski4d", mixPinski4d,
		MixPinski4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Sierpinski 3D", "sierpinski3d", sierpinski3d,
		Sierpinski3dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Sierpinski 4D", "sierpinski4d", sierpinski4d,
		Sierpinski4dIteration, analyticDEType, linearDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionIFS, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("Vicsek", "vicsek", vicsek, VicsekIteration, analyticDEType,
			linearDEFunction, cpixelDisabledByDefault, 10, analyticFunctionIFS, coloringFunctionIFS));

	// ----- JosLeys-Kleinian -----

	fractalList->append(sFractalDescription("JosLeys-Kleinian", "jos_kleinian", josKleinian,
		JosKleinianIteration, analyticDEType, josKleinianDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionJosKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("JosLeys-Kleinian V2", "jos_kleinian_v2", josKleinianV2,
		JosKleinianV2Iteration, analyticDEType, josKleinianDEFunction, cpixelDisabledByDefault, 10,
		analyticFunctionJosKleinian, coloringFunctionDefault));

	// ----- Pseudo Kleinian ----

	fractalList->append(sFractalDescription("Pseudo Kleinian", "pseudo_kleinian", pseudoKleinian,
		PseudoKleinianIteration, analyticDEType, pseudoKleinianDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionPseudoKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Pseudo Kleinian - Mod 1", "pseudo_kleinian_mod1",
		pseudoKleinianMod1, PseudoKleinianMod1Iteration, analyticDEType, pseudoKleinianDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionPseudoKleinian, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Pseudo Kleinian - Mod 2", "pseudo_kleinian_mod2",
		pseudoKleinianMod2, PseudoKleinianMod2Iteration, analyticDEType, pseudoKleinianDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionPseudoKleinian, coloringFunctionDefault));

	// ----- custom DE - dIFS formulas------

	fractalList->append(
		sFractalDescription("DIFS Box V1", "difs_box_v1", dIFSBoxV1, DIFSBoxV1Iteration, analyticDEType,
			dIFSDEFunction, cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Box Diagonal V1", "difs_box_diagonal_v1",
		dIFSBoxDiagonalV1, DIFSBoxDiagonalV1Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Box Diagonal V3", "difs_box_diagonal_v3",
		dIFSBoxDiagonalV3, DIFSBoxDiagonalV3Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Cylinder", "difs_cylinder", dIFSCylinder,
		DIFSCylinderIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Ellipsoid", "difs_ellipsoid", dIFSEllipsoid,
		DIFSEllipsoidIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Hextgrid2", "difs_hextgrid2", dIFSHextgrid2,
		DIFSHextgrid2Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Menger", "difs_menger", dIFSMenger,
		DIFSMengerIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Msltoe Donut", "difs_msltoe_donut", dIFSMsltoeDonut,
		DIFSMsltoeDonutIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("DIFS Prism", "difs_prism", dIFSPrism, DIFSPrismIteration, analyticDEType,
			dIFSDEFunction, cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("DIFS Sphere", "difs_sphere", dIFSSphere,
		DIFSSphereIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("DIFS Torus", "difs_torus", dIFSTorus, DIFSTorusIteration, analyticDEType,
			dIFSDEFunction, cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(
		sFractalDescription("DIFS Multi V1", "difs_multi_v1", dIFSMultiV1, DIFSMultiV1Iteration, analyticDEType,
			dIFSDEFunction, cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));


	// ---- custom DE - dIFS transforms----

	fractalList->append(sFractalDescription("T>DIFS Box", "transf_difs_box", transfDIFSBox,
		TransfDIFSBoxIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Box V2", "transf_difs_box_v2", transfDIFSBoxV2,
		TransfDIFSBoxV2Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Box V3", "transf_difs_box_v3", transfDIFSBoxV3,
		TransfDIFSBoxV3Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Cylinder", "transf_difs_cylinder",
		transfDIFSCylinder, TransfDIFSCylinderIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Cylinder V2", "transf_difs_cylinder_v2",
		transfDIFSCylinderV2, TransfDIFSCylinderV2Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Ellipsoid", "transf_difs_ellipsoid",
		transfDIFSEllipsoid, TransfDIFSEllipsoidIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Grid", "transf_difs_grid", transfDIFSGrid,
		TransfDIFSGridIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Grid V2", "transf_difs_grid_v2", transfDIFSGridV2,
		TransfDIFSGridV2Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Hextgrid2", "transf_difs_hextgrid2",
		transfDIFSHextgrid2, TransfDIFSHextgrid2Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Prism", "transf_difs_prism", transfDIFSPrism,
		TransfDIFSPrismIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Prism V2", "transf_difs_prism_v2",
		transfDIFSPrismV2, TransfDIFSPrismV2Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Sphere", "transf_difs_sphere", transfDIFSSphere,
		TransfDIFSSphereIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Torus", "transf_difs_torus", transfDIFSTorus,
		TransfDIFSTorusIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Torus V2", "transf_difs_torus_v2",
		transfDIFSTorusV2, TransfDIFSTorusV2Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Torus V3", "transf_difs_torus_v3",
		transfDIFSTorusV3, TransfDIFSTorusV3Iteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>DIFS Torus Grid", "transf_difs_torus_grid",
		transfDIFSTorusGrid, TransfDIFSTorusGridIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>DIFS Hybrid Color", "transf_difs_hybrid_color",
		transfDIFSHybridColor, TransfDIFSHybridColorIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// ---- custom DE - non-dIFS Formulas----

	fractalList->append(sFractalDescription("Fold Cut Cube", "fold_cut_cube", foldCutCube,
		FoldCutCubeIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Koch", "koch", koch,
		KochIteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Koch V2", "koch_v2", kochV2,
		KochV2Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Koch V3", "koch_v3", kochV3,
		KochV3Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger V2", "menger_v2", mengerV2,
		MengerV2Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Menger V3", "menger_v3", mengerV3,
		MengerV3Iteration, analyticDEType, dIFSDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Pseudo Kleinian - stdDE", "pseudo_kleinian_std_de",
		pseudoKleinianStdDE, PseudoKleinianStdDEIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Scator Power 2", "scator_power2", scatorPower2,
		ScatorPower2Iteration, analyticDEType, dIFSDEFunction, cpixelEnabledByDefault, 100,
		analyticFunctionDIFS, coloringFunctionDefault));


	//------------------------ DELTA DE FORMULAS ------------------------

	// ----- Logarithmic -----

	// Aexion formulas
	fractalList->append(sFractalDescription("Aexion Octopus Mod", "aexion_octopus_mod",
		aexionOctopusMod, AexionOctopusModIteration, deltaDEType, logarithmicDEFunction,
		cpixelAlreadyHas, 10, analyticFunctionNone, coloringFunctionDefault));

	// Ides formulas
	fractalList->append(
		sFractalDescription("Ides", "ides", ides, IdesIteration, deltaDEType, logarithmicDEFunction,
			cpixelEnabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Ides 2", "ides2", ides2, Ides2Iteration, deltaDEType,
		logarithmicDEFunction, cpixelEnabledByDefault, 10, analyticFunctionNone,
		coloringFunctionDefault));

	// Riemann formulas
	fractalList->append(sFractalDescription("Riemann - Bulb Msltoe Mod2", "riemann_bulb_msltoe_mod2",
		riemannBulbMsltoeMod2, RiemannBulbMsltoeMod2Iteration, deltaDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 1.5, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Riemann - Sphere Msltoe", "riemann_sphere_msltoe",
		riemannSphereMsltoe, RiemannSphereMsltoeIteration, deltaDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 1.5, analyticFunctionNone, coloringFunctionDefault));

	// Scator Algebra formulas
	fractalList->append(sFractalDescription("Scator Power 2 Real", "scator_power2_real",
		scatorPower2Real, ScatorPower2RealIteration, deltaDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("Scator Power 2 Imaginary", "scator_power2_imaginary",
		scatorPower2Imaginary, ScatorPower2ImaginaryIteration, deltaDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));

	// ----- Linear -----

	fractalList->append(sFractalDescription("Box Fold Bulb Pow 2", "box_fold_bulb_pow2",
		boxFoldBulbPow2, BoxFoldBulbPow2Iteration, deltaDEType, linearDEFunction,
		cpixelEnabledByDefault, 10000, analyticFunctionNone, coloringFunctionDefault));

	// Msltoe formulas
	fractalList->append(sFractalDescription("Msltoe - Donut", "msltoe_donut", msltoeDonut,
		MsltoeDonutIteration, deltaDEType, linearDEFunction, cpixelDisabledByDefault, 4,
		analyticFunctionNone, coloringFunctionDonut));

	//-------------------------- Transforms -----------------------------------

	// Abs Transforms
	fractalList->append(sFractalDescription("T>Abs Add Conditional", "transf_abs_add_conditional",
		transfAbsAddConditional, TransfAbsAddConditionalIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Conditional2", "transf_abs_add_conditional2",
		transfAbsAddConditional2, TransfAbsAddConditional2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Constant", "transf_abs_add_constant",
		transfAbsAddConstant, TransfAbsAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Constant 4D", "transf_abs_add_constant4d",
		transfAbsAddConstant4d, TransfAbsAddConstant4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Tglad Fold", "transf_abs_add_tglad_fold",
		transfAbsAddTgladFold, TransfAbsAddTgladFoldIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Tglad Fold 4D", "transf_abs_add_tglad_fold4d",
		transfAbsAddTgladFold4d, TransfAbsAddTgladFold4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Multi", "transf_abs_add_multi",
		transfAbsAddMulti, TransfAbsAddMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Multi 4D", "transf_abs_add_multi4d",
		transfAbsAddMulti4d, TransfAbsAddMulti4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Neg", "transf_neg_abs_add_constant",
		transfNegAbsAddConstant, TransfNegAbsAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Abs Add Conditional 4D",
		"transf_abs_add_conditional4d", transfAbsAddConditional4d, TransfAbsAddConditional4dIteration,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));

	// Add Transforms
	fractalList->append(sFractalDescription("T>Add Constant", "transf_add_constant",
		transfAddConstant, TransfAddConstantIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant 4D", "transf_add_constant4d",
		transfAddConstant4d, TransfAddConstant4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant Mod1", "transf_add_constant_mod1",
		transfAddConstantMod1, TransfAddConstantMod1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant Mod2", "transf_add_constant_mod2",
		transfAddConstantMod2, TransfAddConstantMod2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant Rot V1", "transf_add_constant_rot_v1",
		transfAddConstantRotV1, TransfAddConstantRotV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add Constant VaryV1", "transf_add_constant_vary_v1",
		transfAddConstantVaryV1, TransfAddConstantVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Add exp2(z)", "transf_add_exp2_z", transfAddExp2Z,
		TransfAddExp2ZIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

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
	fractalList->append(sFractalDescription("T>Add Cpixel - Scator", "transf_add_cpixel_scator",
		transfAddCpixelScator, TransfAddCpixelScatorIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(
		sFractalDescription("T>Add Cpixel - Sin Or Cos", "transf_add_cpixel_sin_or_cos",
			transfAddCpixelSinOrCos, TransfAddCpixelSinOrCosIteration, analyticDEType, withoutDEFunction,
			cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));
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
		TransfBenesiT2Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T3", "transf_benesi_t3", transfBenesiT3,
		TransfBenesiT3Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T4", "transf_benesi_t4", transfBenesiT4,
		TransfBenesiT4Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi T5b", "transf_benesi_t5b", transfBenesiT5b,
		TransfBenesiT5bIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Benesi Cube>Sphere", "transf_benesi_cube_sphere",
		transfBenesiCubeSphere, TransfBenesiCubeSphereIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Sphere>Cube", "transf_benesi_sphere_cube",
		transfBenesiSphereCube, TransfBenesiSphereCubeIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Benesi Mag Forward", "transf_benesi_mag_forward",
		transfBenesiMagForward, TransfBenesiMagForwardIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Benesi Mag Backward", "transf_benesi_mag_backward",
		transfBenesiMagBackward, TransfBenesiMagBackwardIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Box Fold Transforms
	fractalList->append(sFractalDescription("T>Box Fold", "transf_box_fold", transfBoxFold,
		TransfBoxFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold 4D", "transf_box_fold4d", transfBoxFold4d,
		TransfBoxFold4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold 4D Tglad", "transf_box_fold4d_tglad",
		transfBoxFold4dTglad, TransfBoxFold4dTgladIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold VaryV1", "transf_box_fold_vary_v1",
		transfBoxFoldVaryV1, TransfBoxFoldVaryV1Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Fold XYZ", "transf_box_fold_xyz", transfBoxFoldXYZ,
		TransfBoxFoldXYZIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Offset", "transf_box_offset", transfBoxOffset,
		TransfBoxOffsetIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Tiling 4D", "transf_box_tiling4d",
		transfBoxTiling4d, TransfBoxTiling4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Box Wrap 4D", "transf_box_wrap4d", transfBoxWrap4d,
		TransfBoxWrap4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Clamp 4D", "transf_clamp4d", transfClamp4d,
		TransfClamp4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Diagonal Fold", "transf_diagonal_fold",
		transfDiagonalFold, TransfDiagonalFoldIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Folding Tetra 3D", "transf_folding_tetra3d",
		transfFoldingTetra3d, TransfFoldingTetra3dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Gnarl", "transf_gnarl", transfGnarl,
		TransfGnarlIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Iteration Weight", "transf_iteration_weight",
		transfIterationWeight, TransfIterationWeightIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Iteration Weight 4D", "transf_iteration_weight4d",
		transfIterationWeight4d, TransfIterationWeight4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Inverse Cylindrical", "transf_inv_cylindrical",
		transfInvCylindrical, TransfInvCylindricalIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Linear Combine CXYZ", "transf_lin_combine_cxyz",
		transfLinCombineCXYZ, TransfLinCombineCXYZIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Menger Fold", "transf_menger_fold", transfMengerFold,
		TransfMengerFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Multiple angle", "transf_multiple_angle",
		transfMultipleAngle, TransfMultipleAngleIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Octo Fold", "transf_octo_fold", transfOctoFold,
		TransfOctoFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Offset S-Curve 4D", "transf_offset_s_curve4d",
		transfOffsetSCurve4d, TransfOffsetSCurve4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Parab Fold", "transf_parab_fold", transfParabFold,
		TransfParabFoldIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Platonic Solid", "transf_platonic_solid",
		transfPlatonicSolid, TransfPlatonicSolidIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Poly Fold Atan", "transf_poly_fold_atan",
		transfPolyFoldAtan, TransfPolyFoldAtanIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Poly Fold Atan2", "transf_poly_fold_atan2",
		transfPolyFoldAtan2, TransfPolyFoldAtan2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Poly Fold Sym XY", "transf_poly_fold_sym_xy",
		transfPolyFoldSymXY, TransfPolyFoldSymXYIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Poly Fold Sym Multi", "transf_poly_fold_sym_multi",
		transfPolyFoldSymMulti, TransfPolyFoldSymMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Power2 Polynomial", "transf_pwr2_polynomial",
		transfPwr2Polynomial, TransfPwr2PolynomialIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 10, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Quaternion Fold", "transf_quaternion_fold",
		transfQuaternionFold, TransfQuaternionFoldIteration, analyticDEType, logarithmicDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// R Power Transforms
	fractalList->append(sFractalDescription("T>Rpow3", "transf_rpow3", transfRpow3,
		TransfRpow3Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>R^Power", "transf_r_power", transfRPower,
		TransfRPowerIteration, analyticDEType, logarithmicDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Reciprocal3", "transf_reciprocal3", transfReciprocal3,
		TransfReciprocal3Iteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Reciprocal 4D", "transf_reciprocal4d",
		transfReciprocal4d, TransfReciprocal4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Rotation Transforms
	fractalList->append(sFractalDescription("T>Rotation", "transf_rotation", transfRotation,
		TransfRotationIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation 4D", "transf_rotation4d", transfRotation4d,
		TransfRotation4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation About Vec3", "transf_rotate_about_vec3",
		transfRotateAboutVec3, TransfRotateAboutVec3Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Rotation Iter Controls",
		"transf_rotation_iter_controls", transfRotationIterControls,
		TransfRotationIterControlsIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));
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

	// Scale Transforms
	fractalList->append(sFractalDescription("T>Scale", "transf_scale", transfScale,
		TransfScaleIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale Offset", "transf_scale_offset",
		transfScaleOffset, TransfScaleOffsetIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Scale OffsetV2", "transf_scale_offset_v2",
		transfScaleOffsetV2, TransfScaleOffsetV2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
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
	fractalList->append(sFractalDescription("T>Scale 4D", "transf_scale4d", transfScale4d,
		TransfScale4dIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));

	// Sin Cos Transforms
	fractalList->append(sFractalDescription("T>Sin Or Cos", "transf_sin_or_cos", transfSinOrCos,
		TransfSinOrCosIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Sin And Cos", "transf_sin_and_cos", transfSinAndCos,
		TransfSinAndCosIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Sin And Cos - Max", "transf_sin_and_cos_max",
		transfSinAndCosMax, TransfSinAndCosMaxIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// Spherical Transforms
	fractalList->append(sFractalDescription("T>Spherical Fold", "transf_spherical_fold",
		transfSphericalFold, TransfSphericalFoldIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold 4D", "transf_spherical_fold4d",
		transfSphericalFold4d, TransfSphericalFold4dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold ABox", "transf_spherical_fold_abox",
		transfSphericalFoldAbox, TransfSphericalFoldAboxIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold CHS", "transf_spherical_fold_chs",
		transfSphericalFoldCHS, TransfSphericalFoldCHSIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold Cuboid", "transf_spherical_fold_cuboid",
		transfSphericalFoldCuboid, TransfSphericalFoldCuboidIteration, analyticDEType,
		withoutDEFunction, cpixelDisabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold Parab", "transf_spherical_fold_parab",
		transfSphericalFoldParab, TransfSphericalFoldParabIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold VaryV1",
		"transf_spherical_fold_vary_v1", transfSphericalFoldVaryV1, TransfSphericalFoldVaryV1Iteration,
		analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold VaryVCL",
		"transf_spherical_fold_vary_vcl", transfSphericalFoldVaryVCL,
		TransfSphericalFoldVaryVCLIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Fold XYZ Bias",
		"transf_spherical_fold_xyz_bias", transfSphericalFoldXYZBias,
		TransfSphericalFoldXYZBiasIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault,
		100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Spherical Invert", "transf_spherical_inv",
		transfSphericalInv, TransfSphericalInvIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Spherical Invert V2", "transf_spherical_inv_v2",
		transfSphericalInvV2, TransfSphericalInvV2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Spherical Invert C", "transf_spherical_inv_c",
		transfSphericalInvC, TransfSphericalInvCIteration, analyticDEType, withoutDEFunction,
		cpixelAlreadyHas, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Spherical Pwr Fold", "transf_spherical_pwr_fold",
		transfSphericalPwrFold, TransfSphericalPwrFoldIteration, analyticDEType, withoutDEFunction,
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
	fractalList->append(sFractalDescription("T>Surf Box_Fold V2", "transf_surf_box_fold_v2",
		transfSurfBoxFoldV2, TransfSurfBoxFoldV2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Surf Box_Fold V2 4D", "transf_surf_box_fold_v24d",
		transfSurfBoxFoldV24d, TransfSurfBoxFoldV24dIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));
	fractalList->append(sFractalDescription("T>Surf Fold Multi", "transf_surf_fold_multi",
		transfSurfFoldMulti, TransfSurfFoldMultiIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Zvector Axis Swap", "transf_zvector_axis_swap",
		transfZvectorAxisSwap, TransfZvectorAxisSwapIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>DE Controls", "transf_de_controls",
		transfDEControls, TransfDEControlsIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	// ------------------ hybrid color trial -----------------------

	fractalList->append(sFractalDescription("T>Hybrid Color", "transf_hybrid_color",
		transfHybridColor, TransfHybridColorIteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Hybrid Color2", "transf_hybrid_color2",
		transfHybridColor2, TransfHybridColor2Iteration, analyticDEType, withoutDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionNone, coloringFunctionDefault));

	// ------------------ experimental  -----------------------

	fractalList->append(
		sFractalDescription("Testing", "testing", testing, TestingIteration, analyticDEType,
			dIFSDEFunction, cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	/*fractalList->append(
		sFractalDescription("Testing", "testing", testing, TestingIteration, analyticDEType,
			withoutDEFunction, cpixelEnabledByDefault, 100, analyticFunctionNone,
		coloringFunctionDefault));*/

	fractalList->append(sFractalDescription("Testing Log", "testing_log", testingLog,
		TestingLogIteration, analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 10,
		analyticFunctionLogarithmic, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Testing4d", "testing4d", testing4d, Testing4dIteration,
		analyticDEType, logarithmicDEFunction, cpixelEnabledByDefault, 100, analyticFunctionLogarithmic,
		coloringFunctionDefault));

	fractalList->append(sFractalDescription("T>Blockify", "transf_blockify", transfBlockify,
		TransfBlockifyIteration, analyticDEType, withoutDEFunction, cpixelDisabledByDefault, 100,
		analyticFunctionNone, coloringFunctionDefault)); //  log or linear???

	fractalList->append(sFractalDescription("Testing Transform", "testing_transform",
		testingTransform, TestingTransformIteration, analyticDEType, dIFSDEFunction,
		cpixelDisabledByDefault, 100, analyticFunctionDIFS, coloringFunctionDefault));

	fractalList->append(sFractalDescription("Testing Transform2", "testing_transform2",
		testingTransform2, TestingTransform2Iteration, analyticDEType, logarithmicDEFunction,
		cpixelEnabledByDefault, 100, analyticFunctionLogarithmic, coloringFunctionDefault));

	//====================== FRACTAL LIST - START ==================
	newFractalList.append(new cFractalAexion());
	newFractalList.append(new cFractalBenesiPineTree());
	newFractalList.append(new cFractalBenesi());
	newFractalList.append(new cFractalBenesiPwr2Mandelbulb());
	newFractalList.append(new cFractalBenesiT1PineTree());
	newFractalList.append(new cFractalBoxFoldBulbPow2V2());
	newFractalList.append(new cFractalBoxFoldBulbPow2V3());
	newFractalList.append(new cFractalBoxFoldBulbMenger());
	newFractalList.append(new cFractalBoxFoldQuat());
	newFractalList.append(new cFractalBoxFold4dBulbPow2());
	newFractalList.append(new cFractalBristorbrot());
	newFractalList.append(new cFractalBristorbrot2());
	newFractalList.append(new cFractalBristorbrot4d());
	newFractalList.append(new cFractalBuffalo());
	newFractalList.append(new cFractalCoastalbrot());
	newFractalList.append(new cFractalHypercomplex());
	newFractalList.append(new cFractalIqBulb());
	newFractalList.append(new cFractalLkmitch());
	newFractalList.append(new cFractalMakin3d2());
	newFractalList.append(new cFractalMandelbulb());
	newFractalList.append(new cFractalMandelbulb2());
	newFractalList.append(new cFractalMandelbulb3());
	newFractalList.append(new cFractalMandelbulb4());
	newFractalList.append(new cFractalMandelbulbAbsPower2());
	newFractalList.append(new cFractalMandelbulbAtan2Power2());
	newFractalList.append(new cFractalMandelbulbBermarte());
	newFractalList.append(new cFractalMandelbulbEye());
	newFractalList.append(new cFractalMandelbulbJuliabulb());
	newFractalList.append(new cFractalMandelbulbKali());
	newFractalList.append(new cFractalMandelbulbKaliMulti());
	newFractalList.append(new cFractalMandelbulbKosalos());
	newFractalList.append(new cFractalMandelbulbKosalosV2());
	newFractalList.append(new cFractalMandelbulbLambda());
	newFractalList.append(new cFractalModulusMandelbulb());
	newFractalList.append(new cFractalMandelbulbMulti());
	newFractalList.append(new cFractalMandelbulbMulti2());
	newFractalList.append(new cFractalMandelbulbPower1234());
	newFractalList.append(new cFractalMandelbulbPower2());
	newFractalList.append(new cFractalMandelbulbPow2V2());
	newFractalList.append(new cFractalMandelbulbQuat());
	newFractalList.append(new cFractalMandelbulbVaryPowerV1());
	newFractalList.append(new cFractalMandeltorus());
	newFractalList.append(new cFractalMsltoeSym2Mod());
	newFractalList.append(new cFractalMsltoeSym3Mod());
	newFractalList.append(new cFractalEiffieMsltoe());
	newFractalList.append(new cFractalMsltoeSym3Mod2());
	newFractalList.append(new cFractalMsltoeSym3Mod3());
	newFractalList.append(new cFractalMsltoeSym4Mod());
	newFractalList.append(new cFractalMsltoeToroidal());
	newFractalList.append(new cFractalMsltoeToroidalMulti());
	newFractalList.append(new cFractalQuaternion());
	newFractalList.append(new cFractalQuaternion3d());
	newFractalList.append(new cFractalQuaternion4d());
	newFractalList.append(new cFractalQuaternionCubic4d());
	newFractalList.append(new cFractalQuickDudley());
	newFractalList.append(new cFractalQuickDudleyMod());
	newFractalList.append(new cFractalRiemannSphereHoboldPow4());
	newFractalList.append(new cFractalRiemannSphereHoboldPow8());
	newFractalList.append(new cFractalRiemannSphereHoboldMulti());
	newFractalList.append(new cFractalRiemannSphereMsltoeV1());
	newFractalList.append(new cFractalRiemannSphereMsltoeV2());
	newFractalList.append(new cFractalScatorPower2StdR());
	newFractalList.append(new cFractalXenodreambuie());
	newFractalList.append(new cFractalAboxMod1());
	newFractalList.append(new cFractalAboxMod2());
	newFractalList.append(new cFractalAboxMod11());
	newFractalList.append(new cFractalAboxMod12());
	newFractalList.append(new cFractalAboxMod13());
	newFractalList.append(new cFractalAboxModKali());
	newFractalList.append(new cFractalAboxModKaliEiffie());
	newFractalList.append(new cFractalAboxModKaliV2());
	newFractalList.append(new cFractalAboxModKaliV3());
	newFractalList.append(new cFractalAboxSphere4d());
	newFractalList.append(new cFractalAboxSurfBox());
	newFractalList.append(new cFractalAboxTetra4d());
	newFractalList.append(new cFractalAboxVSIcen1());
	newFractalList.append(new cFractalAbox4d());
	newFractalList.append(new cFractalAmazingSurf());
	newFractalList.append(new cFractalAmazingSurfMod1());
	newFractalList.append(new cFractalAmazingSurfMod2());
	newFractalList.append(new cFractalAmazingSurfMod3());
	newFractalList.append(new cFractalAmazingSurfMod4());
	newFractalList.append(new cFractalAmazingSurfMulti());
	newFractalList.append(new cFractalBenesiMagTransforms());
	newFractalList.append(new cFractalCollatz());
	newFractalList.append(new cFractalCollatzMod());
	newFractalList.append(new cFractalFoldBoxMod1());
	newFractalList.append(new cFractalGeneralizedFoldBox());
	newFractalList.append(new cFractalIfsXY());
	newFractalList.append(new cFractalKaleidoscopicIfs());
	newFractalList.append(new cFractalKalisets1());
	newFractalList.append(new cFractalMandelbox());
	newFractalList.append(new cFractalMandelboxMenger());
	newFractalList.append(new cFractalMandelboxSmooth());
	newFractalList.append(new cFractalMandelboxVariable());
	newFractalList.append(new cFractalMandelboxVaryScale4d());
	newFractalList.append(new cFractalMengerSponge());
	newFractalList.append(new cFractalMengerCrossKIFS());
	newFractalList.append(new cFractalMengerCrossMod1());
	newFractalList.append(new cFractalMengerMod1());
	newFractalList.append(new cFractalModulusMengerSponge());
	newFractalList.append(new cFractalMengerMiddleMod());
	newFractalList.append(new cFractalMengerOcto());
	newFractalList.append(new cFractalMengerPrismShape());
	newFractalList.append(new cFractalMengerPrismShape2());
	newFractalList.append(new cFractalMengerPolyFold());
	newFractalList.append(new cFractalMengerPwr2Poly());
	newFractalList.append(new cFractalMengerSmooth());
	newFractalList.append(new cFractalMengerSmoothMod1());
	newFractalList.append(new cFractalMenger4d());
	newFractalList.append(new cFractalMenger4dMod1());
	newFractalList.append(new cFractalMixPinski4d());
	newFractalList.append(new cFractalSierpinski3d());
	newFractalList.append(new cFractalSierpinski4d());
	newFractalList.append(new cFractalVicsek());
	newFractalList.append(new cFractalJosKleinian());
	newFractalList.append(new cFractalJosKleinianV2());
	newFractalList.append(new cFractalPseudoKleinian());
	newFractalList.append(new cFractalPseudoKleinianMod1());
	newFractalList.append(new cFractalPseudoKleinianMod2());
	newFractalList.append(new cFractalDIFSBoxV1());
	newFractalList.append(new cFractalDIFSBoxDiagonalV1());
	newFractalList.append(new cFractalDIFSBoxDiagonalV3());
	newFractalList.append(new cFractalDIFSCylinder());
	newFractalList.append(new cFractalDIFSEllipsoid());
	newFractalList.append(new cFractalDIFSHextgrid2());
	newFractalList.append(new cFractalDIFSMenger());
	newFractalList.append(new cFractalDIFSMsltoeDonut());
	newFractalList.append(new cFractalDIFSPrism());
	newFractalList.append(new cFractalDIFSSphere());
	newFractalList.append(new cFractalDIFSTorus());
	newFractalList.append(new cFractalDIFSMultiV1());
	newFractalList.append(new cFractalTransfDIFSBox());
	newFractalList.append(new cFractalTransfDIFSBoxV2());
	newFractalList.append(new cFractalTransfDIFSBoxV3());
	newFractalList.append(new cFractalTransfDIFSCylinder());
	newFractalList.append(new cFractalTransfDIFSCylinderV2());
	newFractalList.append(new cFractalTransfDIFSEllipsoid());
	newFractalList.append(new cFractalTransfDIFSGrid());
	newFractalList.append(new cFractalTransfDIFSGridV2());
	newFractalList.append(new cFractalTransfDIFSHextgrid2());
	newFractalList.append(new cFractalTransfDIFSPrism());
	newFractalList.append(new cFractalTransfDIFSPrismV2());
	newFractalList.append(new cFractalTransfDIFSSphere());
	newFractalList.append(new cFractalTransfDIFSTorus());
	newFractalList.append(new cFractalTransfDIFSTorusV2());
	newFractalList.append(new cFractalTransfDIFSTorusV3());
	newFractalList.append(new cFractalTransfDIFSTorusGrid());
	newFractalList.append(new cFractalTransfDIFSHybridColor());
	newFractalList.append(new cFractalFoldCutCube());
	newFractalList.append(new cFractalKoch());
	newFractalList.append(new cFractalKochV2());
	newFractalList.append(new cFractalKochV3());
	newFractalList.append(new cFractalMengerV2());
	newFractalList.append(new cFractalMengerV3());
	newFractalList.append(new cFractalPseudoKleinianStdDE());
	newFractalList.append(new cFractalScatorPower2());
	newFractalList.append(new cFractalAexionOctopusMod());
	newFractalList.append(new cFractalIdes());
	newFractalList.append(new cFractalIdes2());
	newFractalList.append(new cFractalRiemannBulbMsltoeMod2());
	newFractalList.append(new cFractalRiemannSphereMsltoe());
	newFractalList.append(new cFractalScatorPower2Real());
	newFractalList.append(new cFractalScatorPower2Imaginary());
	newFractalList.append(new cFractalBoxFoldBulbPow2());
	newFractalList.append(new cFractalMsltoeDonut());
	newFractalList.append(new cFractalTransfAbsAddConditional());
	newFractalList.append(new cFractalTransfAbsAddConditional2());
	newFractalList.append(new cFractalTransfAbsAddConstant());
	newFractalList.append(new cFractalTransfAbsAddConstant4d());
	newFractalList.append(new cFractalTransfAbsAddTgladFold());
	newFractalList.append(new cFractalTransfAbsAddTgladFold4d());
	newFractalList.append(new cFractalTransfAbsAddMulti());
	newFractalList.append(new cFractalTransfAbsAddMulti4d());
	newFractalList.append(new cFractalTransfNegAbsAddConstant());
	newFractalList.append(new cFractalTransfAbsAddConditional4d());
	newFractalList.append(new cFractalTransfAddConstant());
	newFractalList.append(new cFractalTransfAddConstant4d());
	newFractalList.append(new cFractalTransfAddConstantMod1());
	newFractalList.append(new cFractalTransfAddConstantMod2());
	newFractalList.append(new cFractalTransfAddConstantRotV1());
	newFractalList.append(new cFractalTransfAddConstantVaryV1());
	newFractalList.append(new cFractalTransfAddExp2Z());
	newFractalList.append(new cFractalTransfAddCpixel());
	newFractalList.append(new cFractalTransfAddCpixelAxisSwap());
	newFractalList.append(new cFractalTransfAddCpixelRotated());
	newFractalList.append(new cFractalTransfAddCpixelCxCyAxisSwap());
	newFractalList.append(new cFractalTransfAddCpixelScator());
	newFractalList.append(new cFractalTransfAddCpixelSinOrCos());
	newFractalList.append(new cFractalTransfAddCpixelSymmetrical());
	newFractalList.append(new cFractalTransfAddCpixelVaryV1());
	newFractalList.append(new cFractalTransfBenesiT1());
	newFractalList.append(new cFractalTransfBenesiT1Mod());
	newFractalList.append(new cFractalTransfBenesiT2());
	newFractalList.append(new cFractalTransfBenesiT3());
	newFractalList.append(new cFractalTransfBenesiT4());
	newFractalList.append(new cFractalTransfBenesiT5b());
	newFractalList.append(new cFractalTransfBenesiCubeSphere());
	newFractalList.append(new cFractalTransfBenesiSphereCube());
	newFractalList.append(new cFractalTransfBenesiMagForward());
	newFractalList.append(new cFractalTransfBenesiMagBackward());
	newFractalList.append(new cFractalTransfBoxFold());
	newFractalList.append(new cFractalTransfBoxFold4d());
	newFractalList.append(new cFractalTransfBoxFold4dTglad());
	newFractalList.append(new cFractalTransfBoxFoldVaryV1());
	newFractalList.append(new cFractalTransfBoxFoldXYZ());
	newFractalList.append(new cFractalTransfBoxOffset());
	newFractalList.append(new cFractalTransfBoxTiling4d());
	newFractalList.append(new cFractalTransfBoxWrap4d());
	newFractalList.append(new cFractalTransfClamp4d());
	newFractalList.append(new cFractalTransfDiagonalFold());
	newFractalList.append(new cFractalTransfFoldingTetra3d());
	newFractalList.append(new cFractalTransfGnarl());
	newFractalList.append(new cFractalTransfIterationWeight());
	newFractalList.append(new cFractalTransfIterationWeight4d());
	newFractalList.append(new cFractalTransfInvCylindrical());
	newFractalList.append(new cFractalTransfLinCombineCXYZ());
	newFractalList.append(new cFractalTransfMengerFold());
	newFractalList.append(new cFractalTransfMultipleAngle());
	newFractalList.append(new cFractalTransfOctoFold());
	newFractalList.append(new cFractalTransfOffsetSCurve4d());
	newFractalList.append(new cFractalTransfParabFold());
	newFractalList.append(new cFractalTransfPlatonicSolid());
	newFractalList.append(new cFractalTransfPolyFoldAtan());
	newFractalList.append(new cFractalTransfPolyFoldAtan2());
	newFractalList.append(new cFractalTransfPolyFoldSymXY());
	newFractalList.append(new cFractalTransfPolyFoldSymMulti());
	newFractalList.append(new cFractalTransfPwr2Polynomial());
	newFractalList.append(new cFractalTransfQuaternionFold());
	newFractalList.append(new cFractalTransfRpow3());
	newFractalList.append(new cFractalTransfRPower());
	newFractalList.append(new cFractalTransfReciprocal3());
	newFractalList.append(new cFractalTransfReciprocal4d());
	newFractalList.append(new cFractalTransfRotation());
	newFractalList.append(new cFractalTransfRotation4d());
	newFractalList.append(new cFractalTransfRotateAboutVec3());
	newFractalList.append(new cFractalTransfRotationIterControls());
	newFractalList.append(new cFractalTransfRotationVaryV1());
	newFractalList.append(new cFractalTransfRotationFoldingPlane());
	newFractalList.append(new cFractalTransfRotationFolding());
	newFractalList.append(new cFractalTransfScale());
	newFractalList.append(new cFractalTransfScaleOffset());
	newFractalList.append(new cFractalTransfScaleOffsetV2());
	newFractalList.append(new cFractalTransfScaleVaryV212());
	newFractalList.append(new cFractalTransfScaleVaryMulti());
	newFractalList.append(new cFractalTransfScaleVaryV1());
	newFractalList.append(new cFractalTransfScaleVaryVCL());
	newFractalList.append(new cFractalTransfScale3d());
	newFractalList.append(new cFractalTransfScale4d());
	newFractalList.append(new cFractalTransfSinOrCos());
	newFractalList.append(new cFractalTransfSinAndCos());
	newFractalList.append(new cFractalTransfSinAndCosMax());
	newFractalList.append(new cFractalTransfSphericalFold());
	newFractalList.append(new cFractalTransfSphericalFold4d());
	newFractalList.append(new cFractalTransfSphericalFoldAbox());
	newFractalList.append(new cFractalTransfSphericalFoldCHS());
	newFractalList.append(new cFractalTransfSphericalFoldCuboid());
	newFractalList.append(new cFractalTransfSphericalFoldParab());
	newFractalList.append(new cFractalTransfSphericalFoldVaryV1());
	newFractalList.append(new cFractalTransfSphericalFoldVaryVCL());
	newFractalList.append(new cFractalTransfSphericalFoldXYZBias());
	newFractalList.append(new cFractalTransfSphericalInv());
	newFractalList.append(new cFractalTransfSphericalInvV2());
	newFractalList.append(new cFractalTransfSphericalInvC());
	newFractalList.append(new cFractalTransfSphericalPwrFold());
	newFractalList.append(new cFractalTransfSphericalOffset());
	newFractalList.append(new cFractalTransfSphericalOffsetVCL());
	newFractalList.append(new cFractalTransfSurfBoxFold());
	newFractalList.append(new cFractalTransfSurfBoxFoldV2());
	newFractalList.append(new cFractalTransfSurfBoxFoldV24d());
	newFractalList.append(new cFractalTransfSurfFoldMulti());
	newFractalList.append(new cFractalTransfZvectorAxisSwap());
	newFractalList.append(new cFractalTransfDEControls());
	newFractalList.append(new cFractalTransfHybridColor());
	newFractalList.append(new cFractalTransfHybridColor2());
	newFractalList.append(new cFractalTesting());
	newFractalList.append(new cFractalTestingLog());
	newFractalList.append(new cFractalTesting4d());
	newFractalList.append(new cFractalTransfBlockify());
	newFractalList.append(new cFractalTestingTransform());
	newFractalList.append(new cFractalTestingTransform2());
	//====================== FRACTAL LIST - END ==================

	for (cAbstractFractal *fractal : newFractalList)
	{
		fractal->CheckForErrors();
	}
}
