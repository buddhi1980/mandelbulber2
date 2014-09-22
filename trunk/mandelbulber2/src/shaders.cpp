/*********************************************************
 /                   MANDELBULBER
 / shaders for rendering engine
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#include "render_worker.hpp"
#include "calculate_distance.hpp"

sRGBAfloat cRenderWorker::ObjectShader(const sShaderInputData &_input, sRGBAfloat *surfaceColour, sRGBAfloat *specularOut)
{
	sRGBAfloat output;

	//normal vector
	CVector3 vn = CalculateNormals(_input);
	sShaderInputData input = _input;
	input.normal = vn;

	sRGBAfloat mainLight;
	mainLight.R = params->mainLightIntensity * params->mainLightColour.R / 65536.0;
	mainLight.G = params->mainLightIntensity * params->mainLightColour.G / 65536.0;
	mainLight.B = params->mainLightIntensity * params->mainLightColour.B / 65536.0;

	//calculate shading based on angle of incidence
	sRGBAfloat shade;
	if (params->mainLightEnable)
	{
		shade = MainShading(input);
		shade.R = params->mainLightIntensity * ((1.0 - params->shading) + params->shading * shade.R);
		shade.G = params->mainLightIntensity * ((1.0 - params->shading) + params->shading * shade.G);
		shade.B = params->mainLightIntensity * ((1.0 - params->shading) + params->shading * shade.B);
	}

	//calculate shadow
	sRGBAfloat shadow(1.0, 1.0, 1.0, 1.0);
	if(params->shadow && params->mainLightEnable)	shadow = MainShadow(input);

	//calculate specular highlight
	sRGBAfloat specular;
	if(params->mainLightEnable)
	{
		specular = MainSpecular(input);
		specular.R *= params->specular;
		specular.G *= params->specular;
		specular.B *= params->specular;
	}

	//calculate surface colour
	sRGBAfloat colour = SurfaceColour(input);
	*surfaceColour = colour;

	//ambient occlusion
	sRGBAfloat ambient(0.0, 0.0, 0.0, 0.0);
	if(params->ambientOcclusionEnabled)
	{
		//fast mode
		if(params->ambientOcclusionMode == params::AOmodeFast)
		{
			ambient = FastAmbientOcclusion(input);
		}
		else if(params->ambientOcclusionMode == params::AOmodeMultipeRays)
		{
			ambient = AmbientOcclusion(input);
		}
	}
	sRGBAfloat ambient2;
	ambient2.R = ambient.R * params->ambientOcclusion;
	ambient2.G = ambient.G * params->ambientOcclusion;
	ambient2.B = ambient.B * params->ambientOcclusion;

	//environment mapping
	sRGBAfloat envMapping(0.0, 0.0, 0.0, 0.0);
	if(params->envMappingEnable)
	{
		envMapping = EnvMapping(input);
	}
	envMapping.R *= params->reflect;
	envMapping.G *= params->reflect;
	envMapping.B *= params->reflect;

	//additional lights
	sRGBAfloat auxLights;
	sRGBAfloat auxLightsSpecular;
	auxLights = AuxLightsShader(input, &auxLightsSpecular);

	//fake orbit trap lights
	sRGBAfloat fakeLights(0.0, 0.0, 0.0, 0.0);
	sRGBAfloat fakeLightsSpecular(0.0, 0.0, 0.0, 0.0);
	if(params->fakeLightsEnabled)
	{
		fakeLights = FakeLights(input, &fakeLightsSpecular);
	}

	//total shader
	output.R = envMapping.R + (ambient2.R + mainLight.R * shade.R * shadow.R) * colour.R;
	output.G = envMapping.G + (ambient2.G + mainLight.G * shade.G * shadow.G) * colour.G;
	output.B = envMapping.B + (ambient2.B + mainLight.B * shade.B * shadow.B) * colour.B;

	output.R += (auxLights.R + fakeLights.R) * colour.R;
	output.G += (auxLights.G + fakeLights.G) * colour.G;
	output.B += (auxLights.B + fakeLights.B) * colour.B;
	output.A = 1.0;

	(*specularOut).R = auxLightsSpecular.R + fakeLightsSpecular.R + mainLight.R * specular.R * shadow.R;
	(*specularOut).G = auxLightsSpecular.G + fakeLightsSpecular.G + mainLight.G * specular.G * shadow.G;
	(*specularOut).B = auxLightsSpecular.B + fakeLightsSpecular.B + mainLight.B * specular.B * shadow.B;
	(*specularOut).A = output.A;

	return output;
}

sRGBAfloat cRenderWorker::BackgroundShader(const sShaderInputData &input)
{
	sRGBAfloat pixel2;

	if (params->texturedBackground)
	{
		if(params->texturedBackgroundMapType == params::mapDoubleHemisphere)
		{
			double alfaTexture = input.viewVector.GetAlpha();
			double betaTexture = input.viewVector.GetBeta();
			int texWidth = data->textures.backgroundTexture->Width()*0.5;
			int texHeight = data->textures.backgroundTexture->Height();
			int offset = 0;

			if(betaTexture < 0)
			{
				betaTexture = -betaTexture;
				alfaTexture += M_PI;
				offset = texWidth;
			}
			double texX = 0.5 * texWidth + cos(alfaTexture) * (1.0 - betaTexture / (0.5 * M_PI)) * texWidth * 0.5 + offset;
			double texY = 0.5 * texHeight + sin(alfaTexture) * (1.0 - betaTexture / (0.5 * M_PI)) * texHeight * 0.5;
			sRGB8 pixel = data->textures.backgroundTexture->Pixel(texX, texY);
			pixel2.R = pixel.R / 256.0;
			pixel2.G = pixel.G / 256.0;
			pixel2.B = pixel.B / 256.0;
		}
		else
		{
			double alfaTexture = fmod(input.viewVector.GetAlpha() + 2.5 * M_PI, 2 * M_PI);
			double betaTexture = input.viewVector.GetBeta();
			if (betaTexture > 0.5 * M_PI) betaTexture = 0.5 * M_PI - betaTexture;
			if (betaTexture < -0.5 * M_PI) betaTexture = -0.5 * M_PI + betaTexture;
			double texX = alfaTexture / (2.0 * M_PI) * data->textures.backgroundTexture->Width();
			double texY = (betaTexture / (M_PI) + 0.5) * data->textures.backgroundTexture->Height();
			sRGB8 pixel = data->textures.backgroundTexture->Pixel(texX, texY);
			pixel2.R = pixel.R/256.0;
			pixel2.G = pixel.G/256.0;
			pixel2.B = pixel.B/256.0;
		}
	}
	else
	{
		CVector3 vector(0.0, 0.0, 1.0);
		vector.Normalize();
		CVector3 viewVectorNorm = input.viewVector;
		viewVectorNorm.Normalize();
		double grad = (viewVectorNorm.Dot(vector)+1.0);
		sRGB16 pixel;
		if(grad < 1)
		{
			double Ngrad = 1.0 - grad;
			pixel.R = (params->background_color3.R * Ngrad + params->background_color2.R * grad);
			pixel.G = (params->background_color3.G * Ngrad + params->background_color2.G * grad);
			pixel.B = (params->background_color3.B * Ngrad + params->background_color2.B * grad);
		}
		else
		{
			grad = grad - 1;
			double Ngrad = 1.0 - grad;
			pixel.R = (params->background_color2.R * Ngrad + params->background_color1.R * grad);
			pixel.G = (params->background_color2.G * Ngrad + params->background_color1.G * grad);
			pixel.B = (params->background_color2.B * Ngrad + params->background_color1.B * grad);
		}

		pixel2.R = pixel.R/65536.0;
		pixel2.G = pixel.G/65536.0;
		pixel2.B = pixel.B/65536.0;
		pixel2.A = 0.0;
	}

	CVector3 viewVectorNorm = input.viewVector;
	viewVectorNorm.Normalize();
	double light = (viewVectorNorm.Dot(input.lightVect) - 1.0) * 360.0 / params->mainLightVisibilitySize;
	light = 1.0 / (1.0 + pow(light, 6.0)) * params->mainLightVisibility * params->mainLightIntensity;
	pixel2.R += light * params->mainLightColour.R / 65536.0;
	pixel2.G += light * params->mainLightColour.G / 65536.0;
	pixel2.B += light * params->mainLightColour.B / 65536.0;

	return pixel2;
}

sRGBAfloat cRenderWorker::VolumetricShader(const sShaderInputData &input, sRGBAfloat oldPixel, sRGBAfloat *opacityOut)
{
	sRGBAfloat output;
	float totalOpacity = 0.0;

	output.R = oldPixel.R;
	output.G = oldPixel.G;
	output.B = oldPixel.B;
	output.A = oldPixel.A;

  //volumetric fog init
	double colourThresh = params->volFogColour1Distance;
	double colourThresh2 = params->volFogColour2Distance;
	double fogReduce = params->volFogDistanceFactor;
	double fogIntensity = params->volFogDensity;

	//visible lights init
	int numberOfLights = data->lights.GetNumberOfLights();
	if (numberOfLights < 4) numberOfLights = 4;

	//glow init
  double glow = input.stepCount * params->glowIntensity / 512.0 * params->DEFactor;
  double glowN = 1.0 - glow;
  if (glowN < 0.0) glowN = 0.0;
  double glowR = (params->glowColor1.R * glowN + params->glowColor1.R * glow) / 65536.0;
  double glowG = (params->glowColor1.G * glowN + params->glowColor2.G * glow) / 65536.0;
  double glowB = (params->glowColor1.B * glowN + params->glowColor2.B * glow) / 65536.0;

  double totalStep = 0.0;

  //qDebug() << "Start volumetric shader &&&&&&&&&&&&&&&&&&&&";

  sShaderInputData input2 = input;
  for(int index = input.stepCount - 1; index > 0; index--)
	{
		double step = input.stepBuff[index].step;
		double distance = input.stepBuff[index].distance;
		CVector3 point = input.stepBuff[index].point;
		totalStep += step;

		input2.point = point;
		input2.distThresh = input.stepBuff[index].distThresh;

		//qDebug() << "i" << index << "dist" << distance << "iters" << input.stepBuff[index].iters << "distThresh" << input2.distThresh << "step" << step << "point" << point.Debug();

		if(totalStep < CalcDelta(point))
		{
			continue;
		}
		step = totalStep;
		totalStep = 0.0;

		//------------------- glow
		if (params->glowEnabled)
		{
			double glowOpacity = glow / input.stepCount;
			if(glowOpacity > 1.0) glowOpacity = 1.0;
			output.R = glowOpacity * glowR + (1.0 - glowOpacity) * output.R;
			output.G = glowOpacity * glowG + (1.0 - glowOpacity) * output.G;
			output.B = glowOpacity * glowB + (1.0 - glowOpacity) * output.B;
			output.A += glowOpacity;
		}
		//qDebug() << "step" << step;
		//------------------ visible light
		if (params->auxLightVisibility > 0)
		{
			double miniStep = 0.0;
			for(double miniSteps = 0.0; miniSteps < step; miniSteps += miniStep)
			{
				double lowestLightSize = 1e10;
				double lowestLightDist = 1e10;
				for (int i = 0; i < numberOfLights; ++i)
				{
					cLights::sLight light = data->lights.GetLight(i);
					if (light.enabled)
					{
						CVector3 lightDistVect = (point - input.viewVector * miniSteps) - light.position;
						double lightDist = lightDistVect.Length();
						double lightSize = sqrt(light.intensity) * params->auxLightVisibilitySize;
						double distToLightSurface = lightDist - lightSize;
						if (distToLightSurface < 0.0) distToLightSurface = 0.0;
						if (distToLightSurface <= lowestLightDist)
						{
							if (lightSize < lowestLightSize)
							{
								lowestLightSize = lightSize;
							}
							lowestLightDist = distToLightSurface;
						}
					}
				}

				miniStep = 0.1 * (lowestLightDist + 0.1 * lowestLightSize);
				if(miniStep > step - miniSteps) miniStep = step - miniSteps;
				//qDebug() << "ministeps" << miniSteps << "ministep" << miniStep;

				for (int i = 0; i < numberOfLights; ++i)
				{
					cLights::sLight light = data->lights.GetLight(i);
					if (light.enabled)
					{
						CVector3 lightDistVect = (point - input.viewVector * miniSteps) - light.position;
						double lightDist = lightDistVect.Length();
						double lightSize = sqrt(light.intensity) * params->auxLightVisibilitySize;
						double r2 = lightDist / lightSize;
						double bellFunction = 1.0 / (1.0 + pow(r2, 4.0));
						double lightDensity = miniStep * bellFunction * params->auxLightVisibility / lightSize;

						output.R += lightDensity * light.colour.R / 65536.0;
						output.G += lightDensity * light.colour.G / 65536.0;
						output.B += lightDensity * light.colour.B / 65536.0;
						output.A += lightDensity;
					}
				}
			}
		}

		//fake lights (orbit trap)
		if(params->fakeLightsEnabled)
		{
			/*
			double r = Compute<orbitTrap>(point, *input.calcParam);
			r = sqrt(1.0f/(r + 1.0e-30f));
			double fakeLight = 1.0 / (pow(r, 10.0 / params->fakeLightsVisibilitySize) * pow(10.0, 10.0 / params->fakeLightsVisibilitySize) + 1e-100);
			output.R += fakeLight * step * params->fakeLightsVisibility;
			output.G += fakeLight * step * params->fakeLightsVisibility;
			output.B += fakeLight * step * params->fakeLightsVisibility;
			output.alpha += fakeLight * step * params->fakeLightsVisibility;
			*/
		}

		//---------------------- volumetric lights with shadows in fog

		for (int i = 0; i < 5; i++)
		{
			if (i == 0 && params->volumetricLightEnabled[0])
			{
				sRGBAfloat shadowOutputTemp = MainShadow(input2);
				output.R += shadowOutputTemp.R * step * params->volumetricLightIntensity[0] * params->mainLightColour.R / 65536.0;
				output.G += shadowOutputTemp.G * step * params->volumetricLightIntensity[0] * params->mainLightColour.G / 65536.0;
				output.B += shadowOutputTemp.B * step * params->volumetricLightIntensity[0] * params->mainLightColour.B / 65536.0;
				output.A += (shadowOutputTemp.R + shadowOutputTemp.G + shadowOutputTemp.B)/3.0 * step * params->volumetricLightIntensity[0];
			}
			if (i > 0)
			{
				cLights::sLight light = data->lights.GetLight(i - 1);
				if (light.enabled && params->volumetricLightEnabled[i])
				{
					CVector3 d = light.position - point;
					double distance = d.Length();
					double distance2 = distance * distance;
					CVector3 lightVectorTemp = d;
					lightVectorTemp.Normalize();
					double lightShadow = AuxShadow(input2, distance, lightVectorTemp);
					output.R += lightShadow * light.colour.R / 65536.0 * params->volumetricLightIntensity[i] * step / distance2;
					output.G += lightShadow * light.colour.G / 65536.0 * params->volumetricLightIntensity[i] * step / distance2;
					output.B += lightShadow * light.colour.B / 65536.0 * params->volumetricLightIntensity[i] * step / distance2;
					output.A += lightShadow * params->volumetricLightIntensity[i] * step / distance2;
				}
			}
		}

		//----------------------- basic fog
		if(params->fogEnabled)
		{
			double fogDensity = step / params->fogVisibility;
			if(fogDensity > 1.0) fogDensity = 1.0;
			output.R = fogDensity * params->fogColor.R / 65536.0 + (1.0 - fogDensity) * output.R;
			output.G = fogDensity * params->fogColor.G / 65536.0 + (1.0 - fogDensity) * output.G;
			output.B = fogDensity * params->fogColor.B / 65536.0 + (1.0 - fogDensity) * output.B;
			totalOpacity = fogDensity + (1.0 - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0 - fogDensity) * output.A;
		}

		//-------------------- volumetric fog
		if(fogIntensity > 0.0 && params->volFogEnabled)
		{
			double densityTemp = (step * fogReduce) / (distance * distance + fogReduce * fogReduce);

			double k = distance / colourThresh;
			if (k > 1) k = 1.0;
			double kn = 1.0 - k;
			double fogRtemp = (params->volFogColour1.R * kn + params->volFogColour2.R * k);
			double fogGtemp = (params->volFogColour1.G * kn + params->volFogColour2.G * k);
			double fogBtemp = (params->volFogColour1.B * kn + params->volFogColour2.B * k);

			double k2 = distance / colourThresh2 * k;
			if (k2 > 1) k2 = 1.0;
			kn = 1.0 - k2;
			fogRtemp = (fogRtemp * kn + params->volFogColour3.R * k2);
			fogGtemp = (fogGtemp * kn + params->volFogColour3.G * k2);
			fogBtemp = (fogBtemp * kn + params->volFogColour3.B * k2);

			double fogDensity = 0.3 * fogIntensity * densityTemp / (1.0 + fogIntensity * densityTemp);
			if(fogDensity > 1) fogDensity = 1.0;

			output.R = fogDensity * fogRtemp / 65536.0 + (1.0 - fogDensity) * output.R;
			output.G = fogDensity * fogGtemp / 65536.0 + (1.0 - fogDensity) * output.G;
			output.B = fogDensity * fogBtemp / 65536.0 + (1.0 - fogDensity) * output.B;
			//qDebug() << "densityTemp " << densityTemp << "k" << k << "k2" << k2 << "fogTempR" << fogRtemp << "fogDensity" << fogDensity << "output.R" << output.R;

			totalOpacity = fogDensity + (1.0 - fogDensity) * totalOpacity;
			output.A = fogDensity + (1.0 - fogDensity) * output.A;
		}

		//iter fog
		if (params->iterFogEnabled)
		{
			int L = input.stepBuff[index].iters;
			double opacity = IterOpacity(step, L, params->N, params->iterFogOpacityTrim, params->iterFogOpacity);

			sRGBAfloat newColour(0.0, 0.0, 0.0, 0.0);
			if (opacity > 0)
			{
				//fog colour
				double iterFactor1 = (double) (L - params->iterFogOpacityTrim) / (params->iterFogColor1Maxiter - params->iterFogOpacityTrim);
				double k = iterFactor1;
				if (k > 1.0) k = 1.0;
				if (k < 0.0) k = 0.0;
				double kn = 1.0 - k;
				double fogColR = (params->iterFogColour1.R * kn + params->iterFogColour2.R * k);
				double fogColG = (params->iterFogColour1.G * kn + params->iterFogColour2.G * k);
				double fogColB = (params->iterFogColour1.B * kn + params->iterFogColour2.B * k);

				double iterFactor2 = (double) (L - params->iterFogColor1Maxiter) / (params->iterFogColor2Maxiter - params->iterFogColor1Maxiter);
				double k2 = iterFactor2;
				if (k2 < 0.0) k2 = 0.0;
				if (k2 > 1.0) k2 = 1.0;
				kn = 1.0 - k2;
				fogColR = (fogColR * kn + params->iterFogColour3.R * k2);
				fogColG = (fogColG * kn + params->iterFogColour3.G * k2);
				fogColB = (fogColB * kn + params->iterFogColour3.B * k2);
				//----

				for (int i = 0; i < 5; i++)
				{
					if (i == 0)
					{
						if (params->mainLightEnable && params->mainLightIntensity > 0.0)
						{
							sRGBAfloat shadowOutputTemp = MainShadow(input2);
							newColour.R += shadowOutputTemp.R * params->mainLightColour.R / 65536.0 * params->mainLightIntensity;
							newColour.G += shadowOutputTemp.G * params->mainLightColour.G / 65536.0 * params->mainLightIntensity;
							newColour.B += shadowOutputTemp.B * params->mainLightColour.B / 65536.0 * params->mainLightIntensity;
						}
					}

					if (i > 0)
					{
						cLights::sLight light = data->lights.GetLight(i - 1);
						if (light.enabled)
						{
							CVector3 d = light.position - point;
							double distance = d.Length();
							double distance2 = distance * distance;
							CVector3 lightVectorTemp = d;
							lightVectorTemp.Normalize();
							double lightShadow = AuxShadow(input2, distance, lightVectorTemp);
							double intensity = light.intensity * 100.0;
							newColour.R += lightShadow * light.colour.R / 65536.0 / distance2 * intensity;
							newColour.G += lightShadow * light.colour.G / 65536.0 / distance2 * intensity;
							newColour.B += lightShadow * light.colour.B / 65536.0 / distance2 * intensity;
						}
					}

				}

				if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeMultipeRays)
				{
					sRGBAfloat AO = AmbientOcclusion(input2);
					newColour.R += AO.R * params->ambientOcclusion;
					newColour.G += AO.G * params->ambientOcclusion;
					newColour.B += AO.B * params->ambientOcclusion;
				}

				if (opacity > 1.0) opacity = 1.0;

				output.R = output.R * (1.0 - opacity) + newColour.R * opacity * fogColR / 65536.0;
				output.G = output.G * (1.0 - opacity) + newColour.G * opacity * fogColG / 65536.0;
				output.B = output.B * (1.0 - opacity) + newColour.B * opacity * fogColB / 65536.0;
				totalOpacity = opacity + (1.0 - opacity) * totalOpacity;
				output.A = opacity + (1.0 - opacity) * output.A;
			}
		}


		if(totalOpacity > 1.0) totalOpacity = 1.0;
		if(output.A > 1.0) output.A = 1.0;
		(*opacityOut).R = totalOpacity;
		(*opacityOut).G = totalOpacity;
		(*opacityOut).B = totalOpacity;

	} //next stepCount


	return output;
}

