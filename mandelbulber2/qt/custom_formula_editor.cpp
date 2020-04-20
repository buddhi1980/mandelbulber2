/*
 * custom_formula_editor.cpp
 *
 *  Created on: 19 mar 2020
 *      Author: krzysztof
 */

#include "custom_formula_editor.h"
#include "ui_custom_formula_editor.h"
#include "src/system.hpp"
#include "src/fractal_container.hpp"
#include "src/interface.hpp"
#include "src/system_directories.hpp"
#include "src/error_message.hpp"

#include "my_line_edit.h"
#include "my_check_box.h"
#include "my_spin_box.h"
#include "my_double_spin_box.h"

#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QDebug>

cCustomFormulaEditor::cCustomFormulaEditor(QWidget *parent)
		: QWidget(parent), ui(new Ui::cCustomFormulaEditor)
{
	ui->setupUi(this);

	connect(ui->pushButton_new, &QPushButton::pressed, this, &cCustomFormulaEditor::slotNewFormula);
	connect(ui->textEdit_formula_code, &QTextEdit::textChanged, this,
		&cCustomFormulaEditor::slotTextChanged);
	connect(ui->pushButton_load_builtin, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotLoadBuiltIn);
	connect(ui->pushButton_check_syntax, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotCheckSyntax);
	connect(
		ui->pushButton_auto_format, &QPushButton::pressed, this, &cCustomFormulaEditor::slotAutoFormat);
	connect(ui->pushButton_insert_parameter, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotInsertParameter);

	CreateConversionTable();
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
	ui->textEdit_formula_code->setMinimumHeight(
		ui->textEdit_formula_code->document()->size().height());
}

void cCustomFormulaEditor::slotLoadBuiltIn()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("OpenCL programs (*.cl)"));
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
									"- Windows: Go to https://llvm.org/builds/ download and install LLVM. Make sure "
									"you have clang-format in your PATH var.\n"
									"- MacOS: When you have the brew package manager installed run: `brew install "
									"clang-format`"),
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
	QString program = "clang-format";
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
	QString program = "clang-format";
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
			listOfFoundParameters.append(match.captured(1));
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

QList<cCustomFormulaEditor::sParameterDesctiption> cCustomFormulaEditor::ConvertListOfParameters(
	const QStringList &inputList)
{
	QList<sParameterDesctiption> list;

	for (int i = 0; i < inputList.size(); i++)
	{
		QString sourceName = inputList[i];

		if (sourceName.endsWith(".x") || sourceName.endsWith(".y") || sourceName.endsWith(".z")
				|| sourceName.endsWith(".w"))
		{
			sourceName = sourceName.left(sourceName.length() - 2);
		}

		if (conversionTable.contains(sourceName))
		{
			QString parameterName = conversionTable[sourceName];
			cOneParameter parameter = gParFractal->at(0).GetAsOneParameter(parameterName);
			qDebug() << inputList[i] << parameterName;
			sParameterDesctiption parDesc;
			parDesc.parameterName = parameterName;
			parDesc.parameter = parameter;
			list.append(parDesc);
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
	while ((item = ui->formLayoutParameters->takeAt(0)) != NULL)
	{
		delete item->widget();
		delete item;
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
		ui->groupBox_parameters, &gParFractal->at(slotIndex), qInterface::write);
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
		connect(completer, SIGNAL(activated(QString)), comboBox, SLOT(onCompleterActivated(QString)));
	}

	dialog->exec();
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
