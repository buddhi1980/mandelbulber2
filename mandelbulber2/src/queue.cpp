/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * cQueue - class to manage rendering queue
 */

#include "queue.hpp"

#include "ui_dock_queue.h"

#include "animation_frames.hpp"
#include "cimage.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "parameters.hpp"
#include "render_queue.hpp"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "settings.hpp"
#include "system.hpp"

#include "qt/dock_queue.h"
#include "qt/preview_file_dialog.h"
#include "qt/thumbnail_widget.h"

cQueue *gQueue = nullptr;

cQueue::cQueue(cInterface *_interface, const QString &_queueListFileName,
	const QString &_queueFolder, QObject *parent)
		: QObject(parent), mainInterface(_interface)
{
	// initializes queue and create necessary files and folders
	queueListFileName = _queueListFileName;
	queueFolder = _queueFolder;

	if (!QFile::exists(queueFolder) && !CreateFolder(queueFolder))
	{
		throw QString("cannot init queueListFileName folder to: " + queueFolder);
	}

	if (!QFile::exists(queueListFileName))
	{
		QFile file(queueListFileName);
		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&file);
			stream << "#\n# Mandelbulber queue file\n#\n";
			file.close();
		}
		else
			throw QString("cannot init queueListFileName to: " + queueListFileName);
	}

	// watch queue folder and the queue file in the filesystem
	queueFileWatcher.addPath(queueListFileName);
	queueFolderWatcher.addPath(queueFolder);
	QApplication::connect(&queueFileWatcher, SIGNAL(fileChanged(const QString &)), this,
		SLOT(queueFileChanged(const QString &)));
	QApplication::connect(&queueFolderWatcher, SIGNAL(directoryChanged(const QString &)), this,
		SLOT(queueFolderChanged(const QString &)));

	UpdateListFromQueueFile();
	UpdateListFromFileSystem();

	image = new cImage(200, 200);

	if (mainInterface->mainWindow)
	{
		ui = mainInterface->mainWindow->GetWidgetDockQueue()->GetUi();
		// Queue
		QApplication::connect(ui->pushButton_queue_add_current_settings, SIGNAL(clicked()), this,
			SLOT(slotQueueAddCurrentSettings()));
		QApplication::connect(
			ui->pushButton_queue_add_from_file, SIGNAL(clicked()), this, SLOT(slotQueueAddFromFile()));
		QApplication::connect(
			ui->pushButton_queue_add_orphaned, SIGNAL(clicked()), this, SLOT(slotQueueAddOrphaned()));
		QApplication::connect(ui->pushButton_queue_remove_orphaned, SIGNAL(clicked()), this,
			SLOT(slotQueueRemoveOrphaned()));
		QApplication::connect(
			ui->pushButton_queue_render_queue, SIGNAL(clicked()), this, SLOT(slotQueueRender()));
		QApplication::connect(
			ui->pushButton_queue_stop_rendering, SIGNAL(clicked()), this, SLOT(slotStopRequest()));
		QApplication::connect(ui->checkBox_show_queue_thumbnails, SIGNAL(stateChanged(int)), this,
			SLOT(slotShowQueueThumbsChanges(int)));

		QApplication::connect(this, SIGNAL(queueChanged()), this, SLOT(slotQueueListUpdate()));
		QApplication::connect(this, SIGNAL(queueChanged(int)), this, SLOT(slotQueueListUpdate(int)));
		QApplication::connect(
			this, SIGNAL(queueChanged(int, int)), this, SLOT(slotQueueListUpdate(int, int)));

		renderedImageWidget = mainInterface->mainWindow->GetWidgetDockQueue()->GetRenderedImageWidget();
		image->CreatePreview(1.0, 400, 300, renderedImageWidget);
		renderedImageWidget->setMinimumSize(image->GetPreviewWidth(), image->GetPreviewHeight());
		renderedImageWidget->AssignImage(image);

		emit queueChanged();
	}
	else
	{
		renderedImageWidget = nullptr;
	}

	stopRequest = false;
}