sRGBAfloat cRenderWorker::MainShadow(const sShaderInputData &input)
{
	sRGBAfloat shadow(1.0, 1.0, 1.0, 1.0);

	//starting point
	CVector3 point2;

	double factor = input.delta / params->resolution;
	if (!params->penetratingLights) factor = params->viewDistanceMax;
	double dist = input.distThresh;

	double DEFactor = params->DEFactor;
	if(params->iterFogEnabled || params->volumetricLightEnabled) DEFactor = 1.0;

	//double start = input.delta;
	double start = input.distThresh;
	if (params->interiorMode) start = input.distThresh * DEFactor;

	double opacity = 0.0;
	double shadowTemp = 1.0;

	double softRange = tan(params->shadowConeAngle / 180.0 * M_PI);
	double maxSoft = 0.0;

	double bSoft = (!params->iterFogEnabled && !params->limitsEnabled && !params->iterThreshMode) && softRange > 0.0;

	for (double i = start; i < factor; i += dist * DEFactor)
	{
		point2 = input.point + input.lightVect * i;

		float dist_thresh;
		if(params->iterFogEnabled || params->volumetricLightEnabled)
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, dist_thresh, false);
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);

		if (bSoft)
		{
			double angle = (dist - dist_thresh) / i;
			if (angle < 0) angle = 0;
			if (dist < dist_thresh) angle = 0;
			double softShadow = (1.0 - angle / softRange);
			if (params->penetratingLights) softShadow *= (factor - i) / factor;
			if (softShadow < 0) softShadow = 0;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

		if (params->iterFogEnabled)
		{
			opacity = IterOpacity(dist * DEFactor, distanceOut.iters, params->N, params->iterFogOpacityTrim, params->iterFogOpacity);
		}
		else
		{
			opacity = 0.0;
		}
		shadowTemp -= opacity * (factor - i) / factor;

		if (dist < dist_thresh || shadowTemp < 0.0)
		{
			shadowTemp -= (factor - i) / factor;
			if (!params->penetratingLights) shadowTemp = 0.0;
			if (shadowTemp < 0.0) shadowTemp = 0.0;
			break;
		}
	}
	if (!bSoft)
	{
		shadow.R = shadowTemp;
		shadow.G = shadowTemp;
		shadow.B = shadowTemp;
	}
	else
	{
		shadow.R = (1.0 - maxSoft);
		shadow.G = (1.0 - maxSoft);
		shadow.B = (1.0 - maxSoft);
	}
	return shadow;
}

