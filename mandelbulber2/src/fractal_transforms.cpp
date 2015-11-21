/*
 * fractal_transforms.cpp
 *
 *  Created on: 30 wrz 2015
 *      Author: krzysztof
 */

#include "fractal_transforms.hpp"
#include "common_math.h"

#define SQRT_1_3 0.57735026918962576450914878050196
#define SQRT_1_2 0.70710678118654752440084436210485
#define SQRT_2_3 0.81649658092772603273242802490196
#define SQRT_3_2 1.22474487139158904909864203735295


// TRANSFORMS

//additionConstant transform 3D
void additionConstantTransform3D(const sTransformAdditionConstant &additionConstant, CVector3 &z, int i)
{
	if (additionConstant.control.enabled && i >= additionConstant.control.startIterations && i < additionConstant.control.stopIterations)
	{
		CVector3 temp = z;
		z += additionConstant.additionConstant;
		//weight function
		if (additionConstant.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, additionConstant.control.weight);
		}
	}
}

//additionConstant transform 4D
void additionConstantTransform4D(const sTransformAdditionConstant &additionConstant, CVector4 &z, int i)
{
	if (additionConstant.control.enabled && i >= additionConstant.control.startIterations && i < additionConstant.control.stopIterations)
	{
		CVector4 temp = z;
		z += additionConstant.additionConstant4D;
		//weight function
		if (additionConstant.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, additionConstant.control.weight);
		}
	}
}

//boxConstantMultiplier transform 3D
void boxConstantMultiplierTransform3D(const sTransformBoxConstantMultiplier &boxConstantMultiplier, CVector3 &z, CVector3 &c, int i)
{
	if (boxConstantMultiplier.control.enabled && i >= boxConstantMultiplier.control.startIterations && i < boxConstantMultiplier.control.stopIterations)
	{
		CVector3 temp = z;
		CVector3 tempFAB = c;
		if (boxConstantMultiplier.boxConstantMultiplierFabsEnabledX)
		{
			tempFAB.x = fabs(tempFAB.x);
		}
		if (boxConstantMultiplier.boxConstantMultiplierFabsEnabledY)
		{
			tempFAB.y = fabs(tempFAB.y);
		}
		if (boxConstantMultiplier.boxConstantMultiplierFabsEnabledZ)
		{
			tempFAB.z = fabs(tempFAB.z);
		}
		tempFAB *= boxConstantMultiplier.boxConstantMultiplier;
		if (z.x > 0) z.x += tempFAB.x;
		else z.x -= tempFAB.x;
		if (z.y > 0) z.y += tempFAB.y;
		else z.y -= tempFAB.y;
		if (z.z > 0) z.z += tempFAB.z;
		else z.z -= tempFAB.z;
		//weight function
		if (boxConstantMultiplier.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, boxConstantMultiplier.control.weight);
		}
	}
}

//boxFoldOriginal transform 3D
void boxFoldOriginalTransform3D(const sTransformBoxFoldOriginal &boxFoldOriginal, CVector3 &z, int i, sExtendedAux &aux)
{
	if (boxFoldOriginal.control.enabled && i >= boxFoldOriginal.control.startIterations && i < boxFoldOriginal.control.stopIterations)
	{
		CVector3 temp = z;
		if (z.x > boxFoldOriginal.foldingLimit)
		{
			z.x = boxFoldOriginal.foldingValue - z.x;
			aux.color += boxFoldOriginal.color.factor.x;
		}
		else if (z.x < -boxFoldOriginal.foldingLimit)
		{
			z.x = -boxFoldOriginal.foldingValue - z.x;
			aux.color += boxFoldOriginal.color.factor.x;
		}
		if (z.y > boxFoldOriginal.foldingLimit)
		{
			z.y = boxFoldOriginal.foldingValue - z.y;
			aux.color += boxFoldOriginal.color.factor.y;
		}
		else if (z.y < -boxFoldOriginal.foldingLimit)
		{
			z.y = -boxFoldOriginal.foldingValue - z.y;
			aux.color += boxFoldOriginal.color.factor.y;
		}
		if (z.z > boxFoldOriginal.foldingLimit)
		{
			z.z = boxFoldOriginal.foldingValue - z.z;
			aux.color += boxFoldOriginal.color.factor.z;
		}
		else if (z.z < -boxFoldOriginal.foldingLimit)
		{
			z.z = -boxFoldOriginal.foldingValue - z.z;
			aux.color += boxFoldOriginal.color.factor.z;
		}
		//oldOriginal weight function
		if (boxFoldOriginal.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (boxFoldOriginal.control.oldWeight));
		}
		//weight function
		if (boxFoldOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, boxFoldOriginal.control.weight);
		}
	}
}

//boxFold transform 3D
void boxFoldTransform3D(const sTransformBoxFold &boxFold, CVector3 &z, int i, sExtendedAux &aux)
{
	if (boxFold.control.enabled && i >= boxFold.control.startIterations && i < boxFold.control.stopIterations)
	{
		CVector3 temp = z;
		if (z.x > boxFold.foldingLimit)
		{
			z.x = boxFold.foldingValue - z.x;
			aux.color += boxFold.color.factor.x;
		}
		else if (z.x < -boxFold.foldingLimit)
		{
			z.x = -boxFold.foldingValue - z.x;
			aux.color += boxFold.color.factor.x;
		}
		if (z.y > boxFold.foldingLimit)
		{
			z.y = boxFold.foldingValue - z.y;
			aux.color += boxFold.color.factor.y;
		}
		else if (z.y < -boxFold.foldingLimit)
		{
			z.y = -boxFold.foldingValue - z.y;
			aux.color += boxFold.color.factor.y;
		}
		if (z.z > boxFold.foldingLimit)
		{
			z.z = boxFold.foldingValue - z.z;
			aux.color += boxFold.color.factor.z;
		}
		else if (z.z < -boxFold.foldingLimit)
		{
			z.z = -boxFold.foldingValue - z.z;
			aux.color += boxFold.color.factor.z;
		}
		//old weight function
		if (boxFold.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (boxFold.control.oldWeight));
		}
		//weight function
		if (boxFold.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, boxFold.control.weight);
		}
	}
}

//boxFold transform 4D
void boxFoldTransform4D(const sTransformBoxFold &boxFold, CVector4 &z, int i, sExtendedAux &aux)
{
	if (boxFold.control.enabled && i >= boxFold.control.startIterations && i < boxFold.control.stopIterations)
	{
		CVector4 temp = z;
		if (z.x > boxFold.foldingLimit)
		{
			z.x = boxFold.foldingValue - z.x;
			aux.color += boxFold.color.factor.x;
		}
		else if (z.x < -boxFold.foldingLimit)
		{
			z.x = -boxFold.foldingValue - z.x;
			aux.color += boxFold.color.factor.x;
		}
		if (z.y > boxFold.foldingLimit)
		{
			z.y = boxFold.foldingValue - z.y;
			aux.color += boxFold.color.factor.y;
		}
		else if (z.y < -boxFold.foldingLimit)
		{
			z.y = -boxFold.foldingValue - z.y;
			aux.color += boxFold.color.factor.y;
		}
		if (z.z > boxFold.foldingLimit)
		{
			z.z = boxFold.foldingValue - z.z;
			aux.color += boxFold.color.factor.z;
		}
		else if (z.z < -boxFold.foldingLimit)
		{
			z.z = -boxFold.foldingValue - z.z;
			aux.color += boxFold.color.factor.z;
		}
		if (z.w > boxFold.foldingLimit) z.w = boxFold.foldingValue - z.w;
		else if (z.w < -boxFold.foldingLimit) z.w = -boxFold.foldingValue - z.w;
		//old weight function
		if (boxFold.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (boxFold.control.oldWeight));
		}
		//weight function
		if (boxFold.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, boxFold.control.weight);
		}
	}
}