cQueue::~cQueue()
{
	delete image;
}

void cQueue::Append(const QString &filename, enumRenderType renderType)
{
	// add new fractal to queue
	if (QFileInfo(filename).suffix() == QString("fract"))
	{
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		if (parSettings.LoadFromFile(filename))
		{
			QString filenameQueue = "queue_" + QFileInfo(filename).completeBaseName() + "_"
															+ parSettings.GetHashCode() + ".fract";
			QString completeFileName = queueFolder + QDir::separator() + filenameQueue;
			parSettings.SaveToFile(completeFileName);
			AddToList(structQueueItem(completeFileName, renderType));
		}
	}
}

void cQueue::Append(enumRenderType renderType)
{
	// add current settings to queue
	Append(gPar, gParFractal, gAnimFrames, gKeyframes, renderType);
}

void cQueue::Append(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames,
	cKeyframes *keyframes, enumRenderType renderType)
{
	// add settings to queue
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(par, fractPar, frames, keyframes);
	QString filename = "queue_" + QFileInfo(systemData.lastSettingsFile).completeBaseName() + "_"
										 + parSettings.GetHashCode() + ".fract";
	QString completeFileName = queueFolder + QDir::separator() + filename;
	parSettings.SaveToFile(completeFileName);
	AddToList(structQueueItem(completeFileName, renderType));
}

void cQueue::AppendList(const QString &filename)
{
	// add all entries from list given with filename to current list
	// qDebug() << "AppendList: " << filename;
	if (QFileInfo(filename).suffix() == QString("fractlist"))
	{
		mutex.lock();
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream in(&file);
			while (!in.atEnd())
			{
				QString line = in.readLine().trimmed();
				if (line.startsWith("#") || line == "") continue;
				QRegularExpression reType("^(.*?\\.fract)\\s*(STILL|KEYFRAME|FLIGHT)?$");
				QRegularExpressionMatch matchType = reType.match(line);
				if (matchType.hasMatch())
				{
					structQueueItem queueItem =
						structQueueItem(matchType.captured(1), GetTypeEnum(matchType.captured(2)));
					if (!queueListFromFile.contains(queueItem)) queueListFromFile << queueItem;
				}
				else
					qWarning() << "wrong format in line: " << line;
			}
			file.close();
		}
		mutex.unlock();
		StoreList();
	}
}

void cQueue::AppendFolder(const QString &filename)
{
	// add all entries from folder given with filename to current list
	QDir fractDir = QDir(filename);
	if (fractDir.exists())
	{
		QStringList fractFileExtensions;
		fractFileExtensions << "*.fract";
		fractDir.setNameFilters(fractFileExtensions);
		QStringList fractFiles = fractDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
		for (int i = 0; i < fractFiles.size(); i++)
		{
			mutex.lock();
			structQueueItem queueItem =
				structQueueItem(filename + QDir::separator() + fractFiles.at(i), queue_STILL);
			if (!queueListFromFile.contains(queueItem)) queueListFromFile << queueItem;
			mutex.unlock();
		}
		StoreList();
	}
}

bool cQueue::Get()
{
	// get next fractal from queue into global scope containers
	return Get(gPar, gParFractal, gAnimFrames, gKeyframes);
}

bool cQueue::Get(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames,
	cKeyframes *keyframes)
{
	// get next fractal from queue
	structQueueItem queueItem = GetNextFromList();
	if (queueItem.filename == "") return false; // reached end of list
	cSettings parSettings(cSettings::formatCondensedText);

	if (parSettings.LoadFromFile(queueItem.filename))
	{
		return parSettings.Decode(par, fractPar, frames, keyframes);
	}
	return false;
}

QStringList cQueue::RemoveOrphanedFiles()
{
	// find and delete files which are not on the list
	mutex.lock();
	QStringList removeList;
	for (int i = 0; i < queueListFileSystem.size(); i++)
	{
		structQueueItem queueItem = structQueueItem(queueListFileSystem.at(i), queue_STILL);
		if (!queueListFromFile.contains(queueItem))
		{
			removeList << queueListFileSystem[i];
			RemoveFromFileSystem(queueListFileSystem[i]);
		}
	}
	// qDebug() << "remove orphaned files " << removeList.size() << " total\n" << removeList;
	mutex.unlock();
	return removeList;
}