sRGBAfloat cRenderWorker::FastAmbientOcclusion(const sShaderInputData &input)
{
	//reference: http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf (Iñigo Quilez – iq/rgba)

	double delta = input.distThresh;
	double aoTemp = 0;
	double quality = params->ambientOcclusionQuality;
	for (int i = 1; i < quality * quality; i++)
	{
		double scan = i * i * delta;
		CVector3 pointTemp = input.point + input.normal * scan;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(pointTemp, input.distThresh, false);
		double dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);
		aoTemp += 1.0 / (pow(2.0, i)) * (scan - params->ambientOcclusionFastTune * dist) / input.distThresh;
	}
	double ao = 1.0 - 0.2 * aoTemp;
	if (ao < 0) ao = 0;
	sRGBAfloat output(ao, ao, ao, 1.0);
	return output;
}

sRGBAfloat cRenderWorker::AmbientOcclusion(const sShaderInputData &input)
{
	sRGBAfloat AO (0, 0, 0, 1.0);

	double start_dist = input.delta;
	double end_dist = input.delta / params->resolution;
	double intense = 0;

	for (int i = 0; i < AOvectorsCount; i++)
	{
		sVectorsAround v = AOvectorsAround[i];

		double dist = input.lastDist;

		double opacity = 0.0;
		double shadowTemp = 1.0;

		for (double r = start_dist; r < end_dist; r += dist * 2.0)
		{
			CVector3 point2 = input.point + v.v * r;

			sDistanceOut distanceOut;
			sDistanceIn distanceIn(point2, input.distThresh, false);
			dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);

			if (params->iterFogEnabled)
			{
				opacity = IterOpacity(dist * 2.0,distanceOut.iters, params->N, params->iterFogOpacityTrim,
						params->iterFogOpacity);
			}
			else
			{
				opacity = 0.0;
			}
			shadowTemp -= opacity * (end_dist - r) / end_dist;

			float dist_thresh;
			if(params->iterFogEnabled || params->volumetricLightEnabled)
			{
				dist_thresh = CalcDistThresh(point2);
			}
			else
				dist_thresh = input.distThresh;

			if (dist < dist_thresh || distanceOut.maxiter || shadowTemp < 0.0)
			{
				shadowTemp -= (end_dist - r) / end_dist;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
				break;
			}
		}

		intense = shadowTemp;

		AO.R += intense * v.R;
		AO.G += intense * v.G;
		AO.B += intense * v.B;

	}
	AO.R /= (AOvectorsCount * 256.0);
	AO.G /= (AOvectorsCount * 256.0);
	AO.B /= (AOvectorsCount * 256.0);

	return AO;
}