//boxOffset transform 3D
void boxOffsetTransform3D(const sTransformBoxOffset &boxOffset, CVector3 &z, int i, sExtendedAux &aux)
{
	if (boxOffset.control.enabled && i >= boxOffset.control.startIterations && i < boxOffset.control.stopIterations)
	{
		CVector3 temp = z;
		if (z.x > 0)
		{
			z.x = z.x + boxOffset.boxOffset.x;
			aux.color += boxOffset.color.factor.x;
		}
		else
		{
			z.x = z.x - boxOffset.boxOffset.x;
			aux.color += boxOffset.color.factor.x;
		}
		if (z.y > 0)
		{
			z.y = z.y + boxOffset.boxOffset.y;
			aux.color += boxOffset.color.factor.y;
		}
		else
		{
			z.y = z.y - boxOffset.boxOffset.y;
			aux.color += boxOffset.color.factor.y;
		}
		if (z.z > 0)
		{
			z.z = z.z + boxOffset.boxOffset.z;
			aux.color += boxOffset.color.factor.z;
		}
		else
		{
			z.z = z.z - boxOffset.boxOffset.z;
			aux.color += boxOffset.color.factor.z;
		}
		//weight function
		if (boxOffset.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, boxOffset.control.weight);
		}
	}
}

//constantMultiplierOriginal transform 3D
void constantMultiplierOriginalTransform3D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector3 &z, CVector3 &c, int i)
{
	if (constantMultiplierOriginal.control.enabled && i >= constantMultiplierOriginal.control.startIterations && i < constantMultiplierOriginal.control.stopIterations)
	{
		CVector3 temp = z;
		z += c * constantMultiplierOriginal.constantMultiplierVect;
		//weight function
		if (constantMultiplierOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, constantMultiplierOriginal.control.weight);
		}
	}
}

//constantMultiplierOriginal transform 4D
void constantMultiplierOriginalTransform4D(const sTransformConstantMultiplierOriginal &constantMultiplierOriginal, CVector4 &z, const CVector4 &c, int i)
{
	if (constantMultiplierOriginal.control.enabled && i >= constantMultiplierOriginal.control.startIterations && i < constantMultiplierOriginal.control.stopIterations)
	{
		CVector4 temp = z;
		z += c * constantMultiplierOriginal.constantMultiplierVect4D;
		//weight function
		if (constantMultiplierOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, constantMultiplierOriginal.control.weight);
		}
	}
}

//constantMultiplier transform 3D
void constantMultiplierTransform3D(const sTransformConstantMultiplier &constantMultiplier, CVector3 &z, CVector3 &c, int i)
{
	if (constantMultiplier.control.enabled && i >= constantMultiplier.control.startIterations && i < constantMultiplier.control.stopIterations)
	{
		CVector3 temp = z;
		z += c * constantMultiplier.constantMultiplierVect;
		//weight function
		if (constantMultiplier.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, constantMultiplier.control.weight);
		}
	}
}

//constantMultiplier transform 4D
void constantMultiplierTransform4D(const sTransformConstantMultiplier &constantMultiplier, CVector4 &z, const CVector4 &c, int i)
{
	if (constantMultiplier.control.enabled && i >= constantMultiplier.control.startIterations && i < constantMultiplier.control.stopIterations)
	{
		CVector4 temp = z;
		z += c * constantMultiplier.constantMultiplierVect4D;
		//weight function
		if (constantMultiplier.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, constantMultiplier.control.weight);
		}
	}
}

//fabsAddConstant  z = ( fabs( z + const.A )  *  const.B )   +   z  * const.C; 3D
void fabsAddConstantTransform3D(const sTransformFabsAddConstant &fabsAddConstant, CVector3 &z, int i)
{
	if (fabsAddConstant.control.enabled && i >= fabsAddConstant.control.startIterations && i < fabsAddConstant.control.stopIterations)
	{
		CVector3 temp = z;
		z += fabsAddConstant.fabsAddConstantA;
		if (fabsAddConstant.fabsAddConstantEnabledx)
		{
			z.x = fabs(z.x);
		}
		if (fabsAddConstant.fabsAddConstantEnabledy)
		{
			z.y = fabs(z.y);
		}
		if (fabsAddConstant.fabsAddConstantEnabledz)
		{
			z.z = fabs(z.z);
		}
		z = (z * fabsAddConstant.fabsAddConstantB) + (temp * fabsAddConstant.fabsAddConstantC);
		//old weight function
		if (fabsAddConstant.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (fabsAddConstant.control.oldWeight));
		}
		//weight function
		if (fabsAddConstant.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsAddConstant.control.weight);
		}
	}
}

//fabsAddConstant 4D z = ( fabs( z + const.A )  *  const.B )   +   z  * const.C; 4D
void fabsAddConstantTransform4D(const sTransformFabsAddConstant &fabsAddConstant, CVector4 &z, int i)
{
	if (fabsAddConstant.control.enabled && i >= fabsAddConstant.control.startIterations && i < fabsAddConstant.control.stopIterations)
	{
		CVector4 temp = z;
		z += fabsAddConstant.fabsAddConstant4DA;
		if (fabsAddConstant.fabsAddConstantEnabledx)
		{
			z.x = fabs(z.x);
		}
		if (fabsAddConstant.fabsAddConstantEnabledy)
		{
			z.y = fabs(z.y);
		}
		if (fabsAddConstant.fabsAddConstantEnabledz)
		{
			z.z = fabs(z.z);
		}
		if (fabsAddConstant.fabsAddConstantEnabledw)
		{
			z.w = fabs(z.w);
		}
		z = (z * fabsAddConstant.fabsAddConstant4DB) + (temp * fabsAddConstant.fabsAddConstant4DC);
		//old weight function
		if (fabsAddConstant.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (fabsAddConstant.control.oldWeight));
		}
		//weight function
		if (fabsAddConstant.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsAddConstant.control.weight);
		}
	}
}

