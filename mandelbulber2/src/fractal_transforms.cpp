/*
 * fractal_transforms.cpp
 *
 *  Created on: 30 wrz 2015
 *      Author: krzysztof
 */

#include "fractal_transforms.hpp"
#include "common_math.h"

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
//  fabsFormulaABCD   z = fabs(  z + const.A ) - fabs(  z - const.B )  + (  z * const.C  +  const.&D );  3D
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
// fabsFormulaABCD   z = fabs(  z + const.A ) - fabs(  z - const.B )  + (  z * const.C  +  const.&D );   4D
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
		double th0 = asin(z.z / aux.r) + mandelbulb.mandelbulbBetaAngleOffset;
		double ph0 = atan2(z.y, z.x) + mandelbulb.mandelbulbAlphaAngleOffset;
		double rp = pow(aux.r, mandelbulb.mandelbulbPower - 1.0);
		double th = th0 * mandelbulb.mandelbulbPower;
		double ph = ph0 * mandelbulb.mandelbulbPower;
		double cth = cos(th);
		aux.r_dz = rp * aux.r_dz * mandelbulb.mandelbulbPower + 1.0;
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
		z.x = fabs(z.x);
		z.y = fabs(z.y);
		z.z = fabs(z.z);
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
		z.x = fabs(z.x);
		z.y = fabs(z.y);
		z.z = fabs(z.z);
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
		double tempAuxColor = aux.color;
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
    CVector3 tempC = variableConstantMultiplier.constantMultiplier;// constant to be varied
    if ( i < variableConstantMultiplier.variableStartIterations)
    {
    ;
    }
    if ( i >= variableConstantMultiplier.variableStartIterations && i < variableConstantMultiplier.variableStopIterations && (variableConstantMultiplier.variableStopIterations - variableConstantMultiplier.variableStartIterations != 0))
    {
      tempC = (tempC + variableConstantMultiplier.variableConstant * (i - variableConstantMultiplier.variableStartIterations) / (variableConstantMultiplier.variableStopIterations - variableConstantMultiplier.variableStartIterations));
    }
    if ( i >= variableConstantMultiplier.variableStopIterations)
    {
      tempC =  (tempC + variableConstantMultiplier.variableConstant);
    }
    z += c  * tempC;

    //weight function
    if (variableConstantMultiplier.control.weightEnabled)
    {
      z = SmoothCVector(temp, z, variableConstantMultiplier.control.weight);
    }
  }
}

// mandelbulb Variable power 3D
void variableMandelbulbPowerTransform3D(const sTransformVariableMandelbulbPower &variableMandelbulbPower, CVector3 &z, int i, sExtendedAux &aux)
{
  if (variableMandelbulbPower.control.enabled && i >= variableMandelbulbPower.control.startIterations && i <variableMandelbulbPower.control.stopIterations)
  {
    CVector3 temp = z;
    double tempC = variableMandelbulbPower.mandelbulbPower;// constant to be varied
    if ( i < variableMandelbulbPower.variableStartIterations)
    {
    ;
    }
    if ( i >= variableMandelbulbPower.variableStartIterations && i < variableMandelbulbPower.variableStopIterations && (variableMandelbulbPower.variableStopIterations - variableMandelbulbPower.variableStartIterations != 0))
    {
      tempC = (tempC + variableMandelbulbPower.variableConstant * (i - variableMandelbulbPower.variableStartIterations) / (variableMandelbulbPower.variableStopIterations - variableMandelbulbPower.variableStartIterations));
    }
    if ( i >= variableMandelbulbPower.variableStopIterations)
    {
      tempC =  (tempC + variableMandelbulbPower.variableConstant);
    }
    aux.r = z.Length();
    double th0 = asin(z.z / aux.r) + variableMandelbulbPower.mandelbulbBetaAngleOffset;
    double ph0 = atan2(z.y, z.x) + variableMandelbulbPower.mandelbulbAlphaAngleOffset;
    double rp = pow(aux.r, tempC - 1.0);
    double th = th0 * tempC;
    double ph = ph0 * tempC;
    double cth = cos(th);
    aux.r_dz = rp * aux.r_dz * tempC + 1.0;
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
    CVector3 tempC = variableScale.scale;// constant to be varied
     if ( i < variableScale.variableStartIterations)
     {
     ;
     }
     if ( i >= variableScale.variableStartIterations && i < variableScale.variableStopIterations && (variableScale.variableStopIterations - variableScale.variableStartIterations != 0))
     {
       tempC = (tempC + variableScale.variableConstant * (i - variableScale.variableStartIterations) / (variableScale.variableStopIterations - variableScale.variableStartIterations));
     }
     if ( i >= variableScale.variableStopIterations)
     {
       tempC =  (tempC + variableScale.variableConstant);
     }

    z = z * tempC;
    aux.DE = aux.DE * fabs((tempC.x * tempC.y * tempC.z)/3) + 1.0; // need to fix
  }
}