CVector3 cRenderWorker::CalculateNormals(const sShaderInputData &input)
{
	CVector3 normal(0.0, 0.0, 0.0);
	//calculating normal vector based on distance estimation (gradient of distance function)
	if (!params->slowShading)
	{
		double delta = input.delta * params->smoothness;
		if(params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		double s1, s2, s3, s4;
		sDistanceOut distanceOut;

		sDistanceIn distanceIn1(input.point, input.distThresh, true);
		s1 = CalculateDistance(*params, *fractal, distanceIn1, &distanceOut);

		CVector3 deltax(delta, 0.0, 0.0);
		sDistanceIn distanceIn2(input.point + deltax, input.distThresh, true);
		s2 = CalculateDistance(*params, *fractal, distanceIn2, &distanceOut);

		CVector3 deltay(0.0, delta, 0.0);
		sDistanceIn distanceIn3(input.point + deltay, input.distThresh, true);
		s3 = CalculateDistance(*params, *fractal, distanceIn3, &distanceOut);

		CVector3 deltaz(0.0, 0.0, delta);
		sDistanceIn distanceIn4(input.point + deltaz, input.distThresh, true);
		s4 = CalculateDistance(*params, *fractal, distanceIn4, &distanceOut);

		normal.x = s2 - s1;
		normal.y = s3 - s1;
		normal.z = s4 - s1;
	}

	//calculating normal vector based on average value of binary central difference
	else
	{
		CVector3 point2;
		CVector3 point3;
		double delta = input.delta * params->smoothness;
		if(params->interiorMode) delta = input.distThresh * 0.2 * params->smoothness;

		sDistanceOut distanceOut;
		for (point2.x = -1.0; point2.x <= 1.0; point2.x += 0.2) //+0.2
		{
			for (point2.y = -1.0; point2.y <= 1.0; point2.y += 0.2)
			{
				for (point2.z = -1.0; point2.z <= 1.0; point2.z += 0.2)
				{
					point3 = input.point + point2 * delta;

					sDistanceIn distanceIn(point3, input.distThresh, true);
					double dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);
					normal += (point2 * dist);
				}
			}
		}
	}
	if(normal.x == 0 && normal.y == 0 && normal.z == 0)
	{
		normal.x = 1;
	}
	else
	{
		normal.Normalize();
	}

	return normal;
}

