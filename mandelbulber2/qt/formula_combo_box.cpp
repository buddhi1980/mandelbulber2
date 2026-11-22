/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cFormulaComboBox - Formula Combobox for the selection of the used formula
 * This is a combobox with icons and autocomplete for fast formula access
 * The item list gets populated with the populateItemsFromFractalList()
 *
 * autocomplete adopted from here:
 * http://stackoverflow.com/questions/4827207/how-do-i-filter-the-pyqt-qcombobox-items-based-on-the-text-input
 */

#include "formula_combo_box.h"

#include <QDebug>
#include <QLineEdit>
#include <QMenu>
#include <QStandardItemModel>
#include <QWidget>

#include "src/common_math.h"
#include "src/initparameters.hpp"
#include "src/parameters.hpp"
#include "src/random.hpp"
#include "src/write_log.hpp"

QMap<QString, QIcon> cFormulaComboBox::iconCache;
QList<cFormulaComboBox *> cFormulaComboBox::instances;

cFormulaComboBox::cFormulaComboBox(QWidget *parent) : QComboBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = 0;
	colorSeed = 0;
	instances.append(this);
	QFontMetrics fm(font());
	int pixelFontSize = fm.height();
	int iconSize = pixelFontSize * 3;
	setIconSize(QSize(iconSize, iconSize));
	setFixedHeight(iconSize);

	setFocusPolicy(Qt::StrongFocus);
	setEditable(true);

	// add a filter model to filter matching items
	pFilterModel = new QSortFilterProxyModel(this);
	pFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	pFilterModel->setSourceModel(model());

	// add a completer, which uses the filter model
	completer = new QCompleter(pFilterModel, this);
	// always show all (filtered) completions
	completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

	setCompleter(completer);

	// connect signals
	connect(qobject_cast<const QObject *>(lineEdit()), SIGNAL(textEdited(const QString &)),
		pFilterModel, SLOT(setFilterFixedString(const QString &)));
}

cFormulaComboBox::~cFormulaComboBox()
{
	// iconCache.clear(); !!! it's is static member and shoudl be kept to the end of the universe
	instances.removeAll(this);
}

void cFormulaComboBox::onCompleterActivated(QString text)
{
	WriteLogString("onCompleterActivated: text -> ", text, 3);
	// on selection of an item from the completer, select the corresponding item from combobox
	if (!text.isEmpty())
	{
		int index = findText(text);
		setCurrentIndex(index);
		emit currentIndexChanged(index);
	}
}

void cFormulaComboBox::setModelColumn(int column)
{
	// on model column change, update the model column of the filter and completer as well
	completer->setCompletionColumn(column);
	pFilterModel->setFilterKeyColumn(column);
	QComboBox::setModelColumn(column);
}

void cFormulaComboBox::setModel(QAbstractItemModel *model)
{
	// on model change, update the models of the filter and completer as well
	QComboBox::setModel(model);
	pFilterModel->setSourceModel(model);
	completer->setModel(pFilterModel);
}

void cFormulaComboBox::populateItemsFromFractalList(QList<cAbstractFractal *> fractals,
	QList<QPair<int, QString> /* */> insertHeader, int randomSeedForColors)
{
	fractalList = fractals;
	headerList = insertHeader;
	colorSeed = randomSeedForColors;

	clear();
	for (int f = 0; f < fractals.size(); f++)
	{
		addItem(GetIconFromCache(QString::fromStdString(fractals[f]->getIconName())),
			QString::fromStdString(fractals[f]->getNameInComboBox()), f);
	}

	// set headings and separator of formulas and transforms
	QFont fontHeading;
	fontHeading.setBold(true);

	QPalette palette = window()->palette();
	QColor globalColor = palette.window().color();
	int brightness = globalColor.value();

	int rBase = globalColor.red();
	int gBase = globalColor.green();
	int bBase = globalColor.blue();

	cRandom random;
	random.Initialize(randomSeedForColors);

	int previousComboIndex = 0;

	for (int hIndex = 0; hIndex < insertHeader.size(); hIndex++)
	{
		QPair<int, QString> header = insertHeader.at(hIndex);
		int comboIndex = -1;
		for (int fIndex = 0; fIndex < fractals.size(); fIndex++)
		{
			if (fractals.at(fIndex)->getInternalId() == header.first)
			{
				// should be fIndex, but every new header inserts two new items, which have to be added
				comboIndex = fIndex + 2 * hIndex;
			}
		}

		if (comboIndex == -1)
		{
			qCritical() << "Cannot insert combobox Header!";
		}
		else
		{
			QColor itemsColor;
			if (brightness > 20)
			{
				int r = random.Random(40) + rBase - 20;
				r = clamp(r, 0, 255);
				int g = random.Random(40) + gBase - 20;
				g = clamp(g, 0, 255);
				int b = random.Random(40) + bBase - 20;
				b = clamp(b, 0, 255);
				itemsColor = QColor(r, g, b);
			}
			else
			{
				int r = random.Random(40) + rBase;
				r = clamp(r, 0, 255);
				int g = random.Random(40) + gBase;
				g = clamp(g, 0, 255);
				int b = random.Random(40) + bBase;
				b = clamp(b, 0, 255);
				itemsColor = QColor(r, g, b);
			}

			for (int i = previousComboIndex; i < comboIndex; i++)
			{
				setItemData(i, itemsColor, Qt::BackgroundRole);
			}

			previousComboIndex = comboIndex;

			insertItem(comboIndex, header.second);
			setItemData(comboIndex, fontHeading, Qt::FontRole);
			setItemData(comboIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
			qobject_cast<QStandardItemModel *>(model())->item(comboIndex)->setEnabled(false);
			insertSeparator(comboIndex);
		}
	}

	InsertFavourites();
}

void cFormulaComboBox::InsertFavourites()
{
	// favourite formulas are listed once more under their own header at the top of the list
	QList<int> favouriteIndexes;
	for (const QString &name : GetFavourites())
	{
		for (int fIndex = 0; fIndex < fractalList.size(); fIndex++)
		{
			if (QString::fromStdString(fractalList.at(fIndex)->getInternalName()) == name)
			{
				favouriteIndexes.append(fIndex);
				break;
			}
		}
	}
	if (favouriteIndexes.isEmpty()) return;

	QFont fontHeading;
	fontHeading.setBold(true);
	insertItem(0, tr("*** Favourites ***"));
	setItemData(0, fontHeading, Qt::FontRole);
	setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole);
	qobject_cast<QStandardItemModel *>(model())->item(0)->setEnabled(false);

	for (int i = 0; i < favouriteIndexes.size(); i++)
	{
		int fIndex = favouriteIndexes.at(i);
		insertItem(i + 1, GetIconFromCache(QString::fromStdString(fractalList[fIndex]->getIconName())),
			QString::fromStdString(fractalList[fIndex]->getNameInComboBox()), fIndex);
	}
	insertSeparator(favouriteIndexes.size() + 1);
}

