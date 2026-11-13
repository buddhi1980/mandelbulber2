/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Robert Pancoast
 *
 * regression test: the bundled examples it renders
 */

#include "regression_test.hpp"

QStringList cRegressionTest::ExampleFiles()
{
	// A fixed list, so that a new example doesn't change how long the regression test takes; add
	// an example here after measuring its render time. These are the examples in the examples
	// folder on 2026-11-13, sorted by path, without the slowest: those that took more than 20
	// seconds for two renders on 32 threads of an Intel Core i9-14900K (Monte Carlo scenes at 320
	// pixels, divided by 4 for their 160 pixels), or more than 60 seconds on a 4-core GitHub
	// runner. A listed example that is missing from the examples folder fails its scene.
	return {
		"EXR example.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box DiagV1 complex "
		"primitive.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box DiagV1 "
		"pseudoKl.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box DiagV3 "
		"asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box DiagV3 complex "
		"primitive.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box Frame asa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Box Frame asa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS BoxV1 asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS BoxV1 flower.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Cylinder complex "
		"primitive.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Cylinder rocket.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Cylinder tree.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS GridV2 boxBulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Hextgrid2 "
		"sphInv.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Menger "
		"Ellipsoid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS "
		"Polyhedra_hexgrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Sphere MBulb a2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Sphere MBulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Torus asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Torus "
		"polyhedron.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS Torus twist "
		"asa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS_AmazIfs_Cyl.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/DIFS_AmazIfs_Hex.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Jos Leys Kleinian Hybrid "
		"esc7.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Jos Leys Kleinian Hybrid "
		"inv cos.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Jos Leys Kleinian "
		"sphereInversion.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Jos Leys Kleinian v3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/KIFS_mandelbulb_trapLights.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV5 01.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV5_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV5_KochV5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/KochV5_absAddConst.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/KochV5_polyFld_sphereFld.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Koch_Ifs aaa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Koch_Ifs aaa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Koch_Ifs aaa3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Koch_Ifs aaa4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/MbulbAbsPow2_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/MbulbAbsPow2_002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Menger 4D Mod1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/MengerMid_RotVary_SphOffsetVCL.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/MengerV4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/MengerV5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Mix Pinski 4D - DE linear "
		"cube.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Mix Pinski 4D hybrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/OctahedronMandalayMenger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/RiemannSphereMsltoeV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/Sierpinski 4D.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsBxFrame_T-DifsCayley2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsCayley2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsCayley2_coloredByChessboard.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsChessboard_T-DifsSphereGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsChessboard_coloredBy "
		"FoldBox.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsHelixMenger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsHelixMenger_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsHelixMenger_002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsHelixV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsHelixV2_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsOctahedron2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsOctahedron2a.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsOctahedron2b.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T-DifsTorusMenger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsTorusMenger_T-DifsClipCustom.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsTorusMenger_T-DifsClipCustom_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsTorusMenger_T-DifsPiriform.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T-DifsTorusMenger_menger7.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Chessboard hybrid "
		"color.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Chessboard.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS GridV3 001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS GridV3 002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Helix 001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Helix 002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Helix 003.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Helix Menger "
		"001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS SphereGrid IQ.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Spring "
		"MixPinski.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Spring "
		"PolyFld.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Torus4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Torus4_sphere.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_DIFS Tube _ Tube.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_PolyfoldAtan2Iter_sierp.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_absAdd_sphInv4_menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_absSym3_koch.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_absSym3_octahedron.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_absSym3_rotCheby_hextGriw2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_absSym3_tube.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_bxFoldInfy_menger3_bxFrame.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_bxFoldInfy_sphGrid3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_difsSupershape.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_difsSupershapeV2 "
		"a1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_difsSupershapeV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_difsSupershape_sphGrid3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_sphInvV4 menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_sphInvV4_abxKali_hexGrid2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_sphInvV4_abxKali_sphGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/T_sphInvV4_menger3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/T_sphInvV4_menger_sphGridV3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/TransfDIFSEllipsoid "
		"asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/TransfSphereCoordInv_bulbs.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aBox4D_addCpixel4D.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox11_aboxSurfBox.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod1 V2.11.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod11_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod11_002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod11_003.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/aboxMod11_addCpixelRotate.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod11_for8cA.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod11_hybrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod13Surf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod13_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod13_002.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod14_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod14_torus.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod15_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/aboxMod15_meng3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_cylinder.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_donut4dV2 naa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_donut4dV2 naa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_donut4d_aa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_donut4d_aa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_donut4d_abs.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_klein.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod11_rotBox.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod11_rotVaryV1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod_kali_v2 aa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod_kali_v2 asurf "
		"mode.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod_kali_v2 "
		"polfFoldSym.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_mod_kali_v3 "
		"hexgridDIFS.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_octahedron.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_sphere4D.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/abox_tetra.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/amazingIFS boxFrame.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/amazingIFS cyl.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/amazingIFS hexprsm2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/amazingIFS shroom.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/amazing_surf_mod2_PlatonicSolid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/amazing_surf_mod2_aaa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/amazing_surf_mod2_mandelbulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf julia "
		"varylimits.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf3_mandalay.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf4_sphere_invert.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf4_worms.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurfKlein_difsGreek.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/asurf_fakelights_backgroundl.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_foldingTetra.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/asurf_invert_spherical.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_julia.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_klein sph.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_klein.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_kleinV2 aa.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_kleinV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/asurf_polyfolds.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/asurf_spheroids_on_sticks.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/blockifyV2_PseudoKlien.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/boolean_boxFoldBulb_quat.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/box4dBulb aab5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v2 "
		"radDEcolor.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v2_01.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v2_twice.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v3 "
		"iridescene.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v3_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxFoldBulb_v3_a01.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/boxFoldBulb_v3_benesiHybrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/boxWrap-bulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/bristorbrot v2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/collatz mod hybrid1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/difsGreek_kochV4_asurfKlein_CUT.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/fakeLights_JuliaBulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/foldbox "
		"sphereFoldParabolic ede6.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/ifs_gen menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/ifs_xy.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/jos_kleinian_v4 sa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/jos_kleinian_v4 "
		"sphInv.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/jos_kleinian_v4 "
		"sphInv_2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/low_res_mode_menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandalay_boxv1 meng3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandalay_boxv1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandalay_boxv2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandalay_kifs.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandalay_tower.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mandelbarV2_sphereGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbarV3 ABa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbarV3 BDA1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbarV3 DBA1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbox_menger_inv "
		"001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbox_rot 001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulbIqPow8.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulbIqV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mandelbulbSinCosV4_addSphInv.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_SinCos_V4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_abs.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mandelbulb_boxFoldBulb_hybrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_boxTilingV3 "
		"aa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_eye 001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_kali_p8.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_plusZ.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_pow2V3 "
		"tglad.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_pupuku "
		"pow6.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mandelbulb_sin_cos_v2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_sin_cos_v3 "
		"hairy.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mandelbulb_sin_cos_v3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelbulb_tails.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnest.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnest4D "
		"IQbulbV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnest4D "
		"boxframe.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnestFull_boxTilingV3 "
		"aa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnestV2_boxTilingV3 "
		"bb1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnest_full.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mandelnest_full_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbCpixelInvert.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbPow2_v1-001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbPow2_v1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbPow2_v2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbRiemannV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulbSinCos "
		"DifsSphGrd3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/mbulbTailsV2_TaddSphericalInvert;.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mbulb_noC_rotateFold.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger - DE linear "
		"cube.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger - poly fold.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger - pyramid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger - "
		"smoothChebyshev.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger - smoothMod1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger addConstCond.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger3 "
		"absAddConstV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/menger3 dotFold.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/menger3_difsSupershape.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mengerV6.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mengerV7 aab1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/mengerV7 dab6.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/msltoe_ToroidalBulb_V2-001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/msltoe_ToroidalBulb_V2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym3_mod4 aa.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym3_mod4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym3_mod5 aa.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym3_mod5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym3_mod6 "
		"mbulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/msltoe_sym4_mod1_sphGrd "
		"va.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/msltoe_sym4_mod1_sphGrd.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/newtonPow3_mbulbTails.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pk3Abx15.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/platonicSolid_menger_hybrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/poly fold atan mbulb "
		"kali.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/polyfold kochIFS.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/polyfold polyhedra "
		"hexprism.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/polyfold sierpinski "
		"gear.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudo kleinian color "
		"tiling.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudo kleinian "
		"maxiter10.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/pseudoKleinianMod3_prismDIFS.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/pseudoKleinianMod3_torusDIFS.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod4 invert "
		"rec.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod4 "
		"invert.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod4 "
		"invertSp.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod4 "
		"rec.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod4.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod5 "
		"inv.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinianMod_4D.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/pseudoKleinian_std_DE_flat_surfaces.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinian_trig "
		"aa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudoKleinian_trig "
		"aa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/pseudo_kleinian_mod6 "
		"sphInv.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sinCosMax_sierp.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sinCos_abox11.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sinCos_pseudoKl.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sinCos_xkifs.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereCluster 2p55 "
		"sym.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereCluster 3p7.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereCluster clip.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV2 baa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV2 baa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV2 baa3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV3 aaa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV3 aaa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sphereClusterV3 aaa3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/sphere_invert_menger_sponge.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretreeV4 aaa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretreeV4 aaa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretreeV4 aaa3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretreeV5 aaa5.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretreeV5 fjb1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretree_1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/spheretree_2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/sym4_mod1_boxTilingV3 "
		"aa2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfAbsRecFoldXY - "
		"sierpinski.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfAddSphericalInvert "
		"menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfBoxTilingV3_mandelnest.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfBoxTilingV3_mengerPyramid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfDiagonalFold - "
		"asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfDiagonalFold - "
		"pseudoKleinian.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfMengerFoldV2 "
		"difsBox.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfMengerFoldV2 "
		"difsDonut.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfRotationChebychev - "
		"torusGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfSinCos_mengOcto.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transfSinOrCos- "
		"pseudoKleinianV1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfSinYm3d_menger3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfSinYm3d_menger3_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transfSphereInvV3_menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_Zvec_axis_swap.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_absAddConstantV2_mbulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_abs_add_multi.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_addCpixelTile_bulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_cayley2_v1 "
		"Torus.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_cayley2_v1 "
		"sphereGrid.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_difs_piriform.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_dotFold_boxFrame.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_juliaBox.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_juliaBoxV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_juliaBox_bxFld_scale_mgrM1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_mandalay_foldv1_asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_mandalay_foldv1_sphereClusterV2.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_mandalay_foldv2_asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_quadratic_fold "
		"menger4D.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_sin_add_asurf.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_sincosHelix.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_sincosHelix_juliaBulb.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_sincosHelix_menger3.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY "
		"4.0)/transf_sincosHelix_menger3_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_sincos_v2 "
		"Mbox_Menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/transf_sincos_v2 "
		"Menger.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/vicsek_001.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/xenodreambuie_v3 aa1.fract",
		"Graeme McLaren  collection - license Creative Commons  (CC-BY 4.0)/xenodreambuie_v3.fract",
		"IFS_001.fract",
		"IFS_002.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Alternative realities-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Cooling System-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Energy Fields-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Fields-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Futura-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Galaxyfactory-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Ifs  Gensamples-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Lost places1-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Magnetic Fields-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Matrix-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Mengerorbs-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Nebula1-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Origins-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Out of Eden-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Perfidious-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Pollen-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Relic-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Return-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Self replicant "
		"Transmitter-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Station34-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/TheGame-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Vitreus-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/Voyager-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/amrand-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/bajopuente-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/defiance-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/industrial thing-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/mandalaycity-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/mandelboxkoch-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/of their kind-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/outburst-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/outofreach-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/parkarea-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/perlinggrass-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/tubular bulbs-batjorge.fract",
		"Jorge Abalo - license Creative Commons (CC-BY-NC-SA 4.0)/upsidedown-batjorge.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/Bristorbrot.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/Construct by Ectoplaz "
		"2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/Construct by Ectoplaz "
		"3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/FoldIntPow2 2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/FoldIntPow2 3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY "
		"4.0)/GeneralizedFoldBox01.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY "
		"4.0)/GeneralizedFoldBox02.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY "
		"4.0)/GeneralizedFoldBox03_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/Hybrid 1.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 19 - maxiter.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 20.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 21.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 21_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 23.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 28.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 29_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 30_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 31.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 32.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/IFS 34.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/Riemann Sphere Msltoe "
		"003.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/aboxmod15.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/addcpixelinvert.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/aexion01.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/aexion02.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/benesi t1 pine tree.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/benesi03.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/blockify 2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/box fold bulb v4.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/bristorbrot01.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/bug.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/cayley2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/clouds 006.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/constant factor 2.0  - "
		"mandelbox scale 2.0.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/distance_fog.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/dune.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/equirectangular "
		"mandelbox.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/equirectangular menger "
		"sponge.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/fish eye.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/folded mender "
		"sponge.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hex prism and "
		"chromatic.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hexgrid.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hexgrid_v2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid animacja - "
		"background.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid14.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid15.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid16.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid17.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid18_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid19.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid22 - foldigIntPow v "
		"2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid23.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid33.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid35.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hybrid42.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/hypercomplex 03.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/interior - mandelbulb "
		"power 8.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iridescence.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 002_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 004.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 006.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 007.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 009.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 1.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/iter fog 2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/jos_kleinian 002.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/light beam.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/luminosity.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandalay.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox 22.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox10.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox11 - "
		"rotations.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox14 - "
		"interior.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox17.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox18.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox19.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox21.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox23 "
		"rotations.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox24.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox25_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox25_3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox26 - "
		"rotations.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox28.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox29.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox30.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox32 - "
		"spiral.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox34 - "
		"lights_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox37_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox38_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox41_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox44_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox48.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox49.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox50 - "
		"hearts.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox51.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox52_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox53.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox54_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox55.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox56.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox57.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox58.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbox8 - "
		"maxiter.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - iter "
		"fog.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - "
		"slice 3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - "
		"slice 4.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 2 - "
		"slice 5.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 3.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 8 - "
		"3_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 8 - "
		"4_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 8 - 7 - "
		"volmetric fog.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/mandelbulb power 8 - "
		"8.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY "
		"4.0)/mandelbulb_multi_001.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/menger sponge 001.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/menger sponge 002_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/menger sponge 004_2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/menger v7.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/modulus mandelbulb.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/monte carlo global "
		"illumination.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/orbitTraps 003.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/orbitTraps 004.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/orbitTraps 005.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/orbitTraps 006.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/orbitTraps 007.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/primitive objects - "
		"inverted box.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/primitive objects - "
		"water.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/pseudo kleinian 009 mc "
		"dof.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/repeat.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/t difs grid v2.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/two pseudo klienian.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/volumetricLight001.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/volumetricLight003.fract",
		"Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)/xenodreambuie2.fract",
		"Makin3D-Julia_001.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/Abox4D8K.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/Caves-PseudoKleinianMod2.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/Caves2-PseudoKleinianMod2.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/IFS31_anim.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/RoadToExascale.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/coastalbrot_bear.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/coastalbrot_smilin.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/continuum.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/hybrid77-stereo.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/hybrid77.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/mandelbox22_anim.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-BoxFold4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/menger-FabsAddConditional4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/menger-FabsAddTgladFold4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/menger-IterationWeight4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-Rotation4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-Scale4D.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY "
		"4.0)/menger-SphericalFold4Dtransform.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-coastn.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/menger-mod1_001_8k.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/mountains_and_valleys.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/msltoeDonut-coastn.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/neuron.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/pine-Bulb.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/sponged.fract",
		"Robert Pancoast collection - license Creative Commons (CC-BY 4.0)/toastn_stonemen_anim.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/aexion "
		"blue-sky-two-suns.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/hybrid 01 - mandelbox "
		"sponge with sphere.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/hybrid 02 - rectangle "
		"hieroglyphs animation.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/hybrid 02 - rectangle "
		"hieroglyphs.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/inverse-reciprocal-iqbulb "
		"mechanical-ribs.fract",
		"Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)/menger smooth mod 1 - "
		"iron man close up.fract",
		"aboxmod1_001.fract",
		"aboxvsicen1_001.fract",
		"aexion001.fract",
		"aexion002.fract",
		"aexion_octopus_001.fract",
		"amazing surf mod1 001.fract",
		"amazing surf multi 001.fract",
		"amazing_surf 002.fract",
		"amoxmodkali_001.fract",
		"benchmark-GPU.fract",
		"benchmark.fract",
		"benesi 001.fract",
		"benesi pwr2 mandelbulbs 001.fract",
		"benesi_mag_transforms_001.fract",
		"benesi_pine_tree_001.fract",
		"benesi_t1_pine_tree_001.fract",
		"boolean001.fract",
		"boolean002.fract",
		"box_fold_bulb_pow2_001.fract",
		"bristorbrot001.fract",
		"buffalo003_2.fract",
		"chromatic_aberration.fract",
		"clouds 001.fract",
		"clouds 002.fract",
		"clouds 003.fract",
		"clouds 004.fract",
		"clouds 005.fract",
		"difs_tree.fract",
		"fabs_box_mod1_001.fract",
		"flight_anim_menger sponge_3.fract",
		"gannjondal - newton- license Creative Commons (CC-BY "
		"4.0)/newtonPow3-PolyFoldSym-delta-gnj-016d.fract",
		"gannjondal - newton- license Creative Commons (CC-BY "
		"4.0)/newtonPow3-ScaleRot-delta-gnj-007d.fract",
		"gannjondal - newton- license Creative Commons (CC-BY 4.0)/newtonPow3-delta-gnj-001b.fract",
		"gannjondal - newton- license Creative Commons (CC-BY 4.0)/newtonPow3-delta-gnj-002h.fract",
		"gannjondal - newton- license Creative Commons (CC-BY "
		"4.0)/newtonPow3-rotfold-delta-gnj-003d.fract",
		"gannjondal - newton- license Creative Commons (CC-BY "
		"4.0)/newtonPow3-rotfold-delta-gnj-010g.fract",
		"gradients - reflectance and transparency.fract",
		"gradients - specular highlights.fract",
		"hex grid 001.fract",
		"hex grid 002.fract",
		"hybrid001.fract",
		"hybrid002.fract",
		"hybrid004.fract",
		"hybrid005.fract",
		"hybrid007.fract",
		"hybrid008 - collatz.fract",
		"hypercomplex001.fract",
		"ides1_001.fract",
		"ides2_001.fract",
		"iq_bulb_001.fract",
		"jos_kleinian 001.fract",
		"kaliset001.fract",
		"keyframe_anim_mandelbox_boxes.fract",
		"keyframe_anim_mandelbulb.fract",
		"light - projection.fract",
		"light types.fract",
		"mandelbox001.fract",
		"mandelbox002.fract",
		"mandelbox003.fract",
		"mandelbox_menger.fract",
		"mandelbox_menger_morph.fract",
		"mandelbulb001.fract",
		"mandelbulb002.fract",
		"mandelbulb2_001.fract",
		"mandelbulb3_001.fract",
		"mandelbulb4_001.fract",
		"mandelbulb_eye.fract",
		"mandelbulb_kali 001.fract",
		"mandelbulb_kali_multi 001.fract",
		"mandelbulb_vary_power_001.fract",
		"mandelnest pow 5.fract",
		"menger 4D 001.fract",
		"menger cross kifs 001.fract",
		"menger cross mod1 001.fract",
		"menger middle mod.fract",
		"menger octo 001.fract",
		"menger prism shape2 001.fract",
		"menger pwr2 polynomial 001.fract",
		"menger smooth 001.fract",
		"menger smooth mod1.fract",
		"menger sponge_with_orbit_trap_lights.fract",
		"menger-mod1_001.fract",
		"monte carlo DOF 001.fract",
		"monte carlo DOF 002.fract",
		"monte carlo DOF 003.fract",
		"monte carlo DOF 004.fract",
		"monte carlo global illumination - emmisive materials.fract",
		"monte carlo global illumination - luminosity gradient.fract",
		"monte carlo global illumination by volumetric effects.fract",
		"monte carlo global illumination.fract",
		"msltoe toroidal bulb 001.fract",
		"msltoe_donut_001.fract",
		"msltoe_julia_bulb_eiffie_001.fract",
		"msltoe_julia_bulb_mod2_001.fract",
		"msltoe_sym4_mod 001.fract",
		"msltoesym2_mod_001.fract",
		"msltoesym2_mod_002.fract",
		"nebula 001.fract",
		"nebula 002.fract",
		"nebula 003.fract",
		"nebula 004.fract",
		"octahedron with box foldig_2.fract",
		"orbittrap001.fract",
		"orbittrap002.fract",
		"orbittrap003.fract",
		"orbittrap004.fract",
		"orbittrap005.fract",
		"primitives001.fract",
		"primitives002.fract",
		"pseudo kleinian 001.fract",
		"pseudo kleinian 002.fract",
		"pseudo kleinian mod 001.fract",
		"pseudo kleinian std de 001.fract",
		"pseudo kleinian std de 002.fract",
		"quaternion_001.fract",
		"quaternion_3DE_001.fract",
		"quick-dudley_001.fract",
		"quick-dudley_mod_001.fract",
		"riemann bulb msltoe mod2 001.fract",
		"riemann sphere 001.fract",
		"riemann_sphere_msltoe_001.fract",
		"riemann_sphere_msltoe_v1_001.fract",
		"sierpinski 3D 002.fract",
		"smooth_mandelbox_001.fract",
		"stereoscopic 001.fract",
		"stereoscopic 002.fract",
		"stereoscopic 003.fract",
		"stereoscopic 004.fract",
		"subsurface scattering 002.fract",
		"the grid 002.fract",
		"transparency reflectance roughness textures.fract",
		"transparent002.fract",
		"voxel_export_with_Bristorbrot4D.fract",
	};
}