sRGBAfloat cRenderWorker::MainShading(const sShaderInputData &input)
{
	sRGBAfloat shading;
	double shade = input.normal.Dot(input.lightVect);
	if (shade < 0) shade = 0;
	shading.R = shade;
	shading.G = shade;
	shading.B = shade;
	return shading;
}

sRGBAfloat cRenderWorker::MainSpecular(const sShaderInputData &input)
{
	sRGBAfloat specular;
	CVector3 half = input.lightVect - input.viewVector;
	half.Normalize();
	double shade2 = input.normal.Dot(half);
	if (shade2 < 0.0) shade2 = 0.0;
	shade2 = pow(shade2, 30.0) * 1.0;
	if (shade2 > 15.0) shade2 = 15.0;
	specular.R = shade2;
	specular.G = shade2;
	specular.B = shade2;
	return specular;
}

sRGBAfloat cRenderWorker::EnvMapping(const sShaderInputData &input)
{
	sRGBAfloat envReflect;
	CVector3 reflect;
	double dot = -input.viewVector.Dot(input.normal);
	reflect = input.normal * 2.0 * dot + input.viewVector;

	double alfaTexture = reflect.GetAlpha() + M_PI;
	double betaTexture = reflect.GetBeta();
	double texWidth = data->textures.envmapTexture->Width();
	double texHeight = data->textures.envmapTexture->Height();

	if (betaTexture > 0.5 * M_PI) betaTexture = 0.5 * M_PI - betaTexture;

	if (betaTexture < -0.5 * M_PI) betaTexture = -0.5 * M_PI + betaTexture;

	double dtx = (alfaTexture / (2.0 * M_PI)) * texWidth + texWidth * 8.25;
	double dty = (betaTexture / (M_PI) + 0.5) * texHeight + texHeight * 8.0;
	dtx = fmod(dtx, texWidth);
	dty = fmod(dty, texHeight);
	if (dtx < 0) dtx = 0;
	if (dty < 0) dty = 0;
	envReflect.R = data->textures.envmapTexture->Pixel(dtx, dty).R / 256.0;
	envReflect.G = data->textures.envmapTexture->Pixel(dtx, dty).G / 256.0;
	envReflect.B = data->textures.envmapTexture->Pixel(dtx, dty).B / 256.0;
	return envReflect;
}