QStringList cQueue::AddOrphanedFilesToList()
{
	// add orphaned files from queue folder to the end of the list
	mutex.lock();
	// qDebug() << "add orphaned";
	QStringList appendList;
	for (int i = 0; i < queueListFileSystem.size(); i++)
	{
		structQueueItem queueItem = structQueueItem(queueListFileSystem.at(i), queue_STILL);
		if (!queueListFromFile.contains(queueItem))
		{
			appendList << queueListFileSystem.at(i);
			queueListFromFile << queueItem;
		}
	}
	mutex.unlock();

	if (appendList.size() > 0)
	{
		StoreList();
	}
	// qDebug() << "add orphaned files " << appendList.size() << " total\n" << appendList;
	return appendList;
}

cQueue::structQueueItem cQueue::GetNextFromList()
{
	// gives next filename
	mutex.lock();
	if (queueListFromFile.size() > 0)
	{
		cQueue::structQueueItem item = queueListFromFile.at(0);
		mutex.unlock();
		return item;
	}
	mutex.unlock();
	return structQueueItem("", queue_STILL);
}

void cQueue::AddToList(const structQueueItem &queueItem)
{
	// add filename to the end of list
	mutex.lock();
	if (queueListFromFile.contains(queueItem))
	{
		qWarning() << "Entry already exists";
		mutex.unlock();
		return;
	}
	queueListFromFile << queueItem;
	mutex.unlock();
	StoreList();
}

void cQueue::SwapQueueItem(int i, int j)
{
	mutex.lock();
	if (i >= 0 && j >= 0 && i < queueListFileName.size() - 1 && j < queueListFileName.size() - 1)
		queueListFromFile.swap(i, j);
	mutex.unlock();
	StoreList();
}

void cQueue::RemoveQueueItem(int i)
{
	mutex.lock();
	structQueueItem item = queueListFromFile.at(i);
	mutex.unlock();
	RemoveQueueItem(item);
}

void cQueue::RemoveQueueItem(const structQueueItem queueItem)
{
	// remove queue item from list and filesystem
	RemoveFromList(queueItem);
	// check if fract file is still on the list (one file with different renderTypes)
	mutex.lock();
	for (int i = 0; i < queueListFromFile.size(); i++)
	{
		if (queueListFromFile.at(i).filename == queueItem.filename)
		{
			mutex.unlock();
			return;
		}
	}
	mutex.unlock();
	RemoveFromFileSystem(queueItem.filename);
}

void cQueue::UpdateQueueItemType(int i, enumRenderType renderType)
{
	mutex.lock();
	queueListFromFile[i].renderType = renderType;
	mutex.unlock();
	StoreList();
}

bool cQueue::ValidateEntry(const QString &filename)
{
	// checks if file exists and it is a proper fractal file
	if (QFileInfo(filename).suffix() == QString("fract"))
	{
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		return parSettings.LoadFromFile(filename);
	}
	return false;
}

void cQueue::RemoveFromList(const structQueueItem &queueItem)
{
	mutex.lock();
	// remove queue item if it is on the list
	queueListFromFile.removeAll(queueItem);
	mutex.unlock();
	StoreList();
}

void cQueue::StoreList()
{
	mutex.lock();
	QFile file(queueListFileName);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream << "#\n# Mandelbulber queue file\n#\n";
		for (int i = 0; i < queueListFromFile.size(); i++)
		{
			stream << queueListFromFile.at(i).filename << " "
						 << GetTypeText(queueListFromFile.at(i).renderType) << endl;
		}
	}
	file.close();
	mutex.unlock();
}

void cQueue::RemoveFromFileSystem(const QString &filename) const
{
	// remove queue file from filesystem
	if (filename.startsWith(queueFolder + QDir::separator()))
	{
		QFile::remove(filename);
	}
}

