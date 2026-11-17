/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cSettings class - migration of legacy flat settings to the objects tree
 *
 * Implements the objects-tree migration part of cSettings (declared in settings.hpp):
 * MigrateToObjectsTree(), FlattenBooleanAddGroups(), the temporary legacy parameter
 * inject/delete helpers and the node definition helpers. It is a separate translation
 * unit of the same class, not a separate class.
 */

#include "settings.hpp"

#include <QRegularExpression>
#include <QSet>
#include <QString>
#include <QStringList>

#include "fractal_container.hpp"
#include "fractal_enums.h"
#include "initparameters.hpp"
#include "object_node_type.h"
#include "primitives.h"
#include "write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"

/**
 * Check if a parameter name is one of the temporary legacy names created by the
 * InjectTemporaryLegacy*Params() helpers (per-fractal indexed variants used by the migration
 * of settings files older than v2.35). Such names carry no meaning on their own: they are only
 * valid while an old file is being migrated (when they are injected into the container), and
 * must be ignored otherwise. The list also covers parameters removed from InitParameters()
 * whose values are consumed directly from the raw settings text during migration
 * (e.g. repeat_from, converted to iteration ranges by MigrateToObjectsTree()).
 * Declared as a member of cSettings (settings.hpp) because DecodeOneLine() in settings.cpp
 * needs it as well, while the parameters it recognises are owned by this translation unit.
 */
bool cSettings::IsTemporaryLegacyParamName(const QString &name)
{
	if (name == "boolean_operators") return true;
	// legacy hybrid wrap-around slot: not registered anymore, the migration reads the value
	// from the settings text and emulates it with per-formula start/stop iteration ranges
	if (name == "repeat_from") return true;

	static const QStringList prefixes = {"initial_waxis_", "julia_mode_", "julia_c_",
		"fractal_constant_factor_", "formula_maxiter_", "boolean_operator_", "dont_add_c_constant_"};

	for (const QString &prefix : prefixes)
	{
		if (!name.startsWith(prefix)) continue;
		bool ok = false;
		const int index = name.mid(prefix.length()).toInt(&ok);
		if (ok && index >= 1 && index <= 9) return true;
	}
	return false;
}