sRGBAfloat cRenderWorker::SurfaceColour(const sShaderInputData &input)
{
#ifdef FAST_MAND
	double zx = point.x;
	double zy = point.y;
	int N = params->N * 10.0;
	double p = 2.0;
	double smooth = 0.0;
	int L = 0;
	double r = 0.0;
	int nrKol = 253 * 256;
  for(L=0; L<N; L++)
  {
  	double temp = zx * zx - zy * zy + point.x;
  	zy = 2.0 * zx * zy + point.y;
  	zx = temp;
  	r = zx * zx + zy * zy;
  	if(r > 1e20)
  	{
    	smooth = (L - log(log(sqrt(r)) / log(N)) / log(p));
      nrKol = smooth * 50.0;
      nrKol = abs(nrKol) % (248*256);
  		break;
  	}
  }

	N_counter += L + 1;
	Loop_counter++;

	if (L/10 < 64)
		histogram[L/10]++;
	else
		histogram[63]++;
  return nrKol;

#else
	sRGBAfloat out;

	switch(input.objectType)
	{
		case fractal::objFractal:
		{
			sRGB colour (256, 256, 256);
			if (params->coloringEnabled)
			{
				sFractalIn fractIn(input.point, params->juliaC, 0, params->N * 10);
				sFractalOut fractOut;
				Compute<fractal::colouring>(*fractal, fractIn, &fractOut);
				int nrCol = floor(fractOut.colorIndex);
				nrCol = abs(nrCol) % (248 * 256);

				int color_number;
				if (nrCol >= 248 * 256)
				{
					color_number = nrCol;
				}
				else
				{
					color_number = (int) (nrCol * params->coloring_speed + 256 * params->paletteOffset) % 65536;
				}
				colour = data->palette.IndexToColour(color_number);
			}

			out.R = colour.R / 256.0;
			out.G = colour.G / 256.0;
			out.B = colour.B / 256.0;
			break;
		}
		case fractal::objWater:
		{
			out.R = params->primitives.water.colour.R / 65536.0;
			out.G = params->primitives.water.colour.G / 65536.0;
			out.B = params->primitives.water.colour.B / 65536.0;
			break;
		}
		case fractal::objBox:
		{
			out.R = params->primitives.box.colour.R / 65536.0;
			out.G = params->primitives.box.colour.G / 65536.0;
			out.B = params->primitives.box.colour.B / 65536.0;
			break;
		}
		case fractal::objBoxInv:
		{
			out.R = params->primitives.invertedBox.colour.R / 65536.0;
			out.G = params->primitives.invertedBox.colour.G / 65536.0;
			out.B = params->primitives.invertedBox.colour.B / 65536.0;
			break;
		}
		case fractal::objSphere:
		{
			out.R = params->primitives.sphere.colour.R / 65536.0;
			out.G = params->primitives.sphere.colour.G / 65536.0;
			out.B = params->primitives.sphere.colour.B / 65536.0;
			break;
		}
		case fractal::objSphereInv:
		{
			out.R = params->primitives.invertedSphere.colour.R / 65536.0;
			out.G = params->primitives.invertedSphere.colour.G / 65536.0;
			out.B = params->primitives.invertedSphere.colour.B / 65536.0;
			break;
		}
		case fractal::objPlane:
		{
			out.R = params->primitives.plane.colour.R / 65536.0;
			out.G = params->primitives.plane.colour.G / 65536.0;
			out.B = params->primitives.plane.colour.B / 65536.0;
			break;
		}
		case fractal::objNone:
		{
			out = sRGBAfloat();
			break;
		}
	};

	return out;
#endif
}

