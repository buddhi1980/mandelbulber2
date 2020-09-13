/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "custom_formula_editor.h"

#include <memory>

#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QVector>

#include "ui_custom_formula_editor.h"

#include "my_check_box.h"
#include "my_double_spin_box.h"
#include "my_line_edit.h"
#include "my_spin_box.h"
#include "my_text_edit.h"

#include "src/error_message.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/nine_fractals.hpp"
#include "src/opencl_engine_render_fractal.h"
#include "src/opencl_global.h"
#include "src/render_data.hpp"
#include "src/system.hpp"
#include "src/system_directories.hpp"

cCustomFormulaEditor::cCustomFormulaEditor(QWidget *parent)
		: QWidget(parent), ui(new Ui::cCustomFormulaEditor)
{
	ui->setupUi(this);

	connect(ui->textEdit_formula_code, &QTextEdit::textChanged, this,
		&cCustomFormulaEditor::slotTextChanged);
	connect(ui->textEdit_formula_code, &cMyTextEdit::signalUpdate, this,
		&cCustomFormulaEditor::slotRebuildUI);
	connect(ui->textEdit_formula_code, &QTextEdit::cursorPositionChanged, this,
		&cCustomFormulaEditor::slotCursorMoved);

	connect(ui->pushButton_new, &QPushButton::pressed, this, &cCustomFormulaEditor::slotNewFormula);
	connect(ui->pushButton_load_builtin, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotLoadBuiltIn);
	connect(ui->pushButton_check_syntax, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotCheckSyntax);
	connect(
		ui->pushButton_auto_format, &QPushButton::pressed, this, &cCustomFormulaEditor::slotAutoFormat);
	connect(ui->pushButton_insert_parameter, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotInsertParameter);

	connect(
		ui->pushButton_save_to_file, &QPushButton::pressed, this, &cCustomFormulaEditor::slotSave);
	connect(
		ui->pushButton_load_from_file, &QPushButton::pressed, this, &cCustomFormulaEditor::slotLoad);

	connect(ui->listWidget_errors, &QListWidget::itemDoubleClicked, this,
		&cCustomFormulaEditor::slotGoToError);

	actualFormulaFileName =
		systemDirectories.GetOpenCLCustomFormulasFolder() + QDir::separator() + "custom_formula.cl";

	CreateConversionTable();

	ui->label_errors->hide();
	ui->listWidget_errors->hide();
}

cCustomFormulaEditor::~cCustomFormulaEditor()
{
	delete ui;
}

void cCustomFormulaEditor::slotNewFormula()
{
	QString code =
		"REAL4 CustomIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)\n"
		"{\n"
		"\treturn z;\n"
		"}\n";

	ui->textEdit_formula_code->setText(code);
}

void cCustomFormulaEditor::slotTextChanged()
{
	// ui->textEdit_formula_code->setMinimumHeight(
	//	ui->textEdit_formula_code->document()->size().height());
	QWidget *parentWidget = qobject_cast<QWidget *>(parent()->parent()->parent()->parent());
	int minimumHeight = parentWidget->height() * 3 / 4;
	if (minimumHeight < 400) minimumHeight = 400;
	ui->textEdit_formula_code->setMinimumHeight(minimumHeight);
}

void cCustomFormulaEditor::slotLoadBuiltIn()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("OpenCL code (*.cl)"));
	dialog.setDirectory(QDir::toNativeSeparators(
		systemDirectories.sharedDir + "formula" + QDir::separator() + "opencl"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load existing fractal formula code..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		QFile file(filename);
		if (file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream stream(&file);
			QString code = stream.readAll();
			code.replace(" * D O    N O T    E D I T    T H I S    F I L E !",
				" * F E E L   F R E E    T O    E D I T    T H I S    F I L E !");
			QRegularExpression regex("REAL4\\ (.*?)\\(REAL4\\ z");
			code.replace(regex, "REAL4 CustomIteration(REAL4 z");
			ui->textEdit_formula_code->setText(code);
			file.close();
		}
	}

	QStringList parametersInCode = CreateListOfParametersInCode();
	QList<sParameterDesctiption> list = ConvertListOfParameters(parametersInCode);
	BuildUI(list);
}