cQueue::enumRenderType cQueue::GetTypeEnum(const QString &queueText)
{
	enumRenderType renderType;
	if (queueText == "STILL")
		renderType = queue_STILL;
	else if (queueText == "KEYFRAME")
		renderType = queue_KEYFRAME;
	else if (queueText == "FLIGHT")
		renderType = queue_FLIGHT;
	else
		renderType = queue_STILL;
	return renderType;
}

QString cQueue::GetTypeText(enumRenderType queueType)
{
	switch (queueType)
	{
		case cQueue::queue_STILL: return "STILL";
		case cQueue::queue_FLIGHT: return "FLIGHT";
		case cQueue::queue_KEYFRAME: return "KEYFRAME";
	}
	return "STILL";
}

QString cQueue::GetTypeName(enumRenderType queueType)
{
	switch (queueType)
	{
		case cQueue::queue_STILL: return tr("STILL");
		case cQueue::queue_FLIGHT: return tr("FLIGHT");
		case cQueue::queue_KEYFRAME: return tr("KEYFRAME");
	}
	return tr("STILL");
}

QString cQueue::GetTypeColor(enumRenderType queueType)
{
	switch (queueType)
	{
		case cQueue::queue_STILL: return "darkgrey";
		case cQueue::queue_FLIGHT: return "green";
		case cQueue::queue_KEYFRAME: return "darkblue";
	}
	return "red";
}

void cQueue::queueFileChanged(const QString &path)
{
	// qDebug() << "queueFileChanged";
	if (path == queueListFileName)
	{
		UpdateListFromQueueFile();
	}
}

void cQueue::queueFolderChanged(const QString &path)
{
	// qDebug() << "queueFolderChanged";
	if (path == queueFolder)
	{
		UpdateListFromFileSystem();
	}
}

void cQueue::UpdateListFromQueueFile()
{
	mutex.lock();
	queueListFromFile.clear();
	// returns list of fractals to render
	QFile file(queueListFileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();
			if (line.startsWith("#") || line == "") continue;
			QRegularExpression reType("^(.*?\\.fract)\\s*(STILL|KEYFRAME|FLIGHT)?$");
			QRegularExpressionMatch matchType = reType.match(line);
			if (matchType.hasMatch())
			{
				queueListFromFile << structQueueItem(
					matchType.captured(1), GetTypeEnum(matchType.captured(2)));
			}
			else
				qWarning() << "wrong format in line: " << line;
		}
		file.close();
	}
	mutex.unlock();
	emit queueChanged();
}

void cQueue::UpdateListFromFileSystem()
{
	mutex.lock();
	queueListFileSystem = QDir(queueFolder).entryList(QDir::NoDotAndDotDot | QDir::Files);
	// make paths absolute
	for (int i = 0; i < queueListFileSystem.size(); i++)
	{
		queueListFileSystem[i] = queueFolder + QDir::separator() + queueListFileSystem.at(i);
	}
	mutex.unlock();
}

