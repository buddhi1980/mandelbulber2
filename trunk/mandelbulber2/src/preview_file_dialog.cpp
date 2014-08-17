/*
 * PreviewFileDialog.cpp
 *
 *  Created on: Aug 2, 2014
 *      Author: krzysztof
 */

#include "preview_file_dialog.h"
#include <QGridLayout>

PreviewFileDialog::PreviewFileDialog(QWidget *parent) : QFileDialog(parent)
{
	setOption(QFileDialog::DontUseNativeDialog);

	preview = NULL;
	vboxlayout = new QVBoxLayout();

	checkbox = new QCheckBox("Preview");
	checkbox->setChecked(true);

	preview = new QLabel("", this);
	preview->setAlignment(Qt::AlignCenter);
	preview->setObjectName("label_preview");

	info = new QLabel("");

	vboxlayout->addWidget(checkbox);
	vboxlayout->addWidget(preview);
	vboxlayout->addWidget(info);
	vboxlayout -> addStretch();

	//add to existing layout
	QGridLayout *gridlayout = (QGridLayout*)this->layout();
	gridlayout->addLayout(vboxlayout, 1, 3, 3, 1);

	connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(OnCurredbtChanged(const QString&)));

}

PreviewFileDialog::~PreviewFileDialog()
{
	delete vboxlayout;
	delete preview;
	delete info;
}

void PreviewFileDialog::OnCurredbtChanged(const QString & filename)
{
	QPixmap pixmap;
	pixmap.load(filename);
	if(pixmap.isNull() || !checkbox->isChecked())
	{
		preview->setText(" ");
		info->setText(" ");
	}
	else
	{
		preview->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		QString text = QString::number(pixmap.width()) + QString(" x ") + QString::number(pixmap.height());
		info->setText(text);
	}
}