void cCustomFormulaEditor::slotAutoFormat()
{
	if (!clangFormatPresent())
	{
		cErrorMessage::showMessage(
			QObject::tr("clang-format is required for autoformat but was not detected\n\n"
									"To install clang-format:\n"
									"- Linux: Install clang-format from ypur package manager.\n"
									"- Windows: Go to https://llvm.org/builds/ \n"
									"  download and install clang-format.\n"
									"- MacOS: When you have the brew package manager installed run: \n"
									"  `brew install clang-format`\n"
									"If required, you can specify the program path under\n"
									"Preferences > OpenCL (GPU) > clang-format path.\n\n"
									"Current clang-format path is: %1")
				.arg(gPar->Get<QString>("clang_format_path")),
			cErrorMessage::warningMessage);
		return;
	}

	QString filePath =
		systemDirectories.GetOpenCLTempFolder() + QDir::separator() + "temp_format" + ".c";
	QFile qFileWrite(filePath);
	if (qFileWrite.open(QIODevice::WriteOnly))
	{
		QTextStream out(&qFileWrite);
		out << ui->textEdit_formula_code->toPlainText();
		qFileWrite.close();
	}
	QProcess process(this);
	QString program = gPar->Get<QString>("clang_format_path");
	QStringList args;
	args << "-i";
	args << "--style=file";
	args << filePath;
	process.start(program, args);
	process.waitForFinished();

	QFile qFileRead(filePath);
	if (qFileRead.open(QIODevice::ReadOnly))
	{
		QString codeFormatted;
		QTextStream sIn(&qFileRead);
		codeFormatted.append(sIn.readAll());
		ui->textEdit_formula_code->setText(codeFormatted);
	}
}

bool cCustomFormulaEditor::clangFormatPresent()
{
	QProcess process(this);
	QString program = gPar->Get<QString>("clang_format_path");
	QStringList args;
	args << "-version";
	process.start(program, args);
	process.waitForFinished();
	QString output(process.readAllStandardOutput());
	return output.contains("clang-format version");
}

void cCustomFormulaEditor::slotCheckSyntax()
{
	// create list of parameters used in the code
	QStringList parametersInCode = CreateListOfParametersInCode();
	QList<sParameterDesctiption> list = ConvertListOfParameters(parametersInCode);
	BuildUI(list);

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

#ifdef USE_OPENCL
	std::shared_ptr<sRenderData> renderData(new sRenderData);
	renderData->objectData.resize(NUMBER_OF_FRACTALS);

	std::shared_ptr<cNineFractals> fractals(new cNineFractals(gParFractal, gPar));
	std::shared_ptr<sParamRender> params(new sParamRender(gPar, &renderData->objectData));

	CreateMaterialsMap(gPar, &renderData.get()->materials, false, true, false);
	renderData->ValidateObjects();

	gOpenCl->openClEngineRenderFractal->Lock();
	gOpenCl->openClEngineRenderFractal->SetParameters(
		gPar, gParFractal, params, fractals, renderData, true);

	QString compilerOutput;
	if (gOpenCl->openClEngineRenderFractal->LoadSourcesAndCompile(gPar, &compilerOutput))
	{
	}
	gOpenCl->openClEngineRenderFractal->ReleaseMemory();
	gOpenCl->openClEngineRenderFractal->Unlock();

	QRegularExpression regex(
		QString("custom%1.cl\\:(\\d+)\\:(\\d+)\\: error\\:\\ (.*)\\n").arg(slotIndex));
	QRegularExpressionMatchIterator regIterator = regex.globalMatch(compilerOutput);
	ui->listWidget_errors->clear();
	ui->listWidget_errors->hide();
	ui->label_errors->hide();
	while (regIterator.hasNext())
	{
		ui->listWidget_errors->show();
		ui->label_errors->show();
		QRegularExpressionMatch match = regIterator.next();
		int row = match.captured(1).toInt();
		int col = match.captured(2).toInt();
		QString error = match.captured(3);

		ui->listWidget_errors->addItem(QString("line %1, col %2: %3").arg(row).arg(col).arg(error));
		ui->listWidget_errors->adjustSize();
	}
#endif
}