//fabsFormulaAB 3D z = fabs( z + const.A ) -  fabs ( z - const.B )  -   z ; 3D
void fabsFormulaABTransform3D(const sTransformFabsFormulaAB &fabsFormulaAB, CVector3 &z, int i)
{
	if (fabsFormulaAB.control.enabled && i >= fabsFormulaAB.control.startIterations && i < fabsFormulaAB.control.stopIterations)
	{
		CVector3 temp = z;
		if (fabsFormulaAB.fabsFormulaABEnabledx)
		{
			z.x = fabs(z.x + fabsFormulaAB.fabsFormulaABConstantA.x) - fabs(z.x - fabsFormulaAB.fabsFormulaABConstantB.x) - z.x;
		}
		if (fabsFormulaAB.fabsFormulaABEnabledy)
		{
			z.y = fabs(z.y + fabsFormulaAB.fabsFormulaABConstantA.y) - fabs(z.y - fabsFormulaAB.fabsFormulaABConstantB.y) - z.y;
		}
		if (fabsFormulaAB.fabsFormulaABEnabledz)
		{
			z.z = fabs(z.z + fabsFormulaAB.fabsFormulaABConstantA.z) - fabs(z.z - fabsFormulaAB.fabsFormulaABConstantB.z) - z.z;
		}
		//old weight function
		if (fabsFormulaAB.control.oldWeightEnabled)
		{
			z = temp + (z - temp) * (fabsFormulaAB.control.oldWeight);
		}
		//weight function
		if (fabsFormulaAB.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsFormulaAB.control.weight);
		}
	}
}
//  fabsFormulaABCD   z = fabs(  z + const.A ) - fabs(  z - const.B )  + (  z * const.C  +  const.D );  3D
void fabsFormulaABCDTransform3D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector3 &z, int i)
{
	if (fabsFormulaABCD.control.enabled && i >= fabsFormulaABCD.control.startIterations && i < fabsFormulaABCD.control.stopIterations)
	{
		CVector3 temp = z;
		CVector3 tempA = z * 0;
		CVector3 tempB = z * 0;
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAx)
		{
			tempA.x = fabs(z.x + fabsFormulaABCD.fabsFormulaABCDA.x);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBx)
		{
			tempB.x = fabs(z.x - fabsFormulaABCD.fabsFormulaABCDB.x);
		}
		z.x = tempA.x - tempB.x + (z.x * fabsFormulaABCD.fabsFormulaABCDC.x + fabsFormulaABCD.fabsFormulaABCDD.x);
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAy)
		{
			tempA.y = fabs(z.y + fabsFormulaABCD.fabsFormulaABCDA.y);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBy)
		{
			tempB.y = fabs(z.y - fabsFormulaABCD.fabsFormulaABCDB.y);
		}
		z.y = tempA.y - tempB.y + (z.y * fabsFormulaABCD.fabsFormulaABCDC.y + fabsFormulaABCD.fabsFormulaABCDD.y);
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAz)
		{
			tempA.z = fabs(z.z + fabsFormulaABCD.fabsFormulaABCDA.z);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBz)
		{
			tempB.z = fabs(z.z - fabsFormulaABCD.fabsFormulaABCDB.z);
		}
		z.z = tempA.z - tempB.z + (z.z * fabsFormulaABCD.fabsFormulaABCDC.z + fabsFormulaABCD.fabsFormulaABCDD.z);

		if (fabsFormulaABCD.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (fabsFormulaABCD.control.oldWeight));
		}
		//weight function
		if (fabsFormulaABCD.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsFormulaABCD.control.weight);
		}
	}
}
// fabsFormulaABCD   z = fabs(  z + const.A ) - fabs(  z - const.B )  + (  z * const.C  +  const.D );   4D
void fabsFormulaABCDTransform4D(const sTransformFabsFormulaABCD &fabsFormulaABCD, CVector4 &z, int i)
{
	if (fabsFormulaABCD.control.enabled && i >= fabsFormulaABCD.control.startIterations && i < fabsFormulaABCD.control.stopIterations)
	{
		CVector4 temp = z;
		CVector4 tempA = z * 0;
		CVector4 tempB = z * 0;
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAx)
		{
			tempA.x = fabs(z.x + fabsFormulaABCD.fabsFormulaABCD4DA.x);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBx)
		{
			tempB.x = fabs(z.x - fabsFormulaABCD.fabsFormulaABCD4DB.x);
		}
		z.x = tempA.x - tempB.x + (z.x * fabsFormulaABCD.fabsFormulaABCD4DC.x + fabsFormulaABCD.fabsFormulaABCD4DD.x);
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAy)
		{
			tempA.y = fabs(z.y + fabsFormulaABCD.fabsFormulaABCD4DA.y);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBy)
		{
			tempB.y = fabs(z.y - fabsFormulaABCD.fabsFormulaABCD4DB.y);
		}
		z.y = tempA.y - tempB.y + (z.y * fabsFormulaABCD.fabsFormulaABCD4DC.y + fabsFormulaABCD.fabsFormulaABCD4DD.y);
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAz)
		{
			tempA.z = fabs(z.z + fabsFormulaABCD.fabsFormulaABCD4DA.z);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBz)
		{
			tempB.z = fabs(z.z - fabsFormulaABCD.fabsFormulaABCD4DB.z);
		}
		z.z = tempA.z - tempB.z + (z.z * fabsFormulaABCD.fabsFormulaABCD4DC.z + fabsFormulaABCD.fabsFormulaABCD4DD.z);
		if (fabsFormulaABCD.fabsFormulaABCDEnabledAw)
		{
			tempA.w = fabs(z.w + fabsFormulaABCD.fabsFormulaABCD4DA.w);
		}
		if (fabsFormulaABCD.fabsFormulaABCDEnabledBw)
		{
			tempB.w = fabs(z.w - fabsFormulaABCD.fabsFormulaABCD4DB.w);
		}
		z.z = tempA.w - tempB.z + (z.w * fabsFormulaABCD.fabsFormulaABCD4DC.w + fabsFormulaABCD.fabsFormulaABCD4DD.w);
		//Old weight
		if (fabsFormulaABCD.control.oldWeightEnabled)
		{
			z = temp + ((z - temp) * (fabsFormulaABCD.control.oldWeight));
		}
		//weight function
		if (fabsFormulaABCD.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsFormulaABCD.control.weight);
		}
	}
}

// fabsFormulaZAB  z =  fabs ( z + constA) + ( z * constB) + constC.;
void fabsFormulaZABTransform3D(const sTransformFabsFormulaZAB &fabsFormulaZAB, CVector3 &z, int i)
{
	if (fabsFormulaZAB.control.enabled && i >= fabsFormulaZAB.control.startIterations && i < fabsFormulaZAB.control.stopIterations)
	{
		CVector3 temp = z;
		z += fabsFormulaZAB.fabsFormulaZABConstantA;
		if (fabsFormulaZAB.fabsFormulaZABEnabledx)
		{
			z.x = fabs(z.x);
		}
		if (fabsFormulaZAB.fabsFormulaZABEnabledy)
		{
			z.y = fabs(z.y);
		}
		if (fabsFormulaZAB.fabsFormulaZABEnabledz)
		{
			z.z = fabs(z.z);
		}
		z += (temp * fabsFormulaZAB.fabsFormulaZABConstantB) + fabsFormulaZAB.fabsFormulaZABConstantC;

		//old weight function
		if (fabsFormulaZAB.control.oldWeightEnabled)
		{
			z = temp + (z - temp) * (fabsFormulaZAB.control.oldWeight);
		}
		//weight function
		if (fabsFormulaZAB.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsFormulaZAB.control.weight);
		}
	}
}

//fabsSubConstant 3D z = - ( fabs( z - const.A )  *  const.B ) 3D
void fabsSubConstantTransform3D(const sTransformFabsSubConstant &fabsSubConstant, CVector3 &z, int i)
{
	if (fabsSubConstant.control.enabled && i >= fabsSubConstant.control.startIterations && i < fabsSubConstant.control.stopIterations)
	{
		CVector3 temp = z;

		z -= fabsSubConstant.fabsSubConstantA;
		if (fabsSubConstant.fabsSubConstantEnabledx)
		{
			z.x = -fabs(z.x);
		}
		if (fabsSubConstant.fabsSubConstantEnabledy)
		{
			z.y = -fabs(z.y);
		}
		if (fabsSubConstant.fabsSubConstantEnabledz)
		{
			z.z = -fabs(z.z);
		}
		z *= fabsSubConstant.fabsSubConstantB;

		//old weight function
		if (fabsSubConstant.control.oldWeightEnabled)
		{
			z = temp + (z - temp) * (fabsSubConstant.control.oldWeight);
		}
		//weight function
		if (fabsSubConstant.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, fabsSubConstant.control.weight);
		}
	}
}

// iterationWeight 3D
void iterationWeightTransform3D(const sTransformIterationWeight &iterationWeight, CVector3 &z, int i)
{
	if (iterationWeight.control.enabled)
	{
		CVector3 temp = z;
		CVector3 zA = z * 0;
		CVector3 zB = z * 0;

		if (i == iterationWeight.iterationWeightIterationA)
		{
			zA = z;
		}
		if (i == (iterationWeight.iterationWeightIterationB))
		{
			zB = z;
		}
		z = (z * iterationWeight.iterationWeightConstantZ) + (zA * iterationWeight.iterationWeightConstantA) + (zB * iterationWeight.iterationWeightConstantB);
		//weight function
		if (iterationWeight.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, iterationWeight.control.weight);
		}
	}
}

// iterationWeight 4D
void iterationWeightTransform4D(const sTransformIterationWeight &iterationWeight, CVector4 &z, int i)
{
	if (iterationWeight.control.enabled)
	{
		CVector4 temp = z;
		CVector4 zA = z * 0;
		CVector4 zB = z * 0;

		if (i == iterationWeight.iterationWeightIterationA)
		{
			zA = z;
		}
		if (i == (iterationWeight.iterationWeightIterationB))
		{
			zB = z;
		}
		z = (z * iterationWeight.iterationWeightConstantZ) + (zA * iterationWeight.iterationWeightConstantA) + (zB * iterationWeight.iterationWeightConstantB);
		//weight function
		if (iterationWeight.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, iterationWeight.control.weight);
		}
	}
}

// mandelbulb Original 3D
void mandelbulbOriginalTransform3D(const sTransformMandelbulbOriginal &mandelbulbOriginal, CVector3 &z, int i, sExtendedAux &aux)
{
	if (mandelbulbOriginal.control.enabled && i >= mandelbulbOriginal.control.startIterations && i < mandelbulbOriginal.control.stopIterations)
	{
		CVector3 temp = z;
		aux.r = z.Length();
    if(aux.r < 1e-21) aux.r = 1e-21;
		double th0 = asin(z.z / aux.r) + mandelbulbOriginal.betaAngleOffset;
		double ph0 = atan2(z.y, z.x) + mandelbulbOriginal.alphaAngleOffset;
		double rp = pow(aux.r, mandelbulbOriginal.power - 1.0);
		double th = th0 * mandelbulbOriginal.power;
		double ph = ph0 * mandelbulbOriginal.power;
		double cth = cos(th);
		aux.r_dz = rp * aux.r_dz * mandelbulbOriginal.power + 1.0;
		rp *= aux.r;
		z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
		//weight function
		if (mandelbulbOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mandelbulbOriginal.control.weight);
		}
	}
}