void cSettings::InjectTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par)
{
	// Injects temporary boolean params from the legacy flat format into the parameter
	// container. Ensure top-level boolean_operators flag exists
	if (!par->IfExists("boolean_operators"))
		par->addParam("boolean_operators", false, morphNone, paramStandard);
	// Inject per-fractal boolean_operator and dont_add_c_constant params
	const int maxLegacyFractals = 9;
	for (int i = 1; i < maxLegacyFractals; i++)
	{
		QString name = QString("boolean_operator_%1").arg(i);
		if (!par->IfExists(name)) par->addParam(name, 1, morphLinear, paramStandard);

		name = QString("dont_add_c_constant_%1").arg(i);
		if (!par->IfExists(name)) par->addParam(name, false, morphLinear, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyFractalParams(std::shared_ptr<cParameterContainer> par)
{
	// Injects temporary per-fractal julia_mode, julia_c, and fractal_constant_factor params
	// from the legacy flat format into the parameter container.
	const int maxLegacyFractals = 9;
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString name = QString("fractal_constant_factor_%1").arg(i);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(1.0, 1.0, 1.0), morphLinear, paramStandard);
	}
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString paramName = QString("julia_mode_%1").arg(i);
		if (!par->IfExists(paramName)) par->addParam(paramName, false, morphLinear, paramStandard);

		paramName = QString("julia_c_%1").arg(i);
		if (!par->IfExists(paramName))
			par->addParam(paramName, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);

		paramName = QString("formula_maxiter_%1").arg(i);
		if (!par->IfExists(paramName)) par->addParam(paramName, 250, morphLinear, paramStandard);

		paramName = QString("initial_waxis_%1").arg(i);
		if (!par->IfExists(paramName)) par->addParam(paramName, 0.0, morphAkima, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyFormulaTransformParams(
	std::shared_ptr<cFractalContainer> fractPar)
{
	// Injects temporary legacy formula transform params into each fractal container.
	// These are needed by TryResolveLegacyFractalParam to resolve formula_position_N,
	// formula_rotation_N, formula_repeat_N, formula_scale_N params from old .fract files.
	if (fractPar)
	{
		const int maxLegacyFractals = 9;
		for (int i = 0; i < maxLegacyFractals; i++)
		{
			fractPar->ensureCapacity(i);
			if (!fractPar->at(i)->IfExists("formula_material_id"))
				fractPar->at(i)->addParam("formula_material_id", 1, morphLinear, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_position"))
				fractPar->at(i)->addParam(
					"formula_position", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_rotation"))
				fractPar->at(i)->addParam(
					"formula_rotation", CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_repeat"))
				fractPar->at(i)->addParam(
					"formula_repeat", CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
			if (!fractPar->at(i)->IfExists("formula_scale"))
				fractPar->at(i)->addParam("formula_scale", 1.0, morphAkima, paramStandard);
		}
	}
}

void cSettings::InjectTemporaryLegacyFormulaMaterialIdParams(
	std::shared_ptr<cParameterContainer> par)
{
	// Injects a temporary global formula_material_id param into the top-level container.
	// This allows the decode loop to accept formula_material_id from old .fract files
	// (before node-based system) where it was a top-level parameter.
	// Compatibility2() will migrate it to per-fractal containers and this method will delete it.
	if (!par->IfExists("formula_material_id"))
	{
		par->addParam("formula_material_id", 1, morphLinear, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyPrimitiveTransformParams(
	std::shared_ptr<cParameterContainer> par, int primitiveIndex)
{
	// Injects temporary position/rotation/scale params for a legacy primitive into the parameter
	// container.
	QStringList legacyTypes = GetLegacyPrimitiveTypes();
	for (const QString &type : legacyTypes)
	{
		// Inject position param with default zero vector
		QString name = QString("primitive_%1_%2_position").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
		// Inject rotation param with default zero vector
		name = QString("primitive_%1_%2_rotation").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkimaAngle, paramStandard);
		// Inject scale param with default 1.0
		name = QString("primitive_%1_%2_scale").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name)) par->addParam(name, 1.0, morphAkima, paramStandard);
		// Inject repeat param with default zero vector (legacy transform param)
		name = QString("primitive_%1_%2_repeat").arg(type).arg(primitiveIndex);
		if (!par->IfExists(name))
			par->addParam(name, CVector3(0.0, 0.0, 0.0), morphAkima, paramStandard);
	}
}

void cSettings::InjectTemporaryLegacyPrimitiveNodeParams(
	std::shared_ptr<cParameterContainer> par, const QString &primitiveFullName)
{
	// Injects temporary node-related params for a specific legacy primitive into the parameter
	// container. These params are needed during decode but will be migrated to node-based params
	// and deleted by MigrateToObjectsTree().
	// Inject enabled param with default true
	QString name = primitiveFullName + "_enabled";
	if (!par->IfExists(name)) par->addParam(name, true, morphAkima, paramStandard);
	// Inject material_id param with default 1
	name = primitiveFullName + "_material_id";
	if (!par->IfExists(name)) par->addParam(name, 1, morphNone, paramStandard);
	// Inject calculation_order param with default 0
	name = primitiveFullName + "_calculation_order";
	if (!par->IfExists(name)) par->addParam(name, 0, morphNone, paramStandard);
	// Inject boolean_operator param with default 1 (Add)
	name = primitiveFullName + "_boolean_operator";
	if (!par->IfExists(name)) par->addParam(name, 1, morphNone, paramStandard);
	// Inject smooth_de_combine_enable param with default false
	name = primitiveFullName + "_smooth_de_combine_enable";
	if (!par->IfExists(name)) par->addParam(name, false, morphAkima, paramStandard);
	// Inject smooth_de_combine_distance param with default 0.1
	name = primitiveFullName + "_smooth_de_combine_distance";
	if (!par->IfExists(name)) par->addParam(name, 0.1, morphAkima, paramStandard);
}

// Base names of the parameters which were stored in the main container as "<baseName>_<N>"
// in the legacy flat format (settings files up to v2.34), where N was the 1-based fractal
// number (1..maxLegacyFractals). Only these names are allowed to be reinterpreted as
// per-fractal parameters. Many other parameters legitimately end with digits
// (e.g. "transf_offset_111", "analyticDE_tweak_005", "mat1_fractal_coloring_xyzC_111") and
// their suffix is part of the name, not a fractal index.
static const QStringList gLegacyPerFractalParams = {
	"boolean_operator",
	"check_for_bailout",
	"dont_add_c_constant",
	"fractal_constant_factor",
	"fractal_enable",
	"formula",
	"formula_iterations",
	"formula_material_id",
	"formula_maxiter",
	"formula_position",
	"formula_repeat",
	"formula_rotation",
	"formula_scale",
	"formula_start_iteration",
	"formula_stop_iteration",
	"formula_weight",
	"initial_waxis",
	"julia_c",
	"julia_mode",
	"smooth_de_combine_distance",
	"smooth_de_combine_enable",
};

bool cSettings::TryResolveLegacyFractalParam(const QString &decodeLine,
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fractPar,
	QString &resolvedLine, std::shared_ptr<cParameterContainer> &targetContainer)
{
	// Resolves a legacy fractal param name (e.g. "formula_1") to the base param name and target
	// fractal container. Early exit if fractPar is null or decodeLine is empty
	if (!fractPar || decodeLine.isEmpty()) return false;

	// Extract the parameter name (before the first space)
	int firstSpace = decodeLine.indexOf(' ');
	if (firstSpace <= 0) return false;

	QString rawName = decodeLine.left(firstSpace).trimmed();
	if (rawName.size() < 2) return false;

	// Strip surrounding quotes (single or double) from the parameter name
	if (((rawName.at(0) == '"' && rawName.at(rawName.size() - 1) == '"')
				|| (rawName.at(0) == '\'' && rawName.at(rawName.size() - 1) == '\'')))
	{
		rawName = rawName.mid(1, rawName.size() - 2).trimmed();
	}

	// Extract the fractal index from the trailing _N suffix
	int lastUnderscore = rawName.lastIndexOf('_');
	if (lastUnderscore <= 0) return false;

	bool conversionOK = false;
	int fractalIndex = rawName.mid(lastUnderscore + 1).toInt(&conversionOK) - 1;
	QString baseParam = rawName.left(lastUnderscore);

	// The legacy flat format supported only maxLegacyFractals fractals and only the known
	// per-fractal base names. Reject anything else (including parameters whose name simply
	// ends with digits) before touching the fractal container - otherwise a bogus index such
	// as "transf_offset_111" would grow the container with empty, uninitialized entries.
	const int maxLegacyFractals = 9;
	if (!conversionOK || fractalIndex < 0 || fractalIndex >= maxLegacyFractals
			|| !gLegacyPerFractalParams.contains(baseParam))
	{
		return false;
	}

	// Ensure the target fractal container exists before checking the base parameter
	fractPar->ensureCapacity(fractalIndex);

	// formula_maxiter_N is a top-level indexed parameter, not a legacy fractal param
	if (baseParam == "formula_maxiter")
	{
		return false;
	}

	// Check if the legacy param does not exist in the top-level container
	// and the base param exists in the target fractal container
	if (fractalIndex < fractPar->size() && !par->IfExists(rawName)
			&& fractPar->at(fractalIndex)->IfExists(baseParam))
	{
		// Build the resolved line with the base param name and update the target container
		resolvedLine = baseParam + decodeLine.mid(firstSpace);
		targetContainer = fractPar->at(fractalIndex);
		return true;
	}

	return false;
}

void cSettings::MigrateLegacyParamsToFractal(
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract)
{
	// Migrates legacy flat params (dont_add_c_constant, fractal_constant_factor, julia_mode, julia_c)
	// to the new fractal-based structure. Migrate per-fractal dont_add_c_constant params.
	// The legacy flat format had at most maxLegacyFractals fractals, so the loop is bounded by
	// that number instead of the current container size (which may contain extra node containers).
	const int maxLegacyFractals = 9;
	for (int i = 1; i <= std::min(fract->size(), maxLegacyFractals); i++)
	{
		QString oldParamName = QString("dont_add_c_constant_%1").arg(i);
		if (par->IfExists(oldParamName))
		{
			if (par->Get<bool>(oldParamName))
			{
				fract->at(i - 1)->Set("dont_add_c_constant", true);
			}
			// Remove the legacy param after migration
			par->DeleteParameter(oldParamName);
		}

		oldParamName = QString("formula_start_iteration_%1").arg(i);
		if (par->IfExists(oldParamName))
		{
			fract->at(i - 1)->Set("formula_start_iteration", par->Get<int>(oldParamName));
			// Remove the legacy param after migration
			par->DeleteParameter(oldParamName);
		}

		oldParamName = QString("formula_stop_iteration_%1").arg(i);
		if (par->IfExists(oldParamName))
		{
			fract->at(i - 1)->Set("formula_stop_iteration", par->Get<int>(oldParamName));
			// Remove the legacy param after migration
			par->DeleteParameter(oldParamName);
		}
	}

	// booleanMode kept for possible future migration logic; currently not consumed
	[[maybe_unused]] bool booleanMode =
		par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators");

	// Migrate global formula_material_id to each fractal container
	// (for files before node-based system where it was a top-level param)
	if (par->IfExists("formula_material_id") && !par->isDefaultValue("formula_material_id"))
	{
		int matId = par->Get<int>("formula_material_id");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("formula_material_id", matId);
		}
	}

	// Migrate global fractal_rotation to per-fractal formula_rotation
	// (for files before node-based system where it was a top-level param)
	if (par->IfExists("fractal_rotation") && !par->isDefaultValue("fractal_rotation"))
	{
		CVector3 rot = par->Get<CVector3>("fractal_rotation");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("formula_rotation", rot);
		}
	}

	// Migrate global fractal_position to per-fractal formula_position
	// (for files before node-based system where it was a top-level param)
	if (par->IfExists("fractal_position") && !par->isDefaultValue("fractal_position"))
	{
		CVector3 pos = par->Get<CVector3>("fractal_position");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("formula_position", pos);
		}
	}

	// Migrate global fractal repeat to per-fractal formula_repeat
	//  (for files before node-based system where it was a top-level param)
	if (par->IfExists("repeat") && !par->isDefaultValue("repeat"))
	{
		CVector3 repeat = par->Get<CVector3>("repeat");
		for (int i = 0; i < fract->size(); i++)
		{
			fract->at(i)->Set("formula_repeat", repeat);
		}
	}
}

QString cSettings::GetFormulaName(int formulaEnum)
{
	for (cAbstractFractal *f : newFractalList)
	{
		if (int(f->getInternalId()) == formulaEnum)
		{
			return QString::fromStdString(f->getInternalName());
		}
	}
	return "fractal";
}

enumNodeType cSettings::ToNodeType(int boolOp)
{
	switch (boolOp)
	{
		case 0: return enumNodeType::booleanMul;
		case 2: return enumNodeType::booleanSub;
		default: return enumNodeType::booleanAdd;
	}
}

enumNodeType cSettings::PrimitiveOpToNodeType(int boolOp)
{
	switch (boolOp)
	{
		case int(primBooleanOperatorAND): return enumNodeType::booleanMul;
		case int(primBooleanOperatorSUB): return enumNodeType::booleanSub;
		case int(primBooleanOperatorRevSUB): return enumNodeType::booleanSub;
		default: return enumNodeType::booleanAdd;
	}
}

QString cSettings::MakeNodeDefinition(const QString &formulaName, int nodeId, enumNodeType type,
	int parentId, int objectId, int displayOrder)
{
	return QString("%1 %2,%2,%3,%4,%5,%6")
		.arg(formulaName)
		.arg(nodeId)
		.arg(int(type))
		.arg(parentId)
		.arg(objectId)
		.arg(displayOrder);
}

QString cSettings::NodePrefix(int nodeId)
{
	return QString("node_%1_").arg(nodeId, 4, 10, QChar('0'));
}

QString cSettings::NodeDefinitionParam(int nodeId)
{
	return NodePrefix(nodeId) + "definition";
}

void cSettings::SetNodeParent(std::shared_ptr<cParameterContainer> par, int nodeId, int parentId)
{
	const QString defParam = NodeDefinitionParam(nodeId);
	if (!par->IfExists(defParam)) return;
	QStringList parts = par->Get<QString>(defParam).split(',');
	if (parts.size() < 5) return;
	parts[3] = QString::number(parentId);
	par->Set(defParam, parts.join(","));
}

void cSettings::CopyFormulaTransform(std::shared_ptr<cParameterContainer> par,
	const QString &prefix, std::shared_ptr<cParameterContainer> fracPar)
{
	par->Set(prefix + "position", fracPar->Get<CVector3>("formula_position"));
	par->Set(prefix + "rotation", fracPar->Get<CVector3>("formula_rotation"));
	par->Set(prefix + "scale", fracPar->Get<double>("formula_scale"));
	par->Set(prefix + "repeat", fracPar->Get<CVector3>("formula_repeat"));
	par->Set(prefix + "material", fracPar->Get<int>("formula_material_id"));
}

// Copy common fractal params (julia_mode, julia_c, etc.) from per-fractal container to node params.
// Used during migration to populate node-level params from legacy per-fractal params.
void cSettings::CopyOneFractalParams(
	std::shared_ptr<cParameterContainer> par, const QString &prefix, int formulaIndex)
{
	par->Set(prefix + "julia_mode", par->Get<bool>("julia_mode", formulaIndex));
	par->Set(prefix + "julia_c", par->Get<CVector3>("julia_c", formulaIndex));
	par->Set(prefix + "fractal_constant_factor",
		par->Get<CVector3>("fractal_constant_factor", formulaIndex));
	par->Set(prefix + "initial_waxis", par->Get<double>("initial_waxis", formulaIndex));
	par->Set(prefix + "formula_maxiter", par->Get<int>("formula_maxiter", formulaIndex));
}

void cSettings::CopyFractalParams(std::shared_ptr<cParameterContainer> par, const QString &prefix)
{
	par->Set(prefix + "julia_mode", par->Get<bool>("julia_mode"));
	par->Set(prefix + "julia_c", par->Get<CVector3>("julia_c"));
	par->Set(prefix + "fractal_constant_factor", par->Get<CVector3>("fractal_constant_factor"));
	par->Set(prefix + "initial_waxis", par->Get<double>("initial_waxis"));
	par->Set(prefix + "formula_maxiter", par->Get<int>("N"));
}

void cSettings::CopyPrimitiveTransform(
	std::shared_ptr<cParameterContainer> par, const QString &primitiveFullName, const QString &prefix)
{
	if (par->IfExists(primitiveFullName + "_position"))
		par->Set(prefix + "position", par->Get<CVector3>(primitiveFullName + "_position"));
	if (par->IfExists(primitiveFullName + "_rotation"))
		par->Set(prefix + "rotation", par->Get<CVector3>(primitiveFullName + "_rotation"));
	if (par->IfExists(primitiveFullName + "_scale"))
	{
		// The legacy per-primitive scale is a vector (one factor per axis), while the node
		// parameter "node_XXXX_scale" is a single value. Copying the vector as-is would store a
		// 3-component value in a double parameter: it is written to the settings text as
		// "node_XXXX_scale 1 0 0" and another instance (NetRender client, queue worker) cannot
		// parse it back, which leaves the node scale at 0 and renders a black image.
		// The node model supports only a uniform scale, so the first component is used.
		par->Set(prefix + "scale", par->Get<CVector3>(primitiveFullName + "_scale").x);
	}
	if (par->IfExists(primitiveFullName + "_repeat"))
		par->Set(prefix + "repeat", par->Get<CVector3>(primitiveFullName + "_repeat"));
}

// Migrates the legacy per-primitive smooth distance-estimate combine params
// (smooth_de_combine_enable / smooth_de_combine_distance) onto the node prefix so
// the objects tree can apply smooth blending on the primitive leaf node.
void cSettings::CopyPrimitiveSmoothDECombine(
	std::shared_ptr<cParameterContainer> par, const QString &primitiveFullName, const QString &prefix)
{
	if (par->IfExists(primitiveFullName + "_smooth_de_combine_enable"))
	{
		par->Set(prefix + "smooth_de_combine_enable",
			par->Get<bool>(primitiveFullName + "_smooth_de_combine_enable"));
	}
	if (par->IfExists(primitiveFullName + "_smooth_de_combine_distance"))
	{
		par->Set(prefix + "smooth_de_combine_distance",
			par->Get<double>(primitiveFullName + "_smooth_de_combine_distance"));
	}
}

QList<int> cSettings::GetEnabledFractals(std::shared_ptr<cFractalContainer> fract)
{
	QList<int> enabledFractals;
	for (int i = 0; i < fract->size(); i++)
	{
		if (fract->at(i)->IfExists("formula")
				&& fract->at(i)->Get<int>("formula") != int(fractal::none))
		{
			enabledFractals.append(i + 1);
		}
	}
	return enabledFractals;
}

bool cSettings::GetFractalEnableFlag(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fract, int fractalIndex)
{
	QString paramName = QString("fractal_enable_%1").arg(fractalIndex);
	if (par->IfExists(paramName))
	{
		return par->Get<bool>(paramName);
	}
	if (fractalIndex >= 1 && fractalIndex - 1 < fract->size())
	{
		if (fract->at(fractalIndex - 1)->IfExists("fractal_enable"))
		{
			return fract->at(fractalIndex - 1)->Get<bool>("fractal_enable");
		}
	}
	return true;
}

// Default object_id value assigned to primitives that were not explicitly renumbered.
static const int DefaultPrimitiveObjectId = 1234;

// Returns the legacy material for a hybrid root node. In hybrid mode all child fractals are
// computed together as a single hybrid sequence and their surfaces take the material from the
// hybrid group node itself (children do not carry material information). The legacy value is
// the global formula_material_id (1-based, mat1_* == 1); MigrateLegacyParamsToFractal copies
// it onto each fractal container, so reading the first used fractal slot covers both old
// (< 2.29, global param) and transitional (2.29, per-fractal param) file layouts.
static int GetHybridRootLegacyMaterial(std::shared_ptr<cFractalContainer> fract)
{
	for (int i = 0; i < fract->size(); i++)
	{
		if (fract->at(i)->IfExists("formula")
				&& fract->at(i)->Get<int>("formula") != int(fractal::none))
		{
			if (fract->at(i)->IfExists("formula_material_id"))
			{
				return fract->at(i)->Get<int>("formula_material_id");
			}
			return 1;
		}
	}
	return 1;
}

// Converts the legacy repeat_from parameter (removed in v2.35) into per-formula iteration
// ranges. In the old nine-slot hybrid mode the sequence generator wrapped back to slot
// (repeat_from - 1) after the last slot, e.g. formulas A, B, C in slots 1..3 with repeat_from 2
// produced the pattern A, B, C, B, C, B, C, ... The compact objects-tree list always wraps back
// to its first formula, so the same pattern is reproduced by restricting every formula placed
// before the wrap point to its first-run iteration block: once past that range the sequence
// builder skips them and the wrap lands on the first formula at or after the legacy wrap slot.
// If the wrap slot pointed at an empty ('none') slot, the old engine terminated the orbit after
// the first pass; that is reproduced by restricting all formulas (the -1/none table fallback
// then ends the sequence). Formulas are identified by their legacy slot order, which is also
// the order of the compact child list created by MigrateToObjectsTree().
static void MigrateRepeatFromToIterationRanges(
	const QString &settingsText, std::shared_ptr<cFractalContainer> fract)
{
	// repeat_from is no longer a registered parameter, its value is recovered from the raw
	// file text (same technique as the boolean_operator_1 detection in MigrateToObjectsTree)
	const QRegularExpression repeatFromRe("repeat_from\\s+(\\d+)");
	const QRegularExpressionMatch match = repeatFromRe.match(settingsText);
	if (!match.hasMatch())
	{
		return;
	}
	// 0-based legacy slot index to which the old sequence wrapped back
	const int wrapSlot = match.captured(1).toInt() - 1;
	if (wrapSlot <= 0)
	{
		// wrapping to the first slot is the default behavior of the compact list
		return;
	}

	// collect the used formula slots (formula != none) in legacy slot order
	const int maxLegacyFractals = 9;
	QList<int> usedSlots;
	for (int i = 0; i < std::min(fract->size(), maxLegacyFractals); i++)
	{
		if (fract->at(i)->IfExists("formula")
				&& fract->at(i)->Get<int>("formula") != int(fractal::none))
		{
			usedSlots.append(i);
		}
	}

	// find the first used slot at or after the wrap point: from there the sequence cycles
	// forever; the fallback value (end of list) means the wrap landed on an empty slot
	int wrapIndex = usedSlots.size();
	for (int j = 0; j < usedSlots.size(); j++)
	{
		if (usedSlots[j] >= wrapSlot)
		{
			wrapIndex = j;
			break;
		}
	}
	if (wrapIndex == 0)
	{
		// nothing is placed before the wrap point, the default wrap to index 0 is equivalent
		return;
	}

	// Restrict each formula before the wrap point to its first-run block. The old engine
	// consumed one table entry per iteration, so formula j occupied iterations
	// [prefixSum, prefixSum + formula_iterations_j - 1]. The existing (user set) stop value
	// is never extended: min() keeps the tighter of both restrictions.
	const int defaultIterations = 1;
	const int defaultStop = 250;
	int blockStart = 0;
	for (int j = 0; j < wrapIndex; j++)
	{
		const int slot = usedSlots[j];
		const int iterations = fract->at(slot)->IfExists("formula_iterations")
														 ? fract->at(slot)->Get<int>("formula_iterations")
														 : defaultIterations;
		const int blockEnd = blockStart + qMax(1, iterations) - 1;
		const int oldStop = fract->at(slot)->IfExists("formula_stop_iteration")
													? fract->at(slot)->Get<int>("formula_stop_iteration")
													: defaultStop;
		fract->at(slot)->Set("formula_stop_iteration", qMin(oldStop, blockEnd));
		blockStart = blockEnd + 1;
	}
}

void cSettings::MigrateToObjectsTree(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> fract, int &nextGroupObjectId)
{
	// Scan raw settings text to find which primitives had any parameter in the original file.
	// This is needed because InitPrimitiveParams adds default values during decode,
	// so IfExists() would always return true. We need the regex to distinguish
	// "parameter was explicitly set in the file" from "parameter was initialized by
	// InitPrimitiveParams".
	QSet<QString> primitivesWithParamInFile;
	QRegularExpression re("(primitive_\\w+_\\d+)_\\w+");
	QRegularExpressionMatchIterator regexIt(re.globalMatch(settingsText));
	while (regexIt.hasNext())
	{
		QRegularExpressionMatch match = regexIt.next();
		primitivesWithParamInFile.insert(match.captured(1));
	}

	// Also track which primitives had explicit object_id in the file.
	QSet<QString> primitivesWithObjectIdInFile;
	QRegularExpression objectIdRe("primitive_(\\w+_\\d+)_object_id");
	QRegularExpressionMatchIterator objectIdIt(objectIdRe.globalMatch(settingsText));
	while (objectIdIt.hasNext())
	{
		QRegularExpressionMatch match = objectIdIt.next();
		primitivesWithObjectIdInFile.insert(match.captured(1));
	}

	// Initialize primitive params for all primitives detected from the file.
	// This is needed because InitPrimitiveParams is only called for primitives
	// that have their *Enable flag set to true. Disabled primitives with any
	// other parameter (e.g., radius) need to be initialized so GetListOfPrimitives
	// can find them via their _enabled parameter.
	for (const QString &primitiveFullName : primitivesWithParamInFile)
	{
		// Check if this primitive has already been initialized (has _enabled param).
		if (par->IfExists(primitiveFullName + "_enabled"))
		{
			continue;
		}
		// Extract primitive type from fullName: "primitive_torus_1" -> "torus"
		QStringList parts = primitiveFullName.split('_');
		if (parts.size() >= 3)
		{
			QString typeName = parts[1];
			int id = parts[2].toInt();
			fractal::enumObjectType objectType = cPrimitives::PrimitiveNameToEnum(typeName);
			if (objectType != fractal::objNone)
			{
				sPrimitiveItem item(objectType, id, primitiveFullName, typeName, 0, false);
				InitPrimitiveParams(item, par);
			}
		}
	}

	// Migrates legacy flat params to the new node-based objects tree structure.
	// Phase 1: migrate transforms for primitives that had explicit object_id in the file.
	QList<sPrimitiveItem> primitives = cPrimitives::GetListOfPrimitives(par);
	for (auto &primitive : primitives)
	{
		primitive.objectIdInFile = primitivesWithObjectIdInFile.contains(primitive.fullName);
	}
	for (const auto &primitive : primitives)
	{
		// Migrate transforms for primitives that had explicit object_id in the file,
		// even if they are currently disabled. This ensures disabled primitives
		// with explicit parameters get their transforms migrated too.
		if (!primitive.objectIdInFile)
		{
			continue;
		}

		int objectId = par->Get<int>(primitive.Name("object_id"));
		// Skip primitives that don't have explicit object_id in the file
		// (they use the default value 1234 set by InitPrimitiveParams).
		if (objectId == DefaultPrimitiveObjectId)
		{
			continue;
		}

		const QString prefix = NodePrefix(objectId);

		if (par->IfExists(primitive.Name("position"))
				&& !par->isDefaultValue(primitive.Name("position")))
		{
			par->Set(prefix + "position", par->Get<CVector3>(primitive.Name("position")));
		}
		if (par->IfExists(primitive.Name("rotation"))
				&& !par->isDefaultValue(primitive.Name("rotation")))
		{
			par->Set(prefix + "rotation", par->Get<CVector3>(primitive.Name("rotation")));
		}
		if (par->IfExists(primitive.Name("scale")) && !par->isDefaultValue(primitive.Name("scale")))
		{
			par->Set(prefix + "scale", par->Get<double>(primitive.Name("scale")));
		}
	}

	bool hybridMode = par->Get<bool>("hybrid_fractal_enable");
	bool booleanMode = par->IfExists("boolean_operators") && par->Get<bool>("boolean_operators");

	// For old file formats (< v2.29), boolean_operators flag may not be set explicitly.
	// Detect if any primitive has a non-default boolean operator (not primBooleanOperatorOR).
	// If so, enable boolean mode to ensure the fractal boolean tree is created.
	if (!booleanMode && fileVersion < 2.29)
	{
		QList<sPrimitiveItem> primList = cPrimitives::GetListOfPrimitives(par);
		for (const auto &prim : primList)
		{
			QString boolOpName = prim.Name("boolean_operator");
			if (par->IfExists(boolOpName))
			{
				int boolOpVal = par->Get<int>(boolOpName);
				if (boolOpVal != int(primBooleanOperatorOR))
				{
					booleanMode = true;
					par->Set("boolean_operators", true);
					break;
				}
			}
		}
	}

	// If boolean_operator_1 is defined in the file, use its value as default for
	// missing boolean_operator_N. This ensures files like boolean001.fract
	// (boolean_operator_1=2) and boolean002.fract (boolean_operator_1=0) get the
	// correct default. If boolean_operator_1 is NOT defined, keep Add (1).
	if (booleanMode)
	{
		QRegularExpression boolOp1Re("boolean_operator_1\\s(\\d+)");
		const QRegularExpressionMatch boolOp1Match = boolOp1Re.match(settingsText);
		const int defaultBoolOp = boolOp1Match.hasMatch() ? boolOp1Match.captured(1).toInt() : 1;
		for (int i = 1; i < 9; i++)
		{
			QString name = QString("boolean_operator_%1").arg(i);
			QRegularExpression boolOpRe(QString("boolean_operator_%1\\s").arg(i));
			if (!boolOpRe.match(settingsText).hasMatch())
			{
				par->Set(name, defaultBoolOp);
			}
		}
	}

	// Migrate legacy flat fractal params to the new fractal-based structure
	MigrateLegacyParamsToFractal(par, fract);

	// The old engine used repeat_from only in hybrid mode, so convert it only when a hybrid
	// sequence will actually be built (boolean mode combined with hybrid shares the same path)
	if (hybridMode)
	{
		MigrateRepeatFromToIterationRanges(settingsText, fract);
	}

	QList<int> enabledFractals;

	// Handle boolean mode: build a node tree for multiple fractals
	if (booleanMode)
	{
		enabledFractals = GetEnabledFractals(fract);
		const int m = enabledFractals.size();

		// In hybrid mode, create the hybrid root first so primitives can attach to it
		if (hybridMode && m >= 1)
		{
			InitNodeParams(1, par);
			par->Set("node_0001_definition",
				MakeNodeDefinition("hybrid", 1, enumNodeType::hybrid, 0, nextGroupObjectId++));
			// The hybrid group node must carry the legacy formula_material_id: hybrid children are
			// rendered as one sequence and their surfaces resolve the material through this node.
			// InitNodeParams defaults material to 1, which wrongly pinned mat1 regardless of the
			// formula_material_id value stored in the file.
			par->Set("node_0001_material", GetHybridRootLegacyMaterial(fract));

			if (par->IfExists("fractal_rotation") && !par->isDefaultValue("fractal_rotation"))
				par->Set("node_0001_rotation", par->Get<CVector3>("fractal_rotation"));
			if (par->IfExists("fractal_position") && !par->isDefaultValue("fractal_position"))
				par->Set("node_0001_position", par->Get<CVector3>("fractal_position"));
			if (par->IfExists("repeat") && !par->isDefaultValue("repeat"))
				par->Set("node_0001_repeat", par->Get<CVector3>("repeat"));
			if (par->IfExists("fractal_constant_factor")
					&& !par->isDefaultValue("fractal_constant_factor"))
				par->Set(
					"node_0001_fractal_constant_factor", par->Get<CVector3>("fractal_constant_factor"));
			if (par->IfExists("initial_waxis") && !par->isDefaultValue("initial_waxis"))
				par->Set("node_0001_initial_waxis", par->Get<double>("initial_waxis"));
			if (par->IfExists("julia_mode") && !par->isDefaultValue("julia_mode"))
				par->Set("node_0001_julia_mode", par->Get<bool>("julia_mode"));
			if (par->IfExists("julia_c") && !par->isDefaultValue("julia_c"))
				par->Set("node_0001_julia_c", par->Get<CVector3>("julia_c"));
			if (par->IfExists("N")) par->Set("node_0001_formula_maxiter", par->Get<int>("N"));

			// Create fractal child nodes under the hybrid root
			int childNodeId = 2;
			for (int objectId = 1; objectId <= fract->size(); objectId++)
			{
				if (fract->at(objectId - 1)->IfExists("formula")
						&& fract->at(objectId - 1)->Get<int>("formula") != int(fractal::none))
				{
					InitNodeParams(childNodeId, par);
					QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
					QString prefix = NodePrefix(childNodeId);
					par->Set(prefix + "definition",
						MakeNodeDefinition(formulaName, childNodeId, enumNodeType::fractal, 1, objectId));
					CopyFormulaTransform(par, prefix, fract->at(objectId - 1));
					CopyOneFractalParams(par, prefix, objectId);
					if (fileVersion < 2.29 && par->IfExists("N"))
						par->Set(prefix + "formula_maxiter", par->Get<int>("N"));

					QString fractalEnableParam = QString("fractal_enable_%1").arg(objectId);
					bool hasParam = par->IfExists(fractalEnableParam)
													|| (fract->at(objectId - 1)->IfExists("fractal_enable"));
					bool paramValue = true;
					if (hasParam)
					{
						if (par->IfExists(fractalEnableParam))
							paramValue = par->Get<bool>(fractalEnableParam);
						else if (fract->at(objectId - 1)->IfExists("fractal_enable"))
							paramValue = fract->at(objectId - 1)->Get<bool>("fractal_enable");
					}
					if (hasParam && !paramValue) par->Set(prefix + "enabled", false);

					childNodeId++;
				}
			}
		}
		// Non-hybrid boolean mode: build fractal boolean tree
		else if (m >= 2)
		{
			// Build a left-associative binary tree for m fractals and m-1 operators.
			// IDs are assigned as:
			//   Boolean nodes (innermost first): ids 1 .. m-1
			//   Fractal nodes: ids m .. 2m-1

			// Create boolean (inner) nodes
			for (int k = 0; k < m - 1; k++)
			{
				int nodeId = k + 1;
				InitNodeParams(nodeId, par);

				int slotIdx = enabledFractals[k + 1] - 1;
				if (slotIdx < 1)
				{
					slotIdx = 1;
				}
				int boolOp = par->Get<int>(QString("boolean_operator_%1").arg(slotIdx));
				enumNodeType nodeType = ToNodeType(boolOp);

				int parentId = (k == m - 2) ? 0 : (k + 2);
				par->Set(QString("node_%1_definition").arg(nodeId, 4, 10, QChar('0')),
					MakeNodeDefinition("boolean", nodeId, nodeType, parentId, nextGroupObjectId++));
				par->Set(NodePrefix(nodeId) + "material", -1);
			}

			// Create fractal (leaf) nodes and attach them to the boolean tree
			for (int i = 0; i < m; i++)
			{
				int nodeId = m + i;
				int objectId = enabledFractals[i];
				InitNodeParams(nodeId, par);

				QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
				int parentId = (i <= 1) ? 1 : i;
				par->Set(QString("node_%1_definition").arg(nodeId, 4, 10, QChar('0')),
					MakeNodeDefinition(formulaName, nodeId, enumNodeType::fractal, parentId, objectId));
				CopyFormulaTransform(par, NodePrefix(nodeId), fract->at(objectId - 1));
				CopyOneFractalParams(par, NodePrefix(nodeId), objectId);
				if (fileVersion < 2.29 && par->IfExists("N"))
					par->Set(NodePrefix(nodeId) + "formula_maxiter", par->Get<int>("N"));

				if (!GetFractalEnableFlag(par, fract, objectId))
					par->Set(NodePrefix(nodeId) + "enabled", false);
			}
		}
		// Single non-hybrid fractal in boolean mode
		else if (m == 1)
		{
			InitNodeParams(1, par);
			int objectId = enabledFractals[0];
			QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
			par->Set("node_0001_definition",
				MakeNodeDefinition(formulaName, 1, enumNodeType::fractal, 0, objectId));
			CopyFormulaTransform(par, "node_0001_", fract->at(objectId - 1));
			CopyOneFractalParams(par, "node_0001_", objectId);
			if (fileVersion < 2.29 && par->IfExists("N"))
				par->Set("node_0001_formula_maxiter", par->Get<int>("N"));
			if (!GetFractalEnableFlag(par, fract, objectId)) par->Set("node_0001_enabled", false);
		}
	}
	// Handle non-hybrid single-fractal mode
	else if (!hybridMode)
	{
		const int nodeId = 1;
		const int objectId = 1;
		InitNodeParams(nodeId, par);

		QString formulaName = GetFormulaName(fract->at(0)->Get<int>("formula"));
		par->Set("node_0001_definition",
			MakeNodeDefinition(formulaName, nodeId, enumNodeType::fractal, 0, objectId));
		CopyFormulaTransform(par, "node_0001_", fract->at(0));
		CopyFractalParams(par, "node_0001_");
		if (fract->at(0)->IfExists("fractal_enable") && !fract->at(0)->Get<bool>("fractal_enable"))
		{
			par->Set("node_0001_enabled", false);
		}
	}
	// Handle hybrid mode
	else
	{
		enabledFractals = GetEnabledFractals(fract);

		if (!enabledFractals.isEmpty())
		{
			// Create a hybrid root node
			InitNodeParams(1, par);
			par->Set("node_0001_definition",
				MakeNodeDefinition("hybrid", 1, enumNodeType::hybrid, 0, nextGroupObjectId++));
			// The hybrid group node must carry the legacy formula_material_id (same reason as in the
			// boolean+hybrid branch): hybrid children are computed together and resolve their
			// material through the group node, not through the individual fractal children.
			par->Set("node_0001_material", GetHybridRootLegacyMaterial(fract));

			// Apply global fractal transform params to the hybrid root node
			if (par->IfExists("fractal_rotation") && !par->isDefaultValue("fractal_rotation"))
			{
				par->Set("node_0001_rotation", par->Get<CVector3>("fractal_rotation"));
			}
			if (par->IfExists("fractal_position") && !par->isDefaultValue("fractal_position"))
			{
				par->Set("node_0001_position", par->Get<CVector3>("fractal_position"));
			}
			if (par->IfExists("repeat") && !par->isDefaultValue("repeat"))
			{
				par->Set("node_0001_repeat", par->Get<CVector3>("repeat"));
			}
			if (par->IfExists("fractal_constant_factor")
					&& !par->isDefaultValue("fractal_constant_factor"))
			{
				par->Set(
					"node_0001_fractal_constant_factor", par->Get<CVector3>("fractal_constant_factor"));
			}
			if (par->IfExists("initial_waxis") && !par->isDefaultValue("initial_waxis"))
			{
				par->Set("node_0001_initial_waxis", par->Get<double>("initial_waxis"));
			}
			if (par->IfExists("julia_mode") && !par->isDefaultValue("julia_mode"))
			{
				par->Set("node_0001_julia_mode", par->Get<bool>("julia_mode"));
			}
			if (par->IfExists("julia_c") && !par->isDefaultValue("julia_c"))
			{
				par->Set("node_0001_julia_c", par->Get<CVector3>("julia_c"));
			}
			if (par->IfExists("N"))
			{
				par->Set("node_0001_formula_maxiter", par->Get<int>("N"));
			}

			// Create fractal child nodes under the hybrid root
			// Iterate over ALL fractals (not just enabled ones) to preserve all fractal slots
			int childNodeId = 2;
			for (int objectId = 1; objectId <= fract->size(); objectId++)
			{
				if (fract->at(objectId - 1)->IfExists("formula")
						&& fract->at(objectId - 1)->Get<int>("formula") != int(fractal::none))
				{
					InitNodeParams(childNodeId, par);

					QString formulaName = GetFormulaName(fract->at(objectId - 1)->Get<int>("formula"));
					QString prefix = NodePrefix(childNodeId);
					par->Set(prefix + "definition",
						MakeNodeDefinition(formulaName, childNodeId, enumNodeType::fractal, 1, objectId));
					CopyFormulaTransform(par, prefix, fract->at(objectId - 1));
					CopyOneFractalParams(par, prefix, objectId);
					// For old files (< v2.29), fall back to global N for formula_maxiter
					if (fileVersion < 2.29 && par->IfExists("N"))
					{
						par->Set(prefix + "formula_maxiter", par->Get<int>("N"));
					}

					// Set node enabled state based on fractal_enable_x parameter
					// Note: fractal_enable_N is resolved by TryResolveLegacyFractalParam to
					// fractal_enable in fract->at(N-1), not to top-level container.
					QString fractalEnableParam = QString("fractal_enable_%1").arg(objectId);
					bool hasParam = par->IfExists(fractalEnableParam)
													|| (fract->at(objectId - 1)->IfExists("fractal_enable"));
					bool paramValue;
					if (par->IfExists(fractalEnableParam))
					{
						paramValue = par->Get<bool>(fractalEnableParam);
					}
					else if (fract->at(objectId - 1)->IfExists("fractal_enable"))
					{
						paramValue = fract->at(objectId - 1)->Get<bool>("fractal_enable");
					}
					else
					{
						paramValue = true;
					}
					if (hasParam && !paramValue)
					{
						par->Set(prefix + "enabled", false);
					}

					childNodeId++;
				}
			}
		}
	}

	// Sort primitives by their calculation order.
	// The legacy cPrimitives::Set() used a stable bubble sort over the list
	// returned by GetListOfPrimitives() (which is alphabetical by parameter
	// name), and it read missing calculation_order as the default value 1.
	// std::stable_sort is required here to reproduce that tie-breaking:
	// primitives with equal order keep alphabetical order. A plain std::sort
	// is unstable and scrambles equal elements, which changes the operand
	// order of SUB/RevSUB boolean chains (e.g. many_booleans.fract).
	std::stable_sort(primitives.begin(), primitives.end(),
		[&](const sPrimitiveItem &a, const sPrimitiveItem &b)
		{
			const QString aOrderName = a.fullName + "_calculation_order";
			const QString bOrderName = b.fullName + "_calculation_order";
			const int orderA = par->IfExists(aOrderName) ? par->Get<int>(aOrderName) : 1;
			const int orderB = par->IfExists(bOrderName) ? par->Get<int>(bOrderName) : 1;
			return orderA < orderB;
		});

	// Find the current root node (node with parent == 0) and the maximum node ID
	int maxNodeId = 0;
	int rootNodeId = -1;
	QStringList allParams = par->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (!paramName.startsWith("node_") || !paramName.endsWith("_definition"))
		{
			continue;
		}
		const int nodeId = paramName.section('_', 1, 1).toInt();
		if (nodeId > maxNodeId)
		{
			maxNodeId = nodeId;
		}
		QStringList parts = par->Get<QString>(paramName).split(',');
		if (parts.size() >= 6 && parts[3].toInt() == 0)
		{
			rootNodeId = nodeId;
		}
	}

	// In hybrid mode with boolean mode, keep rootNodeId so primitives attach to the
	// hybrid fractal root. Only reset when NOT in boolean mode (pure hybrid without
	// boolean operations).
	// The legacy engine combined primitives sequentially with the fractal distance
	// (cPrimitives::TotalDistance), so a non-OR operator (AND/SUB/RevSUB) must be
	// applied against the fractal root: in that case keep rootNodeId and chain the
	// primitives onto it in legacy calculation order. When every primitive uses OR
	// (union), the primitives can live in their own sibling group instead, so reset
	// rootNodeId to let the first primitive become a separate root (previous behavior).
	if (hybridMode && !booleanMode && rootNodeId > 0)
	{
		bool anyNonOrPrimitive = false;
		for (const auto &primitive : primitives)
		{
			const bool hasParamInFile = primitivesWithParamInFile.contains(primitive.fullName);
			const bool isPrimitiveEnabled =
				par->IfExists(primitive.Name("enabled")) && par->Get<bool>(primitive.Name("enabled"));
			if (!hasParamInFile && !isPrimitiveEnabled)
			{
				continue;
			}
			const QString boolOpName = primitive.Name("boolean_operator");
			const int boolOp =
				par->IfExists(boolOpName) ? par->Get<int>(boolOpName) : int(primBooleanOperatorOR);
			if (boolOp != int(primBooleanOperatorOR))
			{
				anyNonOrPrimitive = true;
				break;
			}
		}
		if (!anyNonOrPrimitive)
		{
			rootNodeId = -1;
		}
	}

	// Attach each primitive to the node tree.
	// A primitive is included if it has any parameter in the file OR is currently enabled.
	// Generate unique userObjectId for primitives that don't have explicit object_id in file.
	int nextAutoObjectId = 200;
	int nextDisabledAutoObjectId = 300;
	for (const auto &primitive : primitives)
	{
		// Include primitive if it has any parameter in the file or is enabled.
		bool hasParamInFile = primitivesWithParamInFile.contains(primitive.fullName);
		bool isPrimitiveEnabled =
			par->IfExists(primitive.Name("enabled")) && par->Get<bool>(primitive.Name("enabled"));
		if (!hasParamInFile && !isPrimitiveEnabled)
		{
			continue;
		}

		int primitiveObjectId;
		bool hasExplicitObjectId = primitivesWithObjectIdInFile.contains(primitive.fullName);
		if (hasExplicitObjectId)
		{
			primitiveObjectId = primitive.objectID;
		}
		else if (isPrimitiveEnabled)
		{
			primitiveObjectId = nextAutoObjectId++;
			par->Set(primitive.Name("object_id"), primitiveObjectId);
		}
		else
		{
			primitiveObjectId = nextDisabledAutoObjectId++;
			par->Set(primitive.Name("object_id"), primitiveObjectId);
		}
		const QString primitiveName = par->IfExists(primitive.Name("name"))
																		? par->Get<QString>(primitive.Name("name"))
																		: primitive.typeName;

		// If no root node exists yet, the primitives live in their own sibling
		// group (separate from the fractal/hybrid tree). Create a booleanAdd
		// "primitives" group as the root and put the primitive inside it. This is
		// required even for a single primitive: the global primitive transform
		// (all_primitives_position / all_primitives_rotation) can only be migrated
		// onto a group by FlattenBooleanAddGroups(). Without the group, a lone
		// primitive would have no wrapper and its global transform would be lost.
		if (rootNodeId < 0)
		{
			const int primitiveGroupId = ++maxNodeId;
			InitNodeParams(primitiveGroupId, par);
			par->Set(
				NodeDefinitionParam(primitiveGroupId), MakeNodeDefinition("primitives", primitiveGroupId,
																								 enumNodeType::booleanAdd, 0, nextGroupObjectId++));
			par->Set(NodePrefix(primitiveGroupId) + "material", -1);

			const int primitiveNodeId = ++maxNodeId;
			InitNodeParams(primitiveNodeId, par);
			par->Set(NodeDefinitionParam(primitiveNodeId),
				MakeNodeDefinition(primitiveName, primitiveNodeId, enumNodeType::primitive,
					primitiveGroupId, primitiveObjectId));
			QString matParam = primitive.Name("material_id");
			if (par->IfExists(matParam))
			{
				int matId = par->Get<int>(matParam);
				if (matId > 0)
				{
					par->Set(NodePrefix(primitiveNodeId) + "material", matId);
				}
			}
			CopyPrimitiveTransform(par, primitive.fullName, NodePrefix(primitiveNodeId));
			CopyPrimitiveSmoothDECombine(par, primitive.fullName, NodePrefix(primitiveNodeId));
			if (!isPrimitiveEnabled)
			{
				par->Set(NodePrefix(primitiveNodeId) + "enabled", false);
			}
			rootNodeId = primitiveGroupId;
			continue;
		}

		// Create a boolean node wrapping the primitive and attach it to the current root.
		// For RevSUB (primBooleanOperatorRevSUB=3), the operand order is reversed:
		// primitive - oldRoot instead of oldRoot - primitive.
		// We achieve this by adjusting displayOrder: children are sorted by displayOrder
		// (ascending) then nodeId (ascending), and processed in reverse order (last = first).
		// By giving the primitive a higher displayOrder than the old root, the primitive
		// is processed first and becomes the left operand (A) in booleanSub(A, B).
		const int boolNodeId = ++maxNodeId;
		const int primitiveNodeId = ++maxNodeId;
		const int primitiveBoolOp = par->IfExists(primitive.Name("boolean_operator"))
																	? par->Get<int>(primitive.Name("boolean_operator"))
																	: int(primBooleanOperatorOR);

		InitNodeParams(boolNodeId, par);
		par->Set(NodeDefinitionParam(boolNodeId),
			MakeNodeDefinition(
				"boolean", boolNodeId, PrimitiveOpToNodeType(primitiveBoolOp), 0, nextGroupObjectId++));
		par->Set(NodePrefix(boolNodeId) + "material", -1);

		SetNodeParent(par, rootNodeId, boolNodeId);

		InitNodeParams(primitiveNodeId, par);
		par->Set(NodeDefinitionParam(primitiveNodeId),
			MakeNodeDefinition(
				primitiveName, primitiveNodeId, enumNodeType::primitive, boolNodeId, primitiveObjectId));
		QString matParam = primitive.Name("material_id");
		if (par->IfExists(matParam))
		{
			int matId = par->Get<int>(matParam);
			if (matId > 0)
			{
				par->Set(NodePrefix(primitiveNodeId) + "material", matId);
			}
		}
		CopyPrimitiveTransform(par, primitive.fullName, NodePrefix(primitiveNodeId));
		CopyPrimitiveSmoothDECombine(par, primitive.fullName, NodePrefix(primitiveNodeId));
		if (!isPrimitiveEnabled)
		{
			par->Set(NodePrefix(primitiveNodeId) + "enabled", false);
		}

		// For RevSUB (primBooleanOperatorRevSUB=3), the operand order is reversed compared
		// to standard SUB. In the old Mandelbulber2 system:
		// - SUB (2) = B-A where B=primitive, A=fractal → primitive - fractal
		// - RevSUB (3) = A-B → fractal - primitive (reversed)
		// Children are sorted by displayOrder (asc) then nodeId (asc), processed in reverse.
		// The last child in sorted order is processed first (becomes left operand A).
		// For RevSUB: old root gets displayOrder=1 (processed first → left operand = fractal),
		// primitive gets displayOrder=0 (processed second → right operand = primitive).
		// Result: booleanSub(fractal, primitive) = fractal - primitive.
		if (primitiveBoolOp == int(primBooleanOperatorRevSUB))
		{
			// Set old root's displayOrder to 1 (higher = processed first in reverse)
			QString rootDef = par->Get<QString>(NodeDefinitionParam(rootNodeId));
			QStringList rootParts = rootDef.split(',');
			if (rootParts.size() >= 6)
			{
				rootParts[5] = "1";
				par->Set(NodeDefinitionParam(rootNodeId), rootParts.join(','));
			}
			// Set primitive's displayOrder to 0 (lower = processed second in reverse)
			QString primDef = par->Get<QString>(NodeDefinitionParam(primitiveNodeId));
			QStringList primParts = primDef.split(',');
			if (primParts.size() >= 6)
			{
				primParts[5] = "0";
				par->Set(NodeDefinitionParam(primitiveNodeId), primParts.join(','));
			}
		}

		rootNodeId = boolNodeId;
	}

	// Clean up all temporary legacy params after migration
	DeleteTemporaryLegacyBooleanParams(par);
	DeleteTemporaryLegacyFractalParams(par);
	DeleteTemporaryLegacyFormulaTransformParams(fract);
	DeleteTemporaryLegacyFormulaMaterialIdParams(par);
}

// Flattens nested booleanAdd groups: finds each root booleanAdd, collects all its
// leaf descendants (fractals/primitives), and reparents them directly to a new
// booleanAdd root. Intermediate booleanAdd groups are reparented to the new root.
void cSettings::FlattenBooleanAddGroups(
	std::shared_ptr<cParameterContainer> par, int &nextGroupObjectId)
{
	struct sNodeInfo
	{
		int id;
		enumNodeType type;
		int parentId;
	};

	QStringList allParams = par->GetListOfParameters();

	// Collect all nodes and find all booleanAdd nodes
	QList<sNodeInfo> nodes;
	int maxNodeId = 0;
	QList<int> booleanAddNodes;

	for (const QString &paramName : allParams)
	{
		if (!paramName.startsWith("node_") || !paramName.endsWith("_definition")) continue;

		const int nodeId = paramName.section('_', 1, 1).toInt();
		if (nodeId > maxNodeId) maxNodeId = nodeId;

		QString def = par->Get<QString>(paramName);
		QStringList parts = def.split(',');
		if (parts.size() < 5) continue;

		int id = parts[1].toInt();
		enumNodeType type = static_cast<enumNodeType>(parts[2].toInt());
		int parentId = parts[3].toInt();

		nodes.append({id, type, parentId});

		if (type == enumNodeType::booleanAdd) booleanAddNodes.append(id);
	}

	// Helper lambda: query current children of a node from the parameter container
	auto getChildren = [&](int nodeId) -> QList<sNodeInfo>
	{
		QList<sNodeInfo> children;
		QStringList currentParams = par->GetListOfParameters();
		for (const QString &pn : currentParams)
		{
			if (!pn.startsWith("node_") || !pn.endsWith("_definition")) continue;
			int cnId = pn.section('_', 1, 1).toInt();
			QString cDef = par->Get<QString>(pn);
			QStringList cParts = cDef.split(',');
			if (cParts.size() < 5) continue;
			int cParentId = cParts[3].toInt();
			if (cParentId == nodeId)
			{
				enumNodeType cType = static_cast<enumNodeType>(cParts[2].toInt());
				children.append({cnId, cType, cParentId});
			}
		}
		return children;
	};

	// Helper lambda: update a node's parent in the nodes list
	auto updateNodeParent = [&](int nodeId, int newParentId)
	{
		for (auto &n : nodes)
		{
			if (n.id == nodeId)
			{
				n.parentId = newParentId;
				break;
			}
		}
	};

	// Helper: check if any descendant of a node is booleanSub or booleanMul.
	// Returns true if a blocking descendant exists, false otherwise.
	auto hasBlockingDescendant = [&](int nodeId) -> bool
	{
		// BFS through children
		QList<int> queue;
		QSet<int> visited;
		// Seed with direct children
		QStringList currentParams = par->GetListOfParameters();
		for (const QString &pn : currentParams)
		{
			if (!pn.startsWith("node_") || !pn.endsWith("_definition")) continue;
			int cnId = pn.section('_', 1, 1).toInt();
			QString cDef = par->Get<QString>(pn);
			QStringList cParts = cDef.split(',');
			if (cParts.size() < 5) continue;
			int cParentId = cParts[3].toInt();
			if (cParentId == nodeId)
			{
				queue.append(cnId);
			}
		}
		while (!queue.isEmpty())
		{
			int current = queue.takeFirst();
			if (visited.contains(current)) continue;
			visited.insert(current);
			// Find this node's type
			for (const auto &n : nodes)
			{
				if (n.id == current)
				{
					enumNodeType nType = n.type;
					if (nType == enumNodeType::booleanSub || nType == enumNodeType::booleanMul)
					{
						return true;
					}
					// Only recurse into booleanAdd nodes (skip fractals/primitives)
					if (nType == enumNodeType::booleanAdd)
					{
						// Find this node's children
						for (const QString &pn : currentParams)
						{
							if (!pn.startsWith("node_") || !pn.endsWith("_definition")) continue;
							int cnId = pn.section('_', 1, 1).toInt();
							QString cDef = par->Get<QString>(pn);
							QStringList cParts = cDef.split(',');
							if (cParts.size() < 5) continue;
							int cParentId = cParts[3].toInt();
							if (cParentId == current && !visited.contains(cnId))
							{
								queue.append(cnId);
							}
						}
					}
					break;
				}
			}
		}
		return false;
	};

	// Phase 1: Identify which booleanAdd nodes can be flattened and where they go.
	// Collect booleanAdd nodes that will be flattened under the same booleanSub parent.
	struct sFlattenGroup
	{
		int targetParentId;
		QList<int> boolAddIds;
	};
	QMap<int, sFlattenGroup> flattenGroups;
	QSet<int> flattenableBoolAdds;

	// Helper: recursively collect all booleanAdd descendants of a node
	auto collectBoolAddDescendants = [&](int nodeId) -> QList<int>
	{
		QList<int> result;
		QList<int> queue;
		// Seed with direct children
		QStringList currentParams = par->GetListOfParameters();
		for (const QString &pn : currentParams)
		{
			if (!pn.startsWith("node_") || !pn.endsWith("_definition")) continue;
			int cnId = pn.section('_', 1, 1).toInt();
			QString cDef = par->Get<QString>(pn);
			QStringList cParts = cDef.split(',');
			if (cParts.size() < 5) continue;
			int cParentId = cParts[3].toInt();
			if (cParentId == nodeId)
			{
				queue.append(cnId);
			}
		}
		while (!queue.isEmpty())
		{
			int current = queue.takeFirst();
			// Find this node's type
			for (const auto &n : nodes)
			{
				if (n.id == current)
				{
					enumNodeType nType = n.type;
					if (nType == enumNodeType::booleanAdd)
					{
						result.append(current);
					}
					// Only recurse into booleanAdd nodes
					if (nType == enumNodeType::booleanAdd)
					{
						for (const QString &pn : currentParams)
						{
							if (!pn.startsWith("node_") || !pn.endsWith("_definition")) continue;
							int cnId = pn.section('_', 1, 1).toInt();
							QString cDef = par->Get<QString>(pn);
							QStringList cParts = cDef.split(',');
							if (cParts.size() < 5) continue;
							int cParentId = cParts[3].toInt();
							if (cParentId == current)
							{
								queue.append(cnId);
							}
						}
					}
					break;
				}
			}
		}
		return result;
	};

	// Helper: check whether the subtree rooted at nodeId contains any node
	// of one of the wanted types. Children are queried live from the container.
	auto subtreeHasType = [&](int nodeId, std::initializer_list<enumNodeType> wanted) -> bool
	{
		QList<int> queue;
		QSet<int> visited;
		queue.append(nodeId);
		while (!queue.isEmpty())
		{
			const int current = queue.takeFirst();
			if (visited.contains(current)) continue;
			visited.insert(current);
			for (const auto &child : getChildren(current))
			{
				for (const enumNodeType w : wanted)
				{
					if (child.type == w) return true;
				}
				queue.append(child.id);
			}
		}
		return false;
	};

	for (int boolAddId : booleanAddNodes)
	{
		// Find this node's current parent from the nodes list
		int parentId = 0;
		for (const auto &n : nodes)
		{
			if (n.id == boolAddId)
			{
				parentId = n.parentId;
				break;
			}
		}

		// Check if parent is booleanSub/booleanMul (needs a wrapper)
		bool parentIsBlocking = false;
		if (parentId != 0)
		{
			for (const auto &n : nodes)
			{
				if (n.id == parentId)
				{
					if (n.type == enumNodeType::booleanSub || n.type == enumNodeType::booleanMul)
					{
						parentIsBlocking = true;
					}
					break;
				}
			}
		}

		// Check if any descendant is booleanSub/booleanMul.
		// This conservative check is only needed when the parent is a booleanSub/booleanMul
		// (phase 2 wrapper logic): flattening such a node would change the hierarchy of
		// the booleanSub operands.
		// For a booleanAdd under another booleanAdd or at root level, merging is safe even
		// when the subtree contains booleanMul/booleanSub nodes: those subtrees are moved
		// as a whole and union (min) is associative, so the semantics are preserved.
		// This is required to flatten legacy primitive chains like (((F AND b1) OR b2) OR b3)
		// into a flat union group.
		if (parentIsBlocking && hasBlockingDescendant(boolAddId))
		{
			continue;
		}

		flattenableBoolAdds.insert(boolAddId);

		if (parentIsBlocking)
		{
			// Collect ALL booleanAdd descendants recursively
			QList<int> descendants = collectBoolAddDescendants(boolAddId);
			// Add the node itself and all descendants to the flatten group
			flattenGroups[parentId].boolAddIds.append(boolAddId);
			for (int descId : descendants)
			{
				if (!flattenGroups[parentId].boolAddIds.contains(descId))
				{
					flattenGroups[parentId].boolAddIds.append(descId);
				}
			}
		}
	}

	// Phase 2: Process flatten groups - flatten booleanAdd chain under booleanSub.
	// IMPORTANT: Preserve operand order of booleanSub!
	// booleanSub(firstChild, secondChild) means firstChild - secondChild.
	// We must keep the first child of booleanSub as the first operand.
	for (auto [targetParentId, group] : flattenGroups.asKeyValueRange())
	{
		// Find the first child of the booleanSub parent (this is the first operand).
		// We need to check if the first child is part of the flatten group.
		QList<sNodeInfo> parentChildren = getChildren(targetParentId);
		int firstChildId = parentChildren.isEmpty() ? 0 : parentChildren.first().id;

		// Check if firstChild is in the flatten group
		bool firstChildIsFlattenable = flattenableBoolAdds.contains(firstChildId);

		if (firstChildIsFlattenable)
		{
			// The first child is a booleanAdd that will be flattened.
			// We need to keep it as the first child of booleanSub, but flatten its children.
			// Collect all children from ALL booleanAdd nodes in the group (EXCEPT firstChild)
			QList<int> allChildren;
			for (int boolAddId : group.boolAddIds)
			{
				if (boolAddId == firstChildId)
				{
					continue;
				}
				QList<sNodeInfo> children = getChildren(boolAddId);
				for (const auto &n : children)
				{
					allChildren.append(n.id);
				}
			}

			// Reparent all children (from non-first booleanAdd nodes) to the firstChild
			// Only reparent non-booleanAdd nodes (fractals/primitives), not other booleanAdd nodes
			for (int childId : allChildren)
			{
				// Skip booleanAdd nodes - they will be deleted
				bool isBoolAdd = false;
				for (const auto &n : nodes)
				{
					if (n.id == childId && n.type == enumNodeType::booleanAdd)
					{
						isBoolAdd = true;
						break;
					}
				}
				if (isBoolAdd) continue;
				{
					QString def = par->Get<QString>(NodeDefinitionParam(childId));
					QStringList parts = def.split(',');
					if (parts.size() >= 4)
					{
						parts[3] = QString::number(firstChildId);
						par->Set(NodeDefinitionParam(childId), parts.join(','));
						updateNodeParent(childId, firstChildId);
					}
				}
			}

			// Delete all booleanAdd nodes EXCEPT the firstChild
			for (int boolAddId : group.boolAddIds)
			{
				if (boolAddId == firstChildId)
				{
					continue;
				}
				QString prefix = NodePrefix(boolAddId);
				for (const QString &pn : allParams)
				{
					if (pn.startsWith(prefix)) par->DeleteParameter(pn);
				}
			}
		}
		else
		{
			// The first child is NOT a booleanAdd (e.g., a primitive).
			// Create a new booleanAdd wrapper for the flattened children.
			// Insert it as the SECOND child of booleanSub (after the first operand).
			const int wrapperId = ++maxNodeId;
			InitNodeParams(wrapperId, par);
			par->Set(NodeDefinitionParam(wrapperId),
				MakeNodeDefinition(
					"boolean", wrapperId, enumNodeType::booleanAdd, targetParentId, nextGroupObjectId++));
			par->Set(NodePrefix(wrapperId) + "material", -1);

			// Collect all children from all booleanAdd nodes in this group
			QList<int> allChildren;
			for (int boolAddId : group.boolAddIds)
			{
				QList<sNodeInfo> children = getChildren(boolAddId);
				for (const auto &n : children)
				{
					allChildren.append(n.id);
				}
			}

			// Reparent all children to the wrapper
			for (int childId : allChildren)
			{
				QString def = par->Get<QString>(NodeDefinitionParam(childId));
				QStringList parts = def.split(',');
				if (parts.size() >= 4)
				{
					parts[3] = QString::number(wrapperId);
					par->Set(NodeDefinitionParam(childId), parts.join(','));
					updateNodeParent(childId, wrapperId);
				}
			}

			// Delete all booleanAdd nodes in this group
			for (int boolAddId : group.boolAddIds)
			{
				QString prefix = NodePrefix(boolAddId);
				for (const QString &pn : allParams)
				{
					if (pn.startsWith(prefix)) par->DeleteParameter(pn);
				}
			}
		}
	}

	// Remove processed booleanAdd nodes from flattenableBoolAdds (they were handled in Phase 2)
	for (auto [targetParentId, group] : flattenGroups.asKeyValueRange())
	{
		for (int boolAddId : group.boolAddIds)
		{
			flattenableBoolAdds.remove(boolAddId);
		}
	}

	// Phase 3: Process remaining flattenable booleanAdd nodes (top-level or under non-blocking
	// parent).
	std::sort(booleanAddNodes.begin(), booleanAddNodes.end(), [](int a, int b) { return a > b; });

	for (int boolAddId : booleanAddNodes)
	{
		if (!flattenableBoolAdds.contains(boolAddId))
		{
			continue;
		}

		// Find this node's current parent from the nodes list
		int parentId = 0;
		for (const auto &n : nodes)
		{
			if (n.id == boolAddId)
			{
				parentId = n.parentId;
				break;
			}
		}

		if (parentId == 0)
		{
			// Top-level booleanAdd: create a replacement root
			QString boolAddDef = par->Get<QString>(NodeDefinitionParam(boolAddId));
			QStringList boolAddParts = boolAddDef.split(',');
			QString groupName = "fractals";
			// Query actual children from the container (not the static nodes list)
			QList<sNodeInfo> children = getChildren(boolAddId);
			for (const auto &n : children)
			{
				if (n.type == enumNodeType::primitive)
				{
					groupName = "primitives";
					break;
				}
			}

			const int newRootId = ++maxNodeId;
			InitNodeParams(newRootId, par);
			par->Set(NodeDefinitionParam(newRootId),
				MakeNodeDefinition(groupName, newRootId, enumNodeType::booleanAdd, 0, nextGroupObjectId++));
			par->Set(NodePrefix(newRootId) + "material", -1);

			// The group transform may only be applied when the group contains
			// exclusively objects of the matching kind. A mixed group (e.g. a
			// legacy primitive chain wrapped around the fractal root) must not
			// receive the transform, otherwise it would wrongly move objects of
			// the other kind.
			if (groupName == "fractals")
			{
				if (!subtreeHasType(boolAddId, {enumNodeType::primitive}))
				{
					par->Set(NodePrefix(newRootId) + "position", par->Get<CVector3>("fractal_position"));
					par->Set(NodePrefix(newRootId) + "rotation", par->Get<CVector3>("fractal_rotation"));
					par->Set(NodePrefix(newRootId) + "repeat", par->Get<CVector3>("repeat"));
				}
			}
			else
			{
				if (!subtreeHasType(boolAddId, {enumNodeType::fractal, enumNodeType::hybrid}))
				{
					par->Set(
						NodePrefix(newRootId) + "position", par->Get<CVector3>("all_primitives_position"));
					par->Set(
						NodePrefix(newRootId) + "rotation", par->Get<CVector3>("all_primitives_rotation"));
				}
			}

			// Reparent all direct children of this booleanAdd to the new root
			for (const auto &n : children)
			{
				QString def = par->Get<QString>(NodeDefinitionParam(n.id));
				QStringList parts = def.split(',');
				if (parts.size() >= 4)
				{
					parts[3] = QString::number(newRootId);
					par->Set(NodeDefinitionParam(n.id), parts.join(','));
					updateNodeParent(n.id, newRootId);
				}
			}

			// Delete old booleanAdd node
			QString prefix = NodePrefix(boolAddId);
			for (const QString &pn : allParams)
			{
				if (pn.startsWith(prefix)) par->DeleteParameter(pn);
			}

			continue;
		}

		// Non-root booleanAdd with non-blocking parent: reparent its children to its parent
		QList<sNodeInfo> boolAddChildren = getChildren(boolAddId);
		for (const auto &n : boolAddChildren)
		{
			QString def = par->Get<QString>(NodeDefinitionParam(n.id));
			QStringList parts = def.split(',');
			if (parts.size() >= 4)
			{
				parts[3] = QString::number(parentId);
				par->Set(NodeDefinitionParam(n.id), parts.join(','));
				updateNodeParent(n.id, parentId);
			}
		}

		// Delete this booleanAdd node
		QString prefix = NodePrefix(boolAddId);
		for (const QString &pn : allParams)
		{
			if (pn.startsWith(prefix)) par->DeleteParameter(pn);
		}
	}

	// Remove any boolean groups left without children (orphaned during migration).
	allParams = par->GetListOfParameters();
	QSet<int> allNodeIds;
	QSet<int> parentsWithChildren;
	for (const QString &paramName : allParams)
	{
		if (!paramName.startsWith("node_") || !paramName.endsWith("_definition")) continue;
		const int nodeId = paramName.section('_', 1, 1).toInt();
		allNodeIds.insert(nodeId);

		QString def = par->Get<QString>(paramName);
		QStringList parts = def.split(',');
		if (parts.size() < 4) continue;
		int parentId = parts[3].toInt();
		if (parentId != 0) parentsWithChildren.insert(parentId);
	}
	for (int nodeId : allNodeIds)
	{
		QString def = par->Get<QString>(NodeDefinitionParam(nodeId));
		QStringList parts = def.split(',');
		if (parts.size() < 3) continue;
		enumNodeType type = static_cast<enumNodeType>(parts[2].toInt());
		if ((type == enumNodeType::booleanAdd || type == enumNodeType::booleanMul
					|| type == enumNodeType::booleanSub)
				&& !parentsWithChildren.contains(nodeId))
		{
			QString prefix = NodePrefix(nodeId);
			for (const QString &pn : allParams)
			{
				if (pn.startsWith(prefix)) par->DeleteParameter(pn);
			}
		}
	}
}

void cSettings::DeleteTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary legacy boolean params that were injected during migration.
	// Remove the top-level boolean_operators flag
	if (par->IfExists("boolean_operators"))
	{
		par->DeleteParameter("boolean_operators");
	}
	// Remove per-fractal boolean_operator params
	const int maxLegacyFractals = 9;
	for (int i = 1; i < maxLegacyFractals; i++)
	{
		QString name = QString("boolean_operator_%1").arg(i);
		if (par->IfExists(name))
		{
			par->DeleteParameter(name);
		}
	}
}

void cSettings::DeleteTemporaryLegacyFractalParams(std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary per-fractal julia_mode, julia_c, fractal_constant_factor,
	// formula_maxiter and initial_waxis params. These are injected by
	// InjectTemporaryLegacyFractalParams() when reading files older than v2.35 and must not
	// survive the migration: leftover copies would be written back into the settings text and
	// make other instances (NetRender clients, queue workers) fail to decode the text, because
	// the parameter names are unknown to them. The formula_maxiter_N params are created by the
	// v2.29 migration for boolean mode files and need to be cleaned up after being migrated to
	// fractal containers.
	const int maxLegacyFractals = 9;
	for (int i = 1; i <= maxLegacyFractals; i++)
	{
		QString name = QString("fractal_constant_factor_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("julia_mode_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("julia_c_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("formula_maxiter_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
		name = QString("initial_waxis_%1").arg(i);
		if (par->IfExists(name)) par->DeleteParameter(name);
	}
}

void cSettings::DeleteTemporaryLegacyFormulaTransformParams(
	std::shared_ptr<cFractalContainer> fractPar)
{
	// Deletes the temporary legacy formula transform params from each fractal container.
	if (fractPar)
	{
		const int maxLegacyFractals = 9;
		for (int i = 0; i < maxLegacyFractals; i++)
		{
			if (fractPar->at(i)->IfExists("formula_material_id"))
				fractPar->at(i)->DeleteParameter("formula_material_id");
			if (fractPar->at(i)->IfExists("formula_position"))
				fractPar->at(i)->DeleteParameter("formula_position");
			if (fractPar->at(i)->IfExists("formula_rotation"))
				fractPar->at(i)->DeleteParameter("formula_rotation");
			if (fractPar->at(i)->IfExists("formula_repeat"))
				fractPar->at(i)->DeleteParameter("formula_repeat");
			if (fractPar->at(i)->IfExists("formula_scale"))
				fractPar->at(i)->DeleteParameter("formula_scale");
		}
	}
}

void cSettings::DeleteTemporaryLegacyFormulaMaterialIdParams(
	std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary global formula_material_id param from the top-level container.
	if (par->IfExists("formula_material_id"))
	{
		par->DeleteParameter("formula_material_id");
	}

	// Note: do NOT delete "fractal_rotation" here — sParamRender reads it from the
	// top-level container for fractal coloring / texture mapping purposes. The value
	// was migrated to per-fractal formula_rotation by Compatibility2() for the objects
	// tree, but the top-level copy is still needed by the rendering pipeline.
}

void cSettings::DeleteTemporaryLegacyPrimitiveTransformParams(
	std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary legacy primitive transform params (position/rotation/scale) that were
	// injected during migration.
	QStringList legacyTypes = GetLegacyPrimitiveTypes();
	const int maxLegacyFractals = 9;
	for (const QString &type : legacyTypes)
	{
		for (int i = 1; i <= maxLegacyFractals; i++)
		{
			// Delete position param
			QString name = QString("primitive_%1_%2_position").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete rotation param
			name = QString("primitive_%1_%2_rotation").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete scale param
			name = QString("primitive_%1_%2_scale").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete repeat param (legacy transform param)
			name = QString("primitive_%1_%2_repeat").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
		}
	}
}

void cSettings::DeleteTemporaryLegacyPrimitiveNodeParams(std::shared_ptr<cParameterContainer> par)
{
	// Deletes the temporary legacy primitive node params (enabled, material_id, calculation_order,
	// boolean_operator, smooth_de_combine_enable, smooth_de_combine_distance) that were injected
	// during decode.
	QStringList legacyTypes = GetLegacyPrimitiveTypes();
	const int maxLegacyPrimitives = 99;
	for (const QString &type : legacyTypes)
	{
		for (int i = 1; i <= maxLegacyPrimitives; i++)
		{
			// Delete material_id param
			QString name = QString("primitive_%1_%2_material_id").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete calculation_order param
			name = QString("primitive_%1_%2_calculation_order").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete boolean_operator param
			name = QString("primitive_%1_%2_boolean_operator").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete smooth_de_combine_enable param
			name = QString("primitive_%1_%2_smooth_de_combine_enable").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
			// Delete smooth_de_combine_distance param
			name = QString("primitive_%1_%2_smooth_de_combine_distance").arg(type).arg(i);
			if (par->IfExists(name))
			{
				par->DeleteParameter(name);
			}
		}
	}
}