QStringList cCustomFormulaEditor::CreateListOfParametersInCode()
{
	QString code = ui->textEdit_formula_code->toPlainText();
	QRegularExpression regex("fractal->(.*?)[^a-zA-Z0-9_.]");

	QStringList listOfFoundParameters;

	bool found = false;
	int lastPosition = 0;
	do
	{
		QRegularExpressionMatch match = regex.match(code, lastPosition);
		found = match.hasMatch();
		if (found)
		{
			lastPosition = match.capturedEnd();
			QString parameterName = match.captured(1);
			RemoveVectorComponent(parameterName);
			listOfFoundParameters.append(parameterName);
		}
	} while (found);

	listOfFoundParameters.removeDuplicates();
	return listOfFoundParameters;
}

void cCustomFormulaEditor::CreateConversionTable()
{
	QString fileName(systemDirectories.sharedDir + "data/parameterNames.txt");
	QFile file(fileName);
	QStringList conversionList;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{

		QTextStream textStream(&file);

		while (!textStream.atEnd())
			conversionList << textStream.readLine();

		file.close();
	}
	else
	{
		qCritical()
			<< "cCustomFormulaEditor::ConvertListOfParameters(): cannot open file with parameter names!";
	}

	for (int i = 0; i < conversionList.size(); i++)
	{
		if (conversionList[i].length() > 0)
		{
			QStringList split = conversionList[i].split(" ");
			if (split.length() == 3)
			{
				QString parameterInCode = split[0];

				if (!conversionTable.contains(parameterInCode))
				{
					conversionTable.insert(parameterInCode, split[2]);
					parameterTypes.insert(split[2], split[1]);
				}
			}
			else
			{
				qCritical() << "cCustomFormulaEditor::ConvertListOfParameters(): error in conversion list: "
										<< conversionList[i];
			}
		}
	}
}

void cCustomFormulaEditor::RemoveVectorComponent(QString &sourceName)
{
	if (sourceName.endsWith(".x") || sourceName.endsWith(".y") || sourceName.endsWith(".z")
			|| sourceName.endsWith(".w"))
	{
		sourceName = sourceName.left(sourceName.length() - 2);
	}
}

QList<cCustomFormulaEditor::sParameterDesctiption> cCustomFormulaEditor::ConvertListOfParameters(
	const QStringList &inputList)
{
	QList<sParameterDesctiption> list;

	for (int i = 0; i < inputList.size(); i++)
	{
		QString sourceName = inputList[i];

		RemoveVectorComponent(sourceName);

		if (conversionTable.contains(sourceName))
		{
			QString parameterName = conversionTable[sourceName];
			if (parameterName != "none")
			{
				cOneParameter parameter = gParFractal->at(0)->GetAsOneParameter(parameterName);
				sParameterDesctiption parDesc;
				parDesc.parameterName = parameterName;
				parDesc.parameter = parameter;
				list.append(parDesc);
			}
		}
		else
		{
			qWarning() << "cCustomFormulaEditor::ConvertListOfParameters(): Unknown parameter: "
								 << sourceName;
		}
	}

	return list;
}