// mandelbulb 3D
void mandelbulbTransform3D(const sTransformMandelbulb &mandelbulb, CVector3 &z, int i, sExtendedAux &aux)
{
	if (mandelbulb.control.enabled && i >= mandelbulb.control.startIterations && i < mandelbulb.control.stopIterations)
	{
		CVector3 temp = z;
		aux.r = z.Length();
    if(aux.r < 1e-21) aux.r = 1e-21;
    double th0 = asin(z.z / aux.r) + mandelbulb.betaAngleOffset;
    double ph0 = atan2(z.y, z.x) + mandelbulb.alphaAngleOffset;
    double rp = pow(aux.r, mandelbulb.power - 1.0);
    double th = th0 * mandelbulb.power;
    double ph = ph0 * mandelbulb.power;
		double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * mandelbulb.power + 1.0;
		rp *= aux.r;
		z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;

		//weight function
		if (mandelbulb.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mandelbulb.control.weight);
		}
	}
}

//mainRotation 3D
void mainRotationTransform3D(const sTransformMainRotation &mainRotation, CVector3 &z, int i)
{
	if (mainRotation.control.enabled && i >= mainRotation.control.startIterations && i < mainRotation.control.stopIterations)
	{
		CVector3 temp = z;
		z = mainRotation.mainRot.RotateVector(z);
		//weight function
		if (mainRotation.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mainRotation.control.weight);
		}
	}
}

//mainRotation 4D
void mainRotationTransform4D(const sTransformMainRotation &mainRotation, CVector4 &z, int i)
{
	if (mainRotation.control.enabled && i >= mainRotation.control.startIterations && i < mainRotation.control.stopIterations)
	{
		CVector4 temp = z;
		CVector3 zRot = CVector3(z.x, z.y, z.z);
		zRot = mainRotation.mainRot.RotateVector(zRot);
		z = CVector4(zRot.x, zRot.y, zRot.z, z.w);
		//weight function
		if (mainRotation.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mainRotation.control.weight);
		}
	}
}

void mengerSpongeOriginalTransform3D(const sTransformMengerSpongeOriginal &mengerSpongeOriginal, CVector3 &z, int i, sExtendedAux &aux)
{
	if (mengerSpongeOriginal.control.enabled && i >= mengerSpongeOriginal.control.startIterations && i < mengerSpongeOriginal.control.stopIterations)
	{
		//double tempAuxDE = aux.ifsDE;
		CVector3 temp = z;
		double tempMS;
    z = fabs(z);
		if (z.x - z.y < 0)
		{
			tempMS = z.y;
			z.y = z.x;
			z.x = tempMS;
		}
		if (z.x - z.z < 0)
		{
			tempMS = z.z;
			z.z = z.x;
			z.x = tempMS;
		}
		if (z.y - z.z < 0)
		{
			tempMS = z.z;
			z.z = z.y;
			z.y = tempMS;
		}
		z *= 3.0 * mengerSpongeOriginal.mengerSpongeOriginalConstantZ;
		z.x -= 2.0 * mengerSpongeOriginal.mengerSpongeOriginalFactorConstantVect.x;
		z.y -= 2.0 * mengerSpongeOriginal.mengerSpongeOriginalFactorConstantVect.y;
		if (z.z > 1.0) z.z -= 2.0 * mengerSpongeOriginal.mengerSpongeOriginalFactorConstantVect.z;
		aux.DE *= 3.0 * mengerSpongeOriginal.mengerSpongeOriginalConstantZ;

		//weight function
		if (mengerSpongeOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mengerSpongeOriginal.control.weight);
			//double nkaux = 1.0 - ( mengerSpongeOriginal.control.weight);
			//aux.ifsDE = ( tempAuxDE * nkaux )  + ( aux.ifsDE  *  mengerSpongeOriginal.control.weight);
		}
	}
}

void mengerSpongeTransform3D(const sTransformMengerSponge &mengerSponge, CVector3 &z, int i, sExtendedAux &aux)
{
	if (mengerSponge.control.enabled && i >= mengerSponge.control.startIterations && i < mengerSponge.control.stopIterations)
	{
		//double tempAuxDE = aux.ifsDE;
		CVector3 temp = z;
		double tempMS;
    z = fabs(z);
		if (z.x - z.y < 0)
		{
			tempMS = z.y;
			z.y = z.x;
			z.x = tempMS;
		}
		if (z.x - z.z < 0)
		{
			tempMS = z.z;
			z.z = z.x;
			z.x = tempMS;
		}
		if (z.y - z.z < 0)
		{
			tempMS = z.z;
			z.z = z.y;
			z.y = tempMS;
		}
		z *= 3.0 * mengerSponge.mengerSpongeConstantZ;
		z.x -= 2.0 * mengerSponge.mengerSpongeFactorConstantVect.x;
		z.y -= 2.0 * mengerSponge.mengerSpongeFactorConstantVect.y;
		if (z.z > 1.0) z.z -= 2.0 * mengerSponge.mengerSpongeFactorConstantVect.z;
		aux.DE *= 3.0 * mengerSponge.mengerSpongeConstantZ;

		//weight function
		if (mengerSponge.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, mengerSponge.control.weight);
			//double nkaux = 1.0 - ( mengerSponge.control.weight);
			//aux.ifsDE = ( tempAuxDE * nkaux )  + ( aux.ifsDE  *  mengerSponge.control.weight);
		}
	}
}

//quaternion transform 4D
void quaternionOriginalTransform4D(const sTransformQuaternionOriginal &quaternionOriginal, CVector4 &z, int i)
{
	if (quaternionOriginal.control.enabled && i >= quaternionOriginal.control.startIterations && i < quaternionOriginal.control.stopIterations)
	{
		CVector4 temp = z;
		CVector4 newz(quaternionOriginal.quaternionOriginalFactorConstant.x * (z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w),
				quaternionOriginal.quaternionOriginalFactorConstant.y * z.x * z.y, quaternionOriginal.quaternionOriginalFactorConstant.z * z.x * z.z,
				quaternionOriginal.quaternionOriginalFactorConstant.w * z.w);
		z = newz;
		//weight function
		if (quaternionOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, quaternionOriginal.control.weight);
		}
	}
}

//quaternion transform
void quaternionTransform4D(const sTransformQuaternion &quaternion, CVector4 &z, int i)
{
	if (quaternion.control.enabled && i >= quaternion.control.startIterations && i < quaternion.control.stopIterations)
	{
		CVector4 temp = z;
		CVector4 newz(quaternion.quaternionFactorConstant.x * (z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w), quaternion.quaternionFactorConstant.y * z.x * z.y,
				quaternion.quaternionFactorConstant.z * z.x * z.z, quaternion.quaternionFactorConstant.w * z.w);
		z = newz;
		//weight function
		if (quaternion.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, quaternion.control.weight);
		}
	}
}

//scaleOriginal transform 3D
void scaleOriginalTransform3D(const sTransformScaleOriginal &scaleOriginal, CVector3 &z, int i, sExtendedAux &aux)
{
	if (scaleOriginal.control.enabled && i >= scaleOriginal.control.startIterations && i < scaleOriginal.control.stopIterations)
	{
		z = z * scaleOriginal.scaleOriginal;
		aux.DE = aux.DE * fabs(scaleOriginal.scaleOriginal) + 1.0;
	}
}

//scale transform 3D
void scaleTransform3D(const sTransformScale &scale, CVector3 &z, int i, sExtendedAux &aux)
{
	if (scale.control.enabled && i >= scale.control.startIterations && i < scale.control.stopIterations)
	{
		z = z * scale.scale;
		aux.DE = aux.DE * fabs(scale.scale) + 1.0;
	}
}

//scale transform 4D
void scaleTransform4D(const sTransformScale &scale, CVector4 &z, int i, sExtendedAux &aux)
{
	if (scale.control.enabled && i >= scale.control.startIterations && i < scale.control.stopIterations)
	{
		z = z * scale.scale;
		aux.DE = aux.DE * fabs(scale.scale) + 1.0;
	}
}

