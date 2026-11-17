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
 * cObjectsTreeMigrator - migration of the legacy flat settings format to the objects tree
 *
 * Settings files up to v2.34 stored fractals and primitives as flat parameter lists
 * (formula_N, primitive_type_N_*, boolean_operator_N). This class rebuilds them as the
 * node hierarchy used since v2.35: it creates node definitions, maps the legacy boolean
 * operators onto node types, copies the formula and primitive transforms onto the nodes
 * and flattens nested booleanAdd groups.
 *
 * The class owns the whole migration: cSettings only feeds it the containers and the
 * sLegacyMigrationContext, and calls the helpers which have to run while an old file is
 * still being decoded (the temporary legacy parameters and the per-fractal name resolution).
 */

#ifndef MANDELBULBER2_SRC_OBJECTS_TREE_MIGRATOR_HPP_
#define MANDELBULBER2_SRC_OBJECTS_TREE_MIGRATOR_HPP_

#include <memory>

#include <QList>
#include <QString>
#include <QStringList>

#include "object_node_type.h"

// forward declarations
class cParameterContainer;
class cFractalContainer;

/**
 * Data from the settings file which the migration needs on top of the parameter containers.
 * Both members refer to data owned by the cSettings instance which performs the migration,
 * so the context stays valid for the whole duration of the cObjectsTreeMigrator object.
 */
struct sLegacyMigrationContext
{
	sLegacyMigrationContext(const QString &_rawSettingsText, double _fileVersion)
			: rawSettingsText(_rawSettingsText), fileVersion(_fileVersion)
	{
	}

	// verbatim settings text, scanned to tell "parameter was in the file" from
	// "parameter was injected with a default value during decode"
	const QString &rawSettingsText;

	// version declared by the file header; gates the pre-v2.29 and pre-v2.35 branches
	double fileVersion;
};

class cObjectsTreeMigrator
{
public:
	explicit cObjectsTreeMigrator(const sLegacyMigrationContext &_context);

	// Recognizes the temporary legacy parameter names (per-fractal variants and other
	// names which only exist while an old file is being migrated). Used by
	// cSettings::DecodeOneLine() to ignore them in files which are not migrated.
	static bool IsTemporaryLegacyParamName(const QString &name);

	// Reserve the temporary legacy parameters in the containers before decoding starts,
	// so that the decode loop has somewhere to store the values read from an old file.
	static void InjectTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par);
	static void InjectTemporaryLegacyFractalParams(std::shared_ptr<cParameterContainer> par);
	static void InjectTemporaryLegacyFormulaTransformParams(
		std::shared_ptr<cFractalContainer> fractPar);
	static void InjectTemporaryLegacyFormulaMaterialIdParams(
		std::shared_ptr<cParameterContainer> par);
	static void InjectTemporaryLegacyPrimitiveTransformParams(
		std::shared_ptr<cParameterContainer> par, int primitiveIndex);
	static void InjectTemporaryLegacyPrimitiveNodeParams(
		std::shared_ptr<cParameterContainer> par, const QString &primitiveFullName);

	// Resolves a legacy flat per-fractal name (e.g. "formula_2") into the base parameter
	// name and the fractal container it belongs to. Returns false if the line is not such
	// a legacy name. resolvedLine / targetContainer are output only when true is returned.
	static bool TryResolveLegacyFractalParam(const QString &decodeLine,
		std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fractPar,
		QString &resolvedLine, std::shared_ptr<cParameterContainer> &targetContainer);

	// Builds the node hierarchy out of the legacy flat parameters. Needs the migration
	// context: the raw settings text distinguishes explicit values from injected defaults
	// and the file version selects the legacy semantics to emulate.
	void MigrateToObjectsTree(std::shared_ptr<cParameterContainer> par,
		std::shared_ptr<cFractalContainer> fract, int &nextGroupObjectId);

	// Removes nested booleanAdd groups: their leaves are reparented onto a single new
	// booleanAdd root, which produces the same geometry with a simpler tree.
	static void FlattenBooleanAddGroups(
		std::shared_ptr<cParameterContainer> par, int &nextGroupObjectId);

	// Primitive params which are migrated into node params have to be removed once the
	// migration is done, also when the file turns out to need no migration at all.
	static void DeleteTemporaryLegacyPrimitiveTransformParams(
		std::shared_ptr<cParameterContainer> par);
	static void DeleteTemporaryLegacyPrimitiveNodeParams(std::shared_ptr<cParameterContainer> par);

private:
	static QStringList GetLegacyPrimitiveTypes();

	// Moves the flat per-fractal parameters into the per-formula containers.
	static void MigrateLegacyParamsToFractal(
		std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> fract);

	// Drops the temporary parameters injected before decoding once they are consumed.
	static void DeleteTemporaryLegacyBooleanParams(std::shared_ptr<cParameterContainer> par);
	static void DeleteTemporaryLegacyFractalParams(std::shared_ptr<cParameterContainer> par);
	static void DeleteTemporaryLegacyFormulaTransformParams(
		std::shared_ptr<cFractalContainer> fractPar);
	static void DeleteTemporaryLegacyFormulaMaterialIdParams(
		std::shared_ptr<cParameterContainer> par);

	// node hierarchy helpers
	static QString GetFormulaName(int formulaEnum);
	static enumNodeType ToNodeType(int boolOp);
	static enumNodeType PrimitiveOpToNodeType(int boolOp);
	static QString MakeNodeDefinition(const QString &formulaName, int nodeId, enumNodeType type,
		int parentId, int objectId, int displayOrder = 0);
	static QString NodePrefix(int nodeId);
	static QString NodeDefinitionParam(int nodeId);
	static void SetNodeParent(std::shared_ptr<cParameterContainer> par, int nodeId, int parentId);
	static void CopyFormulaTransform(std::shared_ptr<cParameterContainer> par, const QString &prefix,
		std::shared_ptr<cParameterContainer> fracPar);
	static void CopyPrimitiveTransform(std::shared_ptr<cParameterContainer> par,
		const QString &primitiveFullName, const QString &prefix);
	static void CopyPrimitiveSmoothDECombine(std::shared_ptr<cParameterContainer> par,
		const QString &primitiveFullName, const QString &prefix);
	static void CopyOneFractalParams(
		std::shared_ptr<cParameterContainer> par, const QString &prefix, int formulaIndex);
	static void CopyFractalParams(std::shared_ptr<cParameterContainer> par, const QString &prefix);
	static QList<int> GetEnabledFractals(std::shared_ptr<cFractalContainer> fract);
	static bool GetFractalEnableFlag(std::shared_ptr<cParameterContainer> par,
		std::shared_ptr<cFractalContainer> fract, int fractalIndex);

	sLegacyMigrationContext context;
};

#endif /* MANDELBULBER2_SRC_OBJECTS_TREE_MIGRATOR_HPP_ */