void cCustomFormulaEditor::BuildUI(const QList<sParameterDesctiption> &listOfParameters)
{
	// delete all existing widgets
	QLayoutItem *item;
	while (ui->formLayoutParameters->count() > 0)
	{
		item = ui->formLayoutParameters->takeAt(0);
		if (item)
		{
			delete item->widget();
			delete item;
		}
	}

	for (int i = 0; i < listOfParameters.size(); i++)
	{
		cOneParameter parameter = listOfParameters[i].parameter;
		enumVarType varType = parameter.GetValueType();
		enumMorphType morphType = parameter.GetMorphType();

		if (varType == typeVector3)
		{
			for (int axis = 0; axis < 3; axis++)
			{
				QString sAxis;
				switch (axis)
				{
					case 0: sAxis = "x"; break;
					case 1: sAxis = "y"; break;
					case 2: sAxis = "z"; break;
					default: break;
				}

				switch (morphType)
				{
					case morphAkimaAngle:
					case morphCatMullRomAngle:
					case morphLinearAngle:
					{
						auto *newWidget = new MyDoubleSpinBox(this);
						newWidget->setObjectName(
							QString("spinboxd3_%1_%2").arg(listOfParameters[i].parameterName).arg(sAxis));
						newWidget->setMinimum(-18000.0);
						newWidget->setMaximum(18000.0);
						newWidget->setDecimals(6);
						newWidget->setSingleStep(0.1);
						ui->formLayoutParameters->addRow(
							listOfParameters[i].parameterName + " " + sAxis, newWidget);
						break;
					}
					default:
					{
						auto *newWidget = new MyLineEdit(this);
						newWidget->setObjectName(
							QString("vect3_%1_%2").arg(listOfParameters[i].parameterName).arg(sAxis));
						ui->formLayoutParameters->addRow(
							listOfParameters[i].parameterName + " " + sAxis, newWidget);
						break;
					}
				}
			}
		}
		else if (varType == typeVector4)
		{
			for (int axis = 0; axis < 4; axis++)
			{
				QWidget *newWidget = new MyLineEdit(this);
				QString sAxis;
				switch (axis)
				{
					case 0: sAxis = "x"; break;
					case 1: sAxis = "y"; break;
					case 2: sAxis = "z"; break;
					case 3: sAxis = "w"; break;
					default: break;
				}
				newWidget->setObjectName(
					QString("vect4_%1_%2").arg(listOfParameters[i].parameterName).arg(sAxis));
				ui->formLayoutParameters->addRow(
					listOfParameters[i].parameterName + " " + sAxis, newWidget);
			}
		}
		else if (varType == typeInt)
		{
			MySpinBox *newWidget = new MySpinBox(this);
			newWidget->setObjectName(QString("spinboxInt_%1").arg(listOfParameters[i].parameterName));
			int min = parameter.Get<int>(parameterContainer::valueMin);
			int max = parameter.Get<int>(parameterContainer::valueMax);
			if (min == max)
			{
				min = 0;
				max = 1000;
			}
			newWidget->setMinimum(min);
			newWidget->setMaximum(max);
			ui->formLayoutParameters->addRow(listOfParameters[i].parameterName, newWidget);
		}
		else if (varType == typeBool)
		{
			MyCheckBox *newWidget = new MyCheckBox(this);
			newWidget->setObjectName(QString("checkBox_%1").arg(listOfParameters[i].parameterName));
			newWidget->setText(listOfParameters[i].parameterName);
			ui->formLayoutParameters->addRow(newWidget);
		}
		else
		{
			QWidget *newWidget = new MyLineEdit(this);
			newWidget->setObjectName(QString("edit_%1").arg(listOfParameters[i].parameterName));
			ui->formLayoutParameters->addRow(listOfParameters[i].parameterName, newWidget);
		}
		QFrame *line = new QFrame();
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		ui->formLayoutParameters->addRow(line);
	}
	SynchronizeInterfaceWindow(
		ui->groupBox_parameters, gParFractal->at(slotIndex), qInterface::write);
}