//sphericalFoldOriginal transform 3D
void sphericalFoldOriginalTransform3D(const sTransformSphericalFoldOriginal &sphericalFoldOriginal, CVector3 &z, int i, sExtendedAux &aux)
{
	if (sphericalFoldOriginal.control.enabled && i >= sphericalFoldOriginal.control.startIterations && i < sphericalFoldOriginal.control.stopIterations)
	{
		CVector3 temp = z;
		double tempAuxDE = aux.DE;
    //double tempAuxColor = aux.color;
		double r2 = z.Dot(z);
		if (r2 < sphericalFoldOriginal.mR2)
		{
			z *= sphericalFoldOriginal.mboxFactor1;
			aux.DE *= sphericalFoldOriginal.mboxFactor1;
			aux.color += sphericalFoldOriginal.color.factorSp1;
		}
		else if (r2 < sphericalFoldOriginal.fR2)
		{
			double tglad_factor2 = sphericalFoldOriginal.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += sphericalFoldOriginal.color.factorSp2;
		}
		//aux weight function
		if (sphericalFoldOriginal.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, sphericalFoldOriginal.control.weight);
			double nkaux = 1.0 - (sphericalFoldOriginal.control.weight);
			aux.DE = (tempAuxDE * nkaux) + (aux.DE * sphericalFoldOriginal.control.weight);
			aux.color = (tempAuxDE * nkaux) + (aux.color * sphericalFoldOriginal.control.weight);
		}
	}
}

//sphericalFold transform 3D
void sphericalFoldTransform3D(const sTransformSphericalFold &sphericalFold, CVector3 &z, int i, sExtendedAux &aux)
{
	if (sphericalFold.control.enabled && i >= sphericalFold.control.startIterations && i < sphericalFold.control.stopIterations)
	{
		CVector3 temp = z;
		double tempAuxDE = aux.DE;
		double r2 = z.Dot(z);
		if (r2 < sphericalFold.mR2)
		{
			z *= sphericalFold.mboxFactor1;
			aux.DE *= sphericalFold.mboxFactor1;
			aux.color += sphericalFold.color.factorSp1;
		}
		else if (r2 < sphericalFold.fR2)
		{
			double tglad_factor2 = sphericalFold.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += sphericalFold.color.factorSp1;
		}
		//aux weight function
		if (sphericalFold.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, sphericalFold.control.weight);
			double nkaux = 1.0 - (sphericalFold.control.weight);
			aux.DE = (tempAuxDE * nkaux) + (aux.DE * sphericalFold.control.weight);
			aux.color = (tempAuxDE * nkaux) + (aux.color * sphericalFold.control.weight);
		}
	}
}

//sphericalFold transform 4D
void sphericalFoldTransform4D(const sTransformSphericalFold &sphericalFold, CVector4 &z, int i, sExtendedAux &aux)
{
	if (sphericalFold.control.enabled && i >= sphericalFold.control.startIterations && i < sphericalFold.control.stopIterations)
	{
		CVector4 temp = z;
		double tempAuxDE = aux.DE;
		double r2 = z.Dot(z);
		if (r2 < sphericalFold.mR2)
		{
			z *= sphericalFold.mboxFactor1;
			aux.DE *= sphericalFold.mboxFactor1;
			aux.color += sphericalFold.color.factorSp1;
		}
		else if (r2 < sphericalFold.fR2)
		{
			double tglad_factor2 = sphericalFold.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += sphericalFold.color.factorSp1;
		}
		//aux weight function
		if (sphericalFold.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, sphericalFold.control.weight);
			double nkaux = 1.0 - (sphericalFold.control.weight);
			aux.DE = (tempAuxDE * nkaux) + (aux.DE * sphericalFold.control.weight);
			aux.color = (tempAuxDE * nkaux) + (aux.color * sphericalFold.control.weight);
		}
	}
}

//sphericalOffset transform 3D
void sphericalOffsetTransform3D(const sTransformSphericalOffset &sphericalOffset, CVector3 &z, int i, sExtendedAux &aux)
{
	if (sphericalOffset.control.enabled && i >= sphericalOffset.control.startIterations && i < sphericalOffset.control.stopIterations)
	{
		CVector3 temp = z;
		double tempAuxDE = aux.DE;
		double lengthTempZ = -z.Length();
    if(lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1 + sphericalOffset.offsetRadius / lengthTempZ;
		z *= sphericalOffset.scale;
		aux.DE = aux.DE * fabs(sphericalOffset.scale) + 1.0;

		//weight function
		if (sphericalOffset.control.weightEnabled)
		{
			z = SmoothCVector(temp, z, sphericalOffset.control.weight);
			double nkaux = 1.0 - (sphericalOffset.control.weight);
			aux.DE = (tempAuxDE * nkaux) + (aux.DE * sphericalOffset.control.weight);
		}
	}
}

//variableConstantMultiplier transform 3D  z = z + ( c * (const + variableConst * (i - Start itr)/ (Stop Itr - Start Itr)))
void variableConstantMultiplierTransform3D(const sTransformVariableConstantMultiplier &variableConstantMultiplier, CVector3 &z, CVector3 &c, int i)
{
  if (variableConstantMultiplier.control.enabled && i >= variableConstantMultiplier.control.startIterations && i < variableConstantMultiplier.control.stopIterations)
  {
    CVector3 temp = z;
    CVector3 tempVC = variableConstantMultiplier.constantMultiplier;// constant to be varied
    if ( i < variableConstantMultiplier.variableStartIterations)
    {
    ;
    }
    if ( i >= variableConstantMultiplier.variableStartIterations && i < variableConstantMultiplier.variableStopIterations && (variableConstantMultiplier.variableStopIterations - variableConstantMultiplier.variableStartIterations != 0))
    {
      tempVC = (tempVC + variableConstantMultiplier.variableConstant * (i - variableConstantMultiplier.variableStartIterations) / (variableConstantMultiplier.variableStopIterations - variableConstantMultiplier.variableStartIterations));
    }
    if ( i >= variableConstantMultiplier.variableStopIterations)
    {
      tempVC =  (tempVC + variableConstantMultiplier.variableConstant);
    }

    z += c  * tempVC;
    //weight function
    if (variableConstantMultiplier.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, variableConstantMultiplier.control.weight);
    }
  }
}
// variableMandelbulb power 3D
void variableMandelbulbPowerTransform3D(const sTransformVariableMandelbulbPower &variableMandelbulbPower, CVector3 &z, int i, sExtendedAux &aux)
{
  if (variableMandelbulbPower.control.enabled && i >= variableMandelbulbPower.control.startIterations && i <variableMandelbulbPower.control.stopIterations)
  {
    CVector3 temp = z;
    double tempVC = variableMandelbulbPower.power;// constant to be varied
    if ( i < variableMandelbulbPower.variableStartIterations)
    {
    ;
    }
    if ( i >= variableMandelbulbPower.variableStartIterations && i < variableMandelbulbPower.variableStopIterations && (variableMandelbulbPower.variableStopIterations - variableMandelbulbPower.variableStartIterations != 0))
    {
      tempVC = (tempVC + variableMandelbulbPower.variableConstant * (i - variableMandelbulbPower.variableStartIterations) / (variableMandelbulbPower.variableStopIterations - variableMandelbulbPower.variableStartIterations));
    }
    if ( i >= variableMandelbulbPower.variableStopIterations)
    {
      tempVC =  (tempVC + variableMandelbulbPower.variableConstant);
    }

    aux.r = z.Length();
    if(aux.r < 1e-21) aux.r = 1e-21;
    double th0 = asin(z.z / aux.r) + variableMandelbulbPower.betaAngleOffset;
    double ph0 = atan2(z.y, z.x) + variableMandelbulbPower.alphaAngleOffset;
    double rp = pow(aux.r, tempVC - 1.0);
    double th = th0 * tempVC;
    double ph = ph0 * tempVC;
    double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * tempVC + 1.0;
    rp *= aux.r;
    z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
    //weight function
    if (variableMandelbulbPower.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, variableMandelbulbPower.control.weight);
    }
  }
}
//variableScale transform 3D
void variableScaleTransform3D(const sTransformVariableScale &variableScale, CVector3 &z, int i, sExtendedAux &aux)
{
  if (variableScale.control.enabled && i >= variableScale.control.startIterations && i < variableScale.control.stopIterations)
  {
    CVector3 tempVC = variableScale.scale;// constant to be varied
     if ( i < variableScale.variableStartIterations)
     {
     ;
     }
     if ( i >= variableScale.variableStartIterations && i < variableScale.variableStopIterations && (variableScale.variableStopIterations - variableScale.variableStartIterations != 0))
     {
       tempVC = (tempVC + variableScale.variableConstant * (i - variableScale.variableStartIterations) / (variableScale.variableStopIterations - variableScale.variableStartIterations));
     }
     if ( i >= variableScale.variableStopIterations)
     {
       tempVC =  (tempVC + variableScale.variableConstant);
     }

    z = z * tempVC;
    aux.DE = aux.DE * fabs((tempVC.x * tempVC.y * tempVC.z)/3) + 1.0; // cheap approximation
  }
}
//coloringParameters transform 3D
void coloringParametersTransform3D(const sTransformColoringParameters &coloringParameters, CVector3 &z, double minimumR, int i,  sExtendedAux &aux)
{
  if (coloringParameters.control.enabled && i >= coloringParameters.control.startIterations && i < coloringParameters.control.stopIterations)
    {
      double R;
      R = z.Length();
      if (R < minimumR) minimumR = R;
      R = (aux.color * 100) + (aux.r * coloringParameters.color.factorR * 100) + (1000* minimumR);
      aux.newR = R;
    }
}