sRGBAfloat cRenderWorker::LightShading(const sShaderInputData &input, cLights::sLight light, int number, sRGBAfloat *outSpecular)
{
	sRGBAfloat shading;

	CVector3 d = light.position - input.point;

	double distance = d.Length();

	//angle of incidence
	CVector3 lightVector = d;
	lightVector.Normalize();

	double intensity = 100.0 * light.intensity / (distance * distance) / number;
	double shade = input.normal.Dot(lightVector);
	if (shade < 0) shade = 0;
	shade = shade * intensity;
	if (shade > 500.0) shade = 500.0;

	//specular
	CVector3 half = lightVector - input.viewVector;
	half.Normalize();
	double shade2 = input.normal.Dot(half);
	if (shade2 < 0.0) shade2 = 0.0;
	shade2 = pow(shade2, 30.0) * 1.0;
	shade2 *= intensity * params->specular;
	if (shade2 > 15.0) shade2 = 15.0;

	//calculate shadow
	if ((shade > 0.01 || shade2 > 0.01) && params->shadow)
	{
		double light = AuxShadow(input, distance, lightVector);
		shade *= light;
		shade2 *= light;
	}
	else
	{
		if (params->shadow)
		{
			shade = 0;
			shade2 = 0;
		}
	}

	shading.R = shade * light.colour.R / 65536.0;
	shading.G = shade * light.colour.G / 65536.0;
	shading.B = shade * light.colour.B / 65536.0;

	outSpecular->R = shade2 * light.colour.R / 65536.0;
	outSpecular->G = shade2 * light.colour.G / 65536.0;
	outSpecular->B = shade2 * light.colour.B / 65536.0;

	return shading;
}