void cCustomFormulaEditor::slotInsertParameter()
{
	auto *dialog = new QInputDialog(this);
	dialog->setInputMode(QInputDialog::TextInput);
	QStringList listOfParameters = conversionTable.values();
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterWithType = parameterTypes[listOfParameters[i]] + " " + listOfParameters[i];
		listOfParameters[i] = parameterWithType;
	}
	listOfParameters.sort(Qt::CaseInsensitive);
	dialog->setComboBoxItems(listOfParameters);

	QComboBox *comboBox = dialog->findChild<QComboBox *>();
	if (comboBox)
	{
		comboBox->setFocusPolicy(Qt::StrongFocus);
		comboBox->setEditable(true);
		comboBox->setMaxVisibleItems(30);

		// add a filter model to filter matching items
		auto pFilterModel = new QSortFilterProxyModel(comboBox);
		pFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
		pFilterModel->setSourceModel(comboBox->model());

		// add a completer, which uses the filter model
		auto completer = new QCompleter(pFilterModel, this);
		// always show all (filtered) completions
		completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
		comboBox->setCompleter(completer);

		// connect signals
		connect(comboBox->lineEdit(), &QLineEdit::textEdited, pFilterModel,
			&QSortFilterProxyModel::setFilterFixedString);
	}

	dialog->setWindowTitle(tr("Inserting parameter"));
	dialog->setLabelText("Select parameter to insert");

	dialog->exec();
	if (dialog->result() == QDialog::Accepted)
	{
		QString selected = dialog->textValue();
		QStringList split = selected.split(' ');

		if (split.size() == 2)
		{
			QStringList sourceParameterNames = conversionTable.keys(split[1]);

			if (!sourceParameterNames.isEmpty())
			{
				QTextCursor text_cursor = QTextCursor(ui->textEdit_formula_code->textCursor());
				text_cursor.insertText(QString("fractal->") + sourceParameterNames[0]);
			}
		}
	}
}

void cCustomFormulaEditor::slotSave()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("OpenCL code (*.cl)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(actualFormulaFileName).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(actualFormulaFileName).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Saving custom formula code..."));
	dialog.setDefaultSuffix("cl");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString fileName = QDir::toNativeSeparators(filenames.first());
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			file.write(ui->textEdit_formula_code->toPlainText().toUtf8());
			actualFormulaFileName = fileName;
			file.close();
		}
	}
};

void cCustomFormulaEditor::slotLoad()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("OpenCL code (*.cl)"));
	dialog.setDirectory(QDir::toNativeSeparators(QFileInfo(actualFormulaFileName).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(QFileInfo(actualFormulaFileName).completeBaseName()));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load custom fractal formula code..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		QFile file(filename);
		if (file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream stream(&file);
			QString code = stream.readAll();
			ui->textEdit_formula_code->setText(code);
			file.close();
		}
	}

	QStringList parametersInCode = CreateListOfParametersInCode();
	QList<sParameterDesctiption> list = ConvertListOfParameters(parametersInCode);
	BuildUI(list);
};

void cCustomFormulaEditor::slotRebuildUI()
{
	// create list of parameters used in the code
	QStringList parametersInCode = CreateListOfParametersInCode();
	QList<sParameterDesctiption> list = ConvertListOfParameters(parametersInCode);
	BuildUI(list);
}

void cCustomFormulaEditor::slotCursorMoved()
{
	int row = ui->textEdit_formula_code->textCursor().blockNumber();
	int col = ui->textEdit_formula_code->textCursor().columnNumber();
	ui->label_lineColumn->setText(tr("line %1, column %2").arg(row + 1).arg(col + 1));
}

void cCustomFormulaEditor::slotGoToError(QListWidgetItem *item)
{
	QString error = item->text();
	QRegularExpression regex(QString("line (\\d+),\\ col\\ (\\d+)\\:"));
	QRegularExpressionMatch match = regex.match(error);
	if (match.hasMatch())
	{
		int row = match.captured(1).toInt() - 1;
		int col = match.captured(2).toInt() - 1;
		ui->textEdit_formula_code->setFocus();
		QTextBlock block = ui->textEdit_formula_code->document()->findBlockByLineNumber(row);
		QTextCursor cursor(block); // ln-1 because line number starts from 0
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, col);
		ui->textEdit_formula_code->setTextCursor(cursor);
	}
}