//colorTrial transform 3D
//void colorTrialTransform3D(const sTransformColorTrial &colorTrial, CVector3 &z, CVector3 &sample0, CVector3 &sample1, CVector3 &sample2, CVector3 &sample3,  CVector3 &sample4, CVector3 &sample5, CVector3 &sample6, int i,  sExtendedAux &aux)

void colorTrialTransform3D(const sTransformColorTrial &colorTrial, CVector3 &z, int i,  sExtendedAux &aux)
{
  if (colorTrial.control.enabled && i >= colorTrial.control.startIterations && i < colorTrial.control.stopIterations)

  {
    double biasR;
    double orbitTrapsR;
    //double transformSamplingR;


// simple x, y, z bias
    if (colorTrial.axisBiasEnabled)
    {
      double biasR1 = fabs(z.x) * 10 * colorTrial.colorConstant1;
      double biasR2 = fabs(z.y) * 10 * colorTrial.colorConstant2;
      double biasR3 = fabs(z.z) * 10 * colorTrial.colorConstant3;
      biasR = ((biasR1 ) + ( biasR2 ) + (biasR3)) / 3.0;
      //biasR *= biasR; // trying squaring ??
      if (biasR < aux.axisBias ) aux.axisBias = biasR;
    }
    else
    {
      aux.axisBias = 0.0;
    }
// orbit traps
    if (colorTrial.orbitTrapsEnabled)
    {
      CVector3 tempOT1;
      CVector3 tempOT2;
      tempOT1 = z - colorTrial.orbitTrap1;
      tempOT2 = z - colorTrial.orbitTrap2;
      double lengthOT1 = tempOT1.Length() * colorTrial.orbitTrap1Weight; // * colorTrial.orbitTrap1Weight ;
      double lengthOT2 = tempOT2.Length() * colorTrial.orbitTrap2Weight; // * colorTrial.orbitTrap2Weight;
      if (lengthOT1 < lengthOT2) orbitTrapsR = lengthOT1;
      else orbitTrapsR = lengthOT2;
      if (orbitTrapsR < aux.orbitTraps ) aux.orbitTraps = orbitTrapsR;
    }
    else
    {
      aux.orbitTraps = 0.0;
    }
// transform sampling
   // if (colorTrial.transformSamplingEnabled )
   // {
   //   CVector3 diff1 = sample1 - sample0;
   //   CVector3 diff2 = sample2 - sample1;
   //   CVector3 diff3 = sample3 - sample2;
    //  CVector3 diff4 = sample4 - sample3;
   //   CVector3 diff5 = sample5 - sample4;
   //   CVector3 diff6 = sample6 - sample5;

   //   double newSR1 = diff1.Length() * colorTrial.sampleConstant1;;
   //   double newSR2 = diff2.Length() * colorTrial.sampleConstant2;;
   //   double newSR3 = diff3.Length() * colorTrial.sampleConstant3;
   //   double newSR4 = diff4.Length() * colorTrial.sampleConstant4;
   //   double newSR5 = diff5.Length() * colorTrial.sampleConstant5;
   //   double newSR6 = diff6.Length() * colorTrial.sampleConstant6;
   //   transformSamplingR = ( newSR1 + newSR2 + newSR3 + newSR4 + newSR5 + newSR6 );
   //   if (transformSamplingR < aux.transformSampling ) aux.transformSampling = transformSamplingR;
   // }
   // else
  //  {
  //    aux.transformSampling = 0.0;
  //  }
  //  double R = ((aux.axisBias + (aux.orbitTraps * colorTrial.mainOrbitTrapWeight) +  aux.transformSampling) * 1000* colorTrial.minimumRWeight) ;
    double R = (aux.axisBias + (aux.orbitTraps * colorTrial.mainOrbitTrapWeight)) * 1000* colorTrial.minimumRWeight ;

    aux.newR = R;

  }
}

// mandelbulbMulti 3D
void mandelbulbMultiTransform3D(const sTransformMandelbulbMulti &mandelbulbMulti, CVector3 &z, int i, sExtendedAux &aux)
{
  CVector3 temp = z;
  aux.r = z.Length();
  if (mandelbulbMulti.control.enabled && i >= mandelbulbMulti.control.startIterations && i < mandelbulbMulti.control.stopIterations)
  {
		if (i == mandelbulbMulti.control.startIterations)
    {
      z = mandelbulbMulti.mainRot.RotateVector(z);
    }
		double th0 = mandelbulbMulti.betaAngleOffset;
		double ph0 = mandelbulbMulti.alphaAngleOffset;
		double v1, v2, v3;

		switch(mandelbulbMulti.orderOfxyz){
			case sTransformMandelbulbMulti::xyz: v1 = z.x; v2 = z.y; v3 = z.z; break;
			case sTransformMandelbulbMulti::xzy: v1 = z.x; v2 = z.z; v3 = z.y; break;
			case sTransformMandelbulbMulti::yxz: v1 = z.y; v2 = z.x; v3 = z.z; break;
			case sTransformMandelbulbMulti::yzx: v1 = z.y; v2 = z.z; v3 = z.x; break;
			case sTransformMandelbulbMulti::zxy: v1 = z.z; v2 = z.x; v3 = z.y; break;
			case sTransformMandelbulbMulti::zyx: v1 = z.z; v2 = z.y; v3 = z.x; break;
		}
    if(aux.r < 1e-21) aux.r = 1e-21;
    if(v3 < 1e-21) v3 = 1e-21;
    if(mandelbulbMulti.acosOrasin == sTransformMandelbulbMulti::acos)
		{
			th0 += acos(v1 / aux.r);
		}
		else
		{
			th0 += asin(v1 / aux.r);
		}

		if(mandelbulbMulti.atanOratan2 == sTransformMandelbulbMulti::atan)
		{
			ph0 += atan(v2 / v3);
		}
		else
		{
			ph0 += atan2(v2, v3);
		}

    double rp = pow(aux.r, mandelbulbMulti.power - 1.0);
    double th = th0 * mandelbulbMulti.power;
    double ph = ph0 * mandelbulbMulti.power;
    double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * mandelbulbMulti.power + 1.0;
    rp *= aux.r;
    z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
    //weight function
    if (mandelbulbMulti.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, mandelbulbMulti.control.weight);
    }
  }
}