void cQueue::RenderQueue() const
{
	QThread *thread = new QThread; // deleted by deleteLater()
	cRenderQueue *renderQueue = new cRenderQueue(image, renderedImageWidget);
	renderQueue->moveToThread(thread);
	renderQueue->setObjectName("Queue");
	connect(thread, SIGNAL(started()), renderQueue, SLOT(slotRenderQueue()));
	connect(renderQueue, SIGNAL(finished()), renderQueue, SLOT(deleteLater()));
	if (gMainInterface->mainWindow)
	{
		connect(renderQueue,
			SIGNAL(updateProgressAndStatus(QString, QString, double, cProgressText::enumProgressType)),
			gMainInterface->mainWindow,
			SLOT(slotUpdateProgressAndStatus(QString, QString, double, cProgressText::enumProgressType)));
		connect(renderQueue, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
			gMainInterface->mainWindow, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
		connect(renderQueue, SIGNAL(updateImage()), renderedImageWidget, SLOT(update()));
	}
	if (gMainInterface->headless)
	{
		connect(renderQueue,
			SIGNAL(updateProgressAndStatus(QString, QString, double, cProgressText::enumProgressType)),
			gMainInterface->headless,
			SLOT(slotUpdateProgressAndStatus(QString, QString, double, cProgressText::enumProgressType)));
		connect(renderQueue, SIGNAL(updateStatistics(cStatistics)), gMainInterface->headless,
			SLOT(slotUpdateStatistics(cStatistics)));
	}
	QObject::connect(renderQueue, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->setObjectName("RenderQueue");

	// this is needed to get settings from main parameter container.
	// Further will be used only local container
	systemData.numberOfThreads = gPar->Get<int>("limit_CPU_cores");
	systemData.threadsPriority = enumRenderingThreadPriority(gPar->Get<int>("threads_priority"));

	thread->start();
}

/* UI Slots */

void cQueue::slotQueueRender() const
{
	if (!systemData.noGui)
	{
		SynchronizeInterfaceWindow(
			gMainInterface->mainWindow->GetWidgetDockQueue(), gPar, qInterface::read);
	}
	if (queueListFromFile.size() > 0)
	{
		RenderQueue();
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("No queue items to render"), cErrorMessage::errorMessage, nullptr);
	}
}

void cQueue::slotQueueAddCurrentSettings()
{
	gMainInterface->SynchronizeInterface(
		gPar, gParFractal, qInterface::read); // update appParam before loading new settings
	Append();
}

void cQueue::slotQueueAddFromFile()
{
	PreviewFileDialog dialog;
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(systemData.GetSettingsFolder() + QDir::separator());
	dialog.selectFile(systemData.lastSettingsFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Add file to queue..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = filenames.first();
		Append(filename);
	}
}

void cQueue::slotQueueAddOrphaned()
{
	AddOrphanedFilesToList();
}

void cQueue::slotQueueRemoveOrphaned()
{
	RemoveOrphanedFiles();
}

void cQueue::slotQueueRemoveItem()
{
	QString buttonName = this->sender()->objectName();
	RemoveQueueItem(buttonName.toInt());
}

void cQueue::slotQueueMoveItemUp()
{
	QString buttonName = this->sender()->objectName();
	SwapQueueItem(buttonName.toInt(), buttonName.toInt() - 1);
}

void cQueue::slotQueueMoveItemDown()
{
	QString buttonName = this->sender()->objectName();
	SwapQueueItem(buttonName.toInt(), buttonName.toInt() + 1);
}

void cQueue::slotQueueTypeChanged(int index)
{
	QString buttonName = this->sender()->objectName();
	UpdateQueueItemType(buttonName.toInt(), cQueue::enumRenderType(index));
}

void cQueue::slotQueueListUpdate()
{
	QTableWidget *table = ui->tableWidget_queue_list;

	// reset table
	if (GetListFromQueueFile().size() == 0)
	{
		table->clear();
		return;
	}

	// init table
	if (table->columnCount() == 0)
	{
		QStringList header;
		header << tr("Name") << tr("Preview") << tr("Type") << tr("Action");
		table->setColumnCount(header.size());
		table->setHorizontalHeaderLabels(header);
	}

	// change table
	if (table->rowCount() != GetListFromQueueFile().size())
	{
		table->setRowCount(GetListFromQueueFile().size());
	}

	// update table
	for (int i = 0; i < table->rowCount(); i++)
	{
		slotQueueListUpdate(i);
	}
}

void cQueue::slotQueueListUpdate(int i)
{
	// update row i
	QTableWidget *table = ui->tableWidget_queue_list;
	table->setRowHeight(i, 70);
	for (int j = 0; j < table->columnCount(); j++)
	{
		slotQueueListUpdate(i, j);
	}
}

void cQueue::slotQueueListUpdate(int i, int j)
{
	// update element in row i, column j
	QTableWidget *table = ui->tableWidget_queue_list;
	table->blockSignals(true);
	QTableWidgetItem *cell = table->item(i, j);
	if (!cell)
	{
		cell = new QTableWidgetItem;
		table->setItem(i, j, cell);
	}
	QList<cQueue::structQueueItem> queueList = GetListFromQueueFile();
	switch (j)
	{
		case 0: cell->setText(queueList.at(i).filename); break;
		case 1:
		{
			if (ui->checkBox_show_queue_thumbnails->isChecked())
			{
				cParameterContainer tempPar = *gPar;
				cFractalContainer tempFract = *gParFractal;
				cSettings parSettings(cSettings::formatFullText);
				parSettings.BeQuiet(true);
				if (parSettings.LoadFromFile(queueList.at(i).filename)
						&& parSettings.Decode(&tempPar, &tempFract))
				{
					cThumbnailWidget *thumbWidget = static_cast<cThumbnailWidget *>(table->cellWidget(i, j));
					if (!thumbWidget)
					{
						thumbWidget = new cThumbnailWidget(100, 70, 1, table);
						thumbWidget->UseOneCPUCore(true);
						thumbWidget->AssignParameters(tempPar, tempFract);
						table->setCellWidget(i, j, thumbWidget);
					}
					else
					{
						thumbWidget->AssignParameters(tempPar, tempFract);
						thumbWidget->update();
					}
				}
			}
			break;
		}
		case 2:
		{
			QComboBox *typeComboBox = new QComboBox;
			typeComboBox->addItem(cQueue::GetTypeName(cQueue::queue_STILL));
			typeComboBox->addItem(cQueue::GetTypeName(cQueue::queue_FLIGHT));
			typeComboBox->addItem(cQueue::GetTypeName(cQueue::queue_KEYFRAME));
			typeComboBox->setCurrentIndex(queueList.at(i).renderType);

			typeComboBox->setObjectName(QString::number(i));
			QObject::connect(
				typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotQueueTypeChanged(int)));
			table->setCellWidget(i, j, typeComboBox);
			// cell->setTextColor(color);
			break;
		}
		case 3:
		{
			QFrame *frame = new QFrame;
			QGridLayout *gridLayout = new QGridLayout;
			QToolButton *actionDelete = new QToolButton;
			QToolButton *actionMoveUp = new QToolButton;
			QToolButton *actionMoveDown = new QToolButton;

			actionDelete->setIcon(actionDelete->style()->standardIcon(QStyle::SP_TrashIcon));
			actionMoveUp->setIcon(actionMoveUp->style()->standardIcon(QStyle::SP_ArrowUp));
			actionMoveDown->setIcon(actionMoveDown->style()->standardIcon(QStyle::SP_ArrowDown));

			actionDelete->setObjectName(QString::number(i));
			actionMoveUp->setObjectName(QString::number(i));
			actionMoveDown->setObjectName(QString::number(i));

			QObject::connect(actionDelete, SIGNAL(clicked()), this, SLOT(slotQueueRemoveItem()));
			QObject::connect(actionMoveUp, SIGNAL(clicked()), this, SLOT(slotQueueMoveItemUp()));
			QObject::connect(actionMoveDown, SIGNAL(clicked()), this, SLOT(slotQueueMoveItemDown()));

			gridLayout->addWidget(actionMoveUp, 0, 0);
			gridLayout->addWidget(actionMoveDown, 1, 0);
			gridLayout->addWidget(actionDelete, 0, 1);
			gridLayout->setSpacing(0);

			if (i == 0) actionMoveUp->setEnabled(false);
			if (i == queueList.size() - 1) actionMoveDown->setEnabled(false);
			frame->setLayout(gridLayout);
			table->setCellWidget(i, j, frame);
			break;
		}
		default: break;
	}
	table->blockSignals(false);
}

void cQueue::slotStopRequest()
{
	stopRequest = true;
}

int cQueue::GetQueueSize()
{
	mutex.lock();
	int size = queueListFromFile.size();
	mutex.unlock();
	return size;
}

void cQueue::slotShowQueueThumbsChanges(int state)
{
	(void)state;
	slotQueueListUpdate();
}