//needed rework!!!
sRGBAfloat cRenderWorker::AuxLightsShader(const sShaderInputData &input, sRGBAfloat *specularOut)
{

	int numberOfLights = data->lights.GetNumberOfLights();
	if (numberOfLights < 4) numberOfLights = 4;
	sRGBAfloat shadeAuxSum;
	sRGBAfloat specularAuxSum;
	for (int i = 0; i < numberOfLights; i++)
	{
		cLights::sLight light = data->lights.GetLight(i);
		if (i < params->auxLightNumber || light.enabled)
		{
			sRGBAfloat specularAuxOutTemp;
			sRGBAfloat shadeAux = LightShading(input, light, numberOfLights, &specularAuxOutTemp);
			shadeAuxSum.R += shadeAux.R;
			shadeAuxSum.G += shadeAux.G;
			shadeAuxSum.B += shadeAux.B;
			specularAuxSum.R += specularAuxOutTemp.R;
			specularAuxSum.G += specularAuxOutTemp.G;
			specularAuxSum.B += specularAuxOutTemp.B;
		}
	}
	*specularOut = specularAuxSum;
	return shadeAuxSum;
}

double cRenderWorker::AuxShadow(const sShaderInputData &input, double distance, CVector3 lightVector)
{
	double step = input.delta;
	double dist = step;
	double light = 1.0;

	double opacity = 0.0;
	double shadowTemp = 1.0;

	double DE_factor = params->DEFactor;
	if(params->iterFogEnabled || params->volumetricLightEnabled) DE_factor = 1.0;

	for (double i = input.delta; i < distance; i += dist * DE_factor)
	{
		CVector3 point2 = input.point + lightVector * i;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, input.distThresh, false);
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut);

		if (params->iterFogEnabled)
		{
			opacity = IterOpacity(dist * DE_factor, distanceOut.iters, params->N, params->iterFogOpacityTrim,
					params->iterFogOpacity);
		}
		else
		{
			opacity = 0.0;
		}
		shadowTemp -= opacity * (distance - i) / distance;

		float dist_thresh;
		if(params->iterFogEnabled || params->volumetricLightEnabled)
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		if (dist < dist_thresh || shadowTemp < 0.0)
		{
			if (params->penetratingLights)
			{
				shadowTemp -= (distance - i) / distance;
				if (shadowTemp < 0.0) shadowTemp = 0.0;
			}
			else
			{
				shadowTemp = 0.0;
			}
			break;
		}
	}
	light = shadowTemp;
	return light;
}

double cRenderWorker::IterOpacity(const double step, double iters, double maxN, double trim, double opacitySp)
{
	double opacity = ((double)iters - trim) / maxN;
	if(opacity < 0.0) opacity = 0.0;
	opacity*=opacity;
	opacity*=step  * opacitySp;
	if(opacity > 1.0) opacity = 1.0;
	return opacity;
}

//will be done later
sRGBAfloat cRenderWorker::FakeLights(const sShaderInputData &input, sRGBAfloat *fakeSpec)
{
	sRGBAfloat fakeLights;
	/*
	double delta = input.distThresh * params->smoothness;
	double rr = Compute<orbitTrap>(input.point, *input.calcParam);
	double fakeLight = params->fakeLightsIntensity/rr;
	double r = 1.0/(rr + 1e-30);

	CVector3 deltax(delta, 0.0, 0.0);
	double rx = 1.0/(Compute<orbitTrap>(input.point + deltax, *input.calcParam) + 1e-30);
	CVector3 deltay(0.0, delta, 0.0);
	double ry = 1.0/(Compute<orbitTrap>(input.point + deltay, *input.calcParam) + 1e-30);
	CVector3 deltaz(0.0, 0.0, delta);
	double rz = 1.0/(Compute<orbitTrap>(input.point + deltaz, *input.calcParam) + 1e-30);

	CVector3 fakeLightNormal;
	fakeLightNormal.x = r - rx;
	fakeLightNormal.y = r - ry;
	fakeLightNormal.z = r - rz;

	if(fakeLightNormal.x == 0 && fakeLightNormal.y == 0 && fakeLightNormal.z == 0)
	{
		fakeLightNormal.x = 0.0;
	}
	else
	{
		fakeLightNormal.Normalize();
	}
	double fakeLight2 = fakeLight * input.normal.Dot(fakeLightNormal);
	if(fakeLight2 < 0) fakeLight2 = 0;

	fakeLights.R = fakeLight2;
	fakeLights.G = fakeLight2;
	fakeLights.B = fakeLight2;

	CVector3 half = fakeLightNormal - input.viewVector;
	half.Normalize();
	double fakeSpecular = input.normal.Dot(half);
	if (fakeSpecular < 0.0) fakeSpecular = 0.0;
	fakeSpecular = pow(fakeSpecular, 30.0) * fakeLight;
	if (fakeSpecular > 15.0) fakeSpecular = 15.0;
	fakeSpec->R = fakeSpecular;
	fakeSpec->G = fakeSpecular;
	fakeSpec->B = fakeSpecular;
*/

	*fakeSpec = sRGBAfloat();
	return fakeLights;
}