// for info about the following M.Benesi transforms and formulas  refer http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/
//benesiMagForwardTransform ONE 3D
void benesiMagForwardTransformOneTransform3D(const sTransformBenesiMagForwardTransformOne &benesiMagForwardTransformOne, CVector3 &z,int i)
{
  if (benesiMagForwardTransformOne.control.enabled && i >= benesiMagForwardTransformOne.control.startIterations && i < benesiMagForwardTransformOne.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

    //weight function
    if (benesiMagForwardTransformOne.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagForwardTransformOne.control.weight);
    }
  }
}
//benesiMagBackTransform ONE 3D
void benesiMagBackTransformOneTransform3D(const sTransformBenesiMagBackTransformOne &benesiMagBackTransformOne, CVector3 &z,int i)
{
  if (benesiMagBackTransformOne.control.enabled && i >= benesiMagBackTransformOne.control.startIterations && i < benesiMagBackTransformOne.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
      z.z * SQRT_2_3 - tempXZ  * SQRT_1_3);

    //weight function
    if (benesiMagBackTransformOne.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagBackTransformOne.control.weight);
    }
  }
}
//benesiMagTransform transform ONE 3D
void benesiMagTransformOneTransform3D(const sTransformBenesiMagTransformOne &benesiMagTransformOne, CVector3 &z, int i, sExtendedAux &aux)
{
  if (benesiMagTransformOne.control.enabled && i >= benesiMagTransformOne.control.startIterations && i < benesiMagTransformOne.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

//  Change this for different transforms
    z = fabs(z) * benesiMagTransformOne.scale;

    double avgScale = (fabs(benesiMagTransformOne.scale.x) + fabs(benesiMagTransformOne.scale.y) + fabs(benesiMagTransformOne.scale.z))/3;// cheap approximation
    aux.r_dz *= avgScale;
    aux.DE = aux.DE * avgScale + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
      z.z * SQRT_2_3 - tempXZ  * SQRT_1_3) ;

    z = z - benesiMagTransformOne.offset;

    //weight function
    if (benesiMagTransformOne.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformOne.control.weight);
    }
  }
}
//benesiMagTransform transform OneEnabled 3D
void benesiMagTransformOneEnabledTransform3D(const sTransformBenesiMagTransformOneEnabled &benesiMagTransformOneEnabled, CVector3 &z, int i, sExtendedAux &aux)
{
  if (benesiMagTransformOneEnabled.control.enabled && i >= benesiMagTransformOneEnabled.control.startIterations && i < benesiMagTransformOneEnabled.control.stopIterations)
  {

  CVector3 temp = z;
  double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
  z = CVector3
    ((tempXZ  - z.y) * SQRT_1_2,
    (tempXZ  + z.y) * SQRT_1_2,
  z.x * SQRT_1_3  +  z.z * SQRT_2_3);
  z = fabs(z) * benesiMagTransformOneEnabled.scale;
  double avgScale = (fabs(benesiMagTransformOneEnabled.scale.x) + fabs(benesiMagTransformOneEnabled.scale.y) + fabs(benesiMagTransformOneEnabled.scale.z))/3;// cheap approximation
  aux.r_dz *= avgScale;
  aux.DE = aux.DE * avgScale + 1.0;
  tempXZ  = (z.y + z.x) * SQRT_1_2;
  z = CVector3
    ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
    (z.y - z.x) * SQRT_1_2,
    z.z * SQRT_2_3 - tempXZ  * SQRT_1_3);
  z = z - benesiMagTransformOneEnabled.offset;


    //weight function
    if (benesiMagTransformOneEnabled.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformOneEnabled.control.weight);
    }
  }
}
//benesiMagTransformOnePlusMinus  3D
void benesiMagTransformOnePlusMinusTransform3D(const sTransformBenesiMagTransformOnePlusMinus &benesiMagTransformOnePlusMinus, CVector3 &z, int i, sExtendedAux &aux)
{
  if (benesiMagTransformOnePlusMinus.control.enabled && i >= benesiMagTransformOnePlusMinus.control.startIterations && i < benesiMagTransformOnePlusMinus.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

//  Change this for different transforms
    z *= benesiMagTransformOnePlusMinus.scale;
    z = (fabs( z + benesiMagTransformOnePlusMinus.offset) -  fabs( z - benesiMagTransformOnePlusMinus.offset )  - z) ;

    double avgScale = (fabs(benesiMagTransformOnePlusMinus.scale.x) + fabs(benesiMagTransformOnePlusMinus.scale.y) + fabs(benesiMagTransformOnePlusMinus.scale.z))/3;// cheap approximation
    aux.r_dz *= avgScale;
    aux.DE = aux.DE * fabs(avgScale) + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
      z.z * SQRT_2_3 - tempXZ  * SQRT_1_3) ;

    // weight function
    if (benesiMagTransformOnePlusMinus.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformOnePlusMinus.control.weight);
    }
  }
}
//benesiMagTransformTwo 3D
void benesiMagTransformTwoTransform3D(const sTransformBenesiMagTransformTwo &benesiMagTransformTwo, CVector3 &z, int i, sExtendedAux &aux)
{
  if (benesiMagTransformTwo.control.enabled && i >= benesiMagTransformTwo.control.startIterations && i < benesiMagTransformTwo.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

//  Change this for different transforms
    CVector3 tempV2 = z;
    tempV2.x = sqrt(z.y * z.y + z.z * z.z);
    tempV2.y = sqrt(z.x * z.x + z.z * z.z);  // switching, squared, sqrt
    tempV2.z = sqrt(z.x * z.x + z.y * z.y);
    z =  fabs(tempV2 - benesiMagTransformTwo.offset) * benesiMagTransformTwo.scale;

    double avgScale = (fabs(benesiMagTransformTwo.scale.x) + fabs(benesiMagTransformTwo.scale.y) + fabs(benesiMagTransformTwo.scale.z))/3;// cheap approximation
    //aux.r_dz *= avgScale;
    aux.DE = aux.DE * fabs(avgScale) + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
      z.z * SQRT_2_3 - tempXZ  * SQRT_1_3) ;

    //weight function
    if (benesiMagTransformTwo.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformTwo.control.weight);
    }
  }
}

//benesiMagTransformThree 3D
void benesiMagTransformThreeTransform3D(const sTransformBenesiMagTransformThree &benesiMagTransformThree, CVector3 &z, int i)
{
  if (benesiMagTransformThree.control.enabled && i >= benesiMagTransformThree.control.startIterations && i < benesiMagTransformThree.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

//  Change this for different transforms
    CVector3 tempV2 = z;
    tempV2.x = (z.y + z.z);
    tempV2.y = (z.x + z.z);   // switching
    tempV2.z = (z.x + z.y);
    z =  (fabs(tempV2 - benesiMagTransformThree.offset)) * benesiMagTransformThree.scale ;

    //double avgScale = fabs((benesiMagTransformThree.scale.x + benesiMagTransformThree.scale.y + benesiMagTransformThree.scale.z)/3);// cheap approximation
    //aux.r_dz *= avgScale;
    //aux.DE = aux.DE * fabs(avgScale) + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
       z.z * SQRT_2_3 - tempXZ  * SQRT_1_3);

    //weight function
    if (benesiMagTransformThree.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformThree.control.weight);
    }
  }
}
//benesiMagTransform transform Four 3D


void benesiMagTransformFourTransform3D(const sTransformBenesiMagTransformFour &benesiMagTransformFour, CVector3 &z, int i)
{
  if (benesiMagTransformFour.control.enabled && i >= benesiMagTransformFour.control.startIterations && i < benesiMagTransformFour.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);

  //  Change this for different transforms   This is T4:
    CVector3 tempV2 = z ;
    tempV2.x = (z.y * z.y + z.z * z.z );
    tempV2.y = (z.x * z.x + z.z * z.z );  // switching, squared,
    tempV2.z = (z.x * z.x + z.y * z.y );
    z = (fabs(tempV2 -  benesiMagTransformFour.offset)) * benesiMagTransformFour.scale;

    //double avgScale = (fabs(benesiMagTransformFour.scale.x) + fabs(benesiMagTransformFour.scale.y) + fabs(benesiMagTransformFour.scale.z))/3;// cheap approximation
    //aux.r_dz *= avgScale;
    //aux.DE = aux.DE * fabs(avgScale) + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
       z.z * SQRT_2_3 - tempXZ  * SQRT_1_3) ;

    //weight function
    if (benesiMagTransformFour.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformFour.control.weight);
    }
  }
}
//benesiMagTransformFiveB 3D
void benesiMagTransformFiveBTransform3D(const sTransformBenesiMagTransformFiveB &benesiMagTransformFiveB, CVector3 &z, int i)
{
  if (benesiMagTransformFiveB.control.enabled && i >= benesiMagTransformFiveB.control.startIterations && i < benesiMagTransformFiveB.control.stopIterations)
  {
    CVector3 temp = z;
    double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
    z = CVector3
      ((tempXZ  - z.y) * SQRT_1_2,
      (tempXZ  + z.y) * SQRT_1_2,
      z.x * SQRT_1_3  +  z.z * SQRT_2_3);
    if (z.x > -1e-21 && z.x < 1e-21) z.x = 1e-21;
    if (z.y > -1e-21 && z.y < 1e-21) z.y = 1e-21;
    if (z.z > -1e-21 && z.z < 1e-21) z.z = 1e-21;
    CVector3 tempV2 = z;
    tempV2.x = fabs(pow(pow(z.y, benesiMagTransformFiveB.powOne.x) + pow(z.z, benesiMagTransformFiveB.powOne.x),benesiMagTransformFiveB.powTwo.x));
    tempV2.y = fabs(pow(pow(z.x, benesiMagTransformFiveB.powOne.y) + pow(z.z, benesiMagTransformFiveB.powOne.y),benesiMagTransformFiveB.powTwo.y));
    tempV2.z = fabs(pow(pow(z.x, benesiMagTransformFiveB.powOne.z) + pow(z.y, benesiMagTransformFiveB.powOne.z),benesiMagTransformFiveB.powTwo.z));
    z =  (tempV2  - benesiMagTransformFiveB.offset)* benesiMagTransformFiveB.scale;

    //double avgScale = (fabs(benesiMagTransformFiveB.scale.x) + fabs(benesiMagTransformFiveB.scale.y) + fabs(benesiMagTransformFiveB.scale.z))/3;// cheap approximation
    //aux.r_dz *= avgScale;
    //aux.DE = aux.DE * fabs(avgScale) + 1.0;

    tempXZ  = (z.y + z.x) * SQRT_1_2;
    z = CVector3
      ( z.z * SQRT_1_3 + tempXZ  * SQRT_2_3,
      (z.y - z.x) * SQRT_1_2,
       z.z * SQRT_2_3 - tempXZ  * SQRT_1_3) ;

    //weight function
    if (benesiMagTransformFiveB.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiMagTransformFiveB.control.weight);
    }
  }
}