void cFormulaComboBox::RefreshItems()
{
	// rebuild the list and keep the selected formula without emitting a change
	if (fractalList.isEmpty()) return;
	int fractalIndex = -1;
	if (currentIndex() >= 0 && itemData(currentIndex()).isValid())
		fractalIndex = itemData(currentIndex()).toInt();

	blockSignals(true);
	populateItemsFromFractalList(fractalList, headerList, colorSeed);
	if (fractalIndex >= 0) setCurrentIndex(findData(fractalIndex));
	blockSignals(false);
}

QStringList cFormulaComboBox::GetFavourites()
{
	if (!gPar || !gPar->IfExists("favourite_formulas")) return QStringList();
	return gPar->Get<QString>("favourite_formulas").split(',', Qt::SkipEmptyParts);
}

void cFormulaComboBox::SetFavourites(const QStringList &favourites)
{
	if (!gPar) return;
	gPar->Set("favourite_formulas", favourites.join(','));
	for (cFormulaComboBox *comboBox : instances)
		comboBox->RefreshItems();
}

QString cFormulaComboBox::GetCurrentInternalName() const
{
	if (currentIndex() < 0 || !itemData(currentIndex()).isValid()) return QString();
	int fIndex = itemData(currentIndex()).toInt();
	if (fIndex < 0 || fIndex >= fractalList.size()) return QString();
	if (fractalList.at(fIndex)->getInternalId() == fractal::none) return QString();
	return QString::fromStdString(fractalList.at(fIndex)->getInternalName());
}

void cFormulaComboBox::slotToggleFavourite()
{
	QString name = GetCurrentInternalName();
	if (name.isEmpty()) return;
	QStringList favourites = GetFavourites();
	if (favourites.contains(name))
		favourites.removeAll(name);
	else
		favourites.append(name);
	SetFavourites(favourites);
}

int cFormulaComboBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void cFormulaComboBox::paintEvent(QPaintEvent *event)
{
	GetDefault();
	QComboBox::paintEvent(event);
}

void cFormulaComboBox::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	QString name = GetCurrentInternalName();
	if (!name.isEmpty())
	{
		QAction *actionFavourite = menu->addAction(
			GetFavourites().contains(name) ? tr("Remove from favourites") : tr("Add to favourites"));
		connect(actionFavourite, SIGNAL(triggered()), this, SLOT(slotToggleFavourite()));
	}
	// the menu is deleted by CommonMyWidgetWrapper::contextMenuEvent()
	CommonMyWidgetWrapper::contextMenuEvent(event, menu);
}

QIcon cFormulaComboBox::GetIconFromCache(const QString &filename)
{
	if (!iconCache.contains(filename))
	{
		iconCache.insert(filename, QIcon(filename));
	}
	return iconCache[filename];
}

void cFormulaComboBox::resetToDefault()
{
	// the default is a formula id; map it to its item, because favourites shift the item indexes
	int selection = defaultValue;
	for (int i = 0; i < newFractalList.size(); i++)
	{
		if (newFractalList[i]->getInternalId() == selection)
		{
			selection = findData(i);
			break;
		}
	}
	setCurrentIndex(selection);
}

QString cFormulaComboBox::getDefaultAsString()
{
	return QString::number(defaultValue);
}

QString cFormulaComboBox::getFullParameterName()
{
	return parameterName;
}

void cFormulaComboBox::UpdateScriptAppearance(bool hasScript)
{
	// scripting-dependent appearance is not implemented for this widget (stub)
	Q_UNUSED(hasScript);
}
