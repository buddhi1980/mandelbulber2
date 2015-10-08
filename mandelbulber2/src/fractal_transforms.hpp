/*
 * fractal_transforms.hpp
 *
 *  Created on: 30 wrz 2015
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_TRANSFORMS_HPP_
#define MANDELBULBER2_SRC_FRACTAL_TRANSFORMS_HPP_

#include "four_fractals.hpp"
#include "commonparams.hpp"

void additionConstantTransform3D(const sTransformAdditionConstant &additionConstant, CVector3 &z, int i);
void additionConstantTransform4D(const sTransformAdditionConstant &additionConstant, CVector4 &z, int i);
void boxConstantMultiplierTransform3D(const sTransformBoxConstantMultiplier &boxConstantMultiplier, CVector3 &z,CVector3 &c, int i);
void boxFoldOriginalTransform3D(const sTransformBoxFoldOriginal &boxFoldOriginal, CVector3 &z, int i, sExtendedAux &aux);
void boxFoldTransform3D(const sTransformBoxFold &boxFold, CVector3 &z, int i, sExtendedAux &aux);
void boxFoldTransform4D(const sTransformBoxFold &boxFold, CVector4 &z, int i, sExtendedAux &aux);
void boxOffsetTransform3D(const sTransformBoxOffset &boxOffset, CVector3 &z, int i, sExtendedAux &aux);
void constantMultiplierOriginalTransform3D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector3 &z, CVector3 &c, int i);
void constantMultiplierOriginalTransform4D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector4 &z, const CVector4 &c, int i);
void constantMultiplierTransform3D(const sTransformConstantMultiplier &constantMultiplier, CVector3 &z, CVector3 &c, int i);
void constantMultiplierTransform4D(const sTransformConstantMultiplier &constantMultiplier, CVector4 &z, const CVector4 &c, int i);
void fabsAddConstantTransform3D(const sTransformFabsAddConstant &fabsAddConstant, CVector3 &z, int i);
void fabsAddConstantTransform4D(const sTransformFabsAddConstant &fabsAddConstant, CVector4 &z, int i);
void fabsFormulaABTransform3D(const sTransformFabsFormulaAB &fabsFormulaAB, CVector3 &z, int i);
void fabsFormulaABCDTransform3D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector3 &z, int i);
void fabsFormulaABCDTransform4D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector4 &z, int i);
void fabsFormulaZABTransform3D(const sTransformFabsFormulaZAB &fabsFormulaZAB, CVector3 &z, int i);
void fabsSubConstantTransform3D(const sTransformFabsSubConstant &fabsSubConstant, CVector3 &z, int i);
void iterationWeightTransform3D(const sTransformIterationWeight &iterationWeight, CVector3 &z, int i);
void iterationWeightTransform4D(const sTransformIterationWeight &iterationWeight, CVector4 &z, int i);
void mandelbulbTransform3D(const sTransformMandelbulb &mandelbulb, CVector3 &z, int i, sExtendedAux &aux);
void mandelbulbOriginalTransform3D(const sTransformMandelbulbOriginal &mandelbulbOriginal, CVector3 &z, int i, sExtendedAux &aux);
void mainRotationTransform3D(const sTransformMainRotation &mainRotation, CVector3 &z, int i);
void mainRotationTransform4D(const sTransformMainRotation &mainRotation, CVector4 &z, int i);
void mengerSpongeOriginalTransform3D(const sTransformMengerSpongeOriginal &mengerSpongeOriginal, CVector3 &z, int i, sExtendedAux &aux);
void mengerSpongeTransform3D(const sTransformMengerSponge &mengerSponge, CVector3 &z, int i, sExtendedAux &aux);
void quaternionOriginalTransform4D(const sTransformQuaternionOriginal &quaternionOriginal, CVector4 &z, int i);
void quaternionTransform4D(const sTransformQuaternion &quaternion, CVector4 &z, int i);
void scaleOriginalTransform3D(const sTransformScaleOriginal &scaleOriginal, CVector3 &z, int i, sExtendedAux &aux);
void scaleTransform3D(const sTransformScale &scale, CVector3 &z, int i, sExtendedAux &aux);
void scaleTransform4D(const sTransformScale &scale, CVector4 &z, int i, sExtendedAux &aux);
void sphericalFoldOriginalTransform3D(const sTransformSphericalFoldOriginal &sphericalFoldOriginal, CVector3 &z, int i, sExtendedAux &aux);
void sphericalFoldTransform3D(const sTransformSphericalFold &sphericalFold, CVector3 &z, int i, sExtendedAux &aux);
void sphericalFoldTransform4D(const sTransformSphericalFold &sphericalFold, CVector4 &z, int i, sExtendedAux &aux);
void sphericalOffsetTransform3D(const sTransformSphericalOffset &sphericalOffset, CVector3 &z, int i, sExtendedAux &aux);
void variableConstantMultiplierTransform3D(const sTransformVariableConstantMultiplier &variableConstantMultiplier, CVector3 &z, CVector3 &c, int i);
void variableMandelbulbPowerTransform3D(const sTransformVariableMandelbulbPower &variableMandelbulbPower, CVector3 &z, int i, sExtendedAux &aux);
void variableScaleTransform3D(const sTransformVariableScale &variableScale, CVector3 &z, int i, sExtendedAux &aux);
#endif /* MANDELBULBER2_SRC_FRACTAL_TRANSFORMS_HPP_ */