//benesiFastPwr2PineTree  3D
//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/

void benesiFastPwr2PineTreeTransform3D(const sTransformBenesiFastPwr2PineTree &benesiFastPwr2PineTree, CVector3 &z, CVector3 &c, int i, sExtendedAux &aux)
{
  if (benesiFastPwr2PineTree.control.enabled && i >= benesiFastPwr2PineTree.control.startIterations && i < benesiFastPwr2PineTree.control.stopIterations)
  {
    CVector3 temp = z;
    aux.r = z.Length();
    z *= z;

    double t = 2 * temp.x;
    if (z.y + z.z > 0.0) t = t/sqrt(z.y + z.z);
    else t = 1.0;
    //double t = 2 * temp.x/sqrt(z.y + z.z);
    c *= benesiFastPwr2PineTree.constantMultiplierVect;
    z.x = (z.x - z.y - z.z) + c.x ;
    z.z = (t * (z.y - z.z)) + c.y ;
    z.y = (2 * t * temp.y * temp.z) + c.z;
    aux.r_dz =aux.r * aux.r_dz * 2.0 + 1.0;
    //weight function
    if (benesiFastPwr2PineTree.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiFastPwr2PineTree.control.weight);
    }
  }
}
//benesiFastPwr2PineTreeEnabled  3D
void benesiFastPwr2PineTreeEnabledTransform3D(const sTransformBenesiFastPwr2PineTreeEnabled &benesiFastPwr2PineTreeEnabled, CVector3 &z, CVector3 &c, int i, sExtendedAux &aux)
{
  if (benesiFastPwr2PineTreeEnabled.control.enabled && i >= benesiFastPwr2PineTreeEnabled.control.startIterations && i < benesiFastPwr2PineTreeEnabled.control.stopIterations)
  {
    CVector3 temp = z;
    aux.r = z.Length();
    z *= z;
    double t = 2 * temp.x;
    if (z.y + z.z > 0.0) t = t/sqrt(z.y + z.z);
    else t = 1.0;
    //double t = 2 * temp.x/sqrt(z.y + z.z);
    c *= benesiFastPwr2PineTreeEnabled.constantMultiplierVect;
    z.x = (z.x - z.y - z.z) + c.x ;
    z.z = (t * (z.y - z.z)) + c.y ;
    z.y = (2 * t * temp.y * temp.z) + c.z;
    aux.r_dz =aux.r * aux.r_dz * 2.0 + 1.0;
    //weight function
    if (benesiFastPwr2PineTreeEnabled.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, benesiFastPwr2PineTreeEnabled.control.weight);
    }
  }
}
//boxFoldMultiMulti transform 3D
void boxFoldMultiTransform3D(const sTransformBoxFoldMulti &boxFoldMulti, CVector3 &z, int i, sExtendedAux &aux)
{
  if (boxFoldMulti.control.enabled && i >= boxFoldMulti.control.startIterations && i < boxFoldMulti.control.stopIterations)
  {
    CVector3 temp = z;
    if (z.x > boxFoldMulti.foldingLimit.x)
    {
            z.x = boxFoldMulti.foldingValue.x - z.x;
            aux.color += boxFoldMulti.color.factor.x;
    }
    else if (z.x < -boxFoldMulti.foldingLimit.x)
    {
            z.x = -boxFoldMulti.foldingValue.x - z.x;
            aux.color += boxFoldMulti.color.factor.x;
    }
    if (z.y > boxFoldMulti.foldingLimit.y)
    {
            z.y = boxFoldMulti.foldingValue.y - z.y;
            aux.color += boxFoldMulti.color.factor.y;
    }
    else if (z.y < -boxFoldMulti.foldingLimit.y)
    {
            z.y = -boxFoldMulti.foldingValue.y - z.y;
            aux.color += boxFoldMulti.color.factor.y;
    }
    if (z.z > boxFoldMulti.foldingLimit.z)
    {
            z.z = boxFoldMulti.foldingValue.z - z.z;
            aux.color += boxFoldMulti.color.factor.z;
    }
    else if (z.z < -boxFoldMulti.foldingLimit.z)
    {
            z.z = -boxFoldMulti.foldingValue.z - z.z;
            aux.color += boxFoldMulti.color.factor.z;
    }
    //weight function
    if (boxFoldMulti.control.weightEnabled)
    {
            z = SmoothCVector(temp, z, boxFoldMulti.control.weight);
    }
  }
}

  //Cube to sphere transform
  //Description: Warps a cube to a sphere; transform made by M.Benesi, optimized by Luca.
    //http://www.fractalforums.com/mathematics/circle2square/

void cubeSphereTransform3D(const sTransformCubeSphere &cubeSphere, CVector3 &z, int i)
{
  if (cubeSphere.control.enabled && i >= cubeSphere.control.startIterations && i < cubeSphere.control.stopIterations)
  {
    CVector3 temp = z;
    z *= z;       // so all now positive

    if (z.x == 0.0) z.x = 1e-21;
    if (z.z == 0.0) z.z = 1e-21;

    double rCyz = z.y / z.z;

    double rCxyz = (z.y + z.z ) / z.x ;

    if (rCxyz == -1.0) z.z = 1.0 + 1e-21;
    if (rCyz < 1.0) rCyz = sqrt(rCyz + 1.0);
    else rCyz=sqrt( 1.0 / rCyz + 1.0);

    if (rCxyz < 1.0) rCxyz = sqrt(rCxyz + 1.0);
    else rCxyz=sqrt(1.0/ rCxyz + 1.0);

    z.y *=rCyz;
    z.z *=rCyz;

    z *= rCxyz/SQRT_3_2;

    //weight function
    if (cubeSphere.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, cubeSphere.control.weight);
    }
  }
}

//sphere to Cube transform
//Warps a sphere to a cube; transform made by M.Benesi, optimized by
//Luca.  Scavenged and edited from code optimized by Luca.

  //http://www.fractalforums.com/mathematics/circle2square/
void sphereCubeTransform3D(const sTransformSphereCube &sphereCube, CVector3 &z, int i)
{
  if (sphereCube.control.enabled && i >= sphereCube.control.startIterations && i < sphereCube.control.stopIterations)
  {
    CVector3 temp = z;
    if (z.z == 0.0) z.z = 1e-21;
    z *= z;
    double rCyz = z.y / z.z;
    if (rCyz < 1.0) rCyz = 1.0 / sqrt(rCyz + 1.0);
    else rCyz = 1.0 / sqrt(1.0 / rCyz + 1.0);

    z.y*=rCyz;
    z.z*=rCyz;

    if (z.x == 0.0) z.x = 1e-21;
    double rCxyz = (z.y * z.y + z.z * z.z ) / z.x ;

    if (rCxyz < 1.0) rCxyz = 1.0 / sqrt(rCxyz + 1.0);
    else rCxyz = 1.0 /sqrt(1.0/ rCxyz + 1.0);

    z *= rCxyz * SQRT_3_2;

    //weight function
    if (sphereCube.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, sphereCube.control.weight);
    }
  }
}

// aboxModKali transform 3D
//http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/


