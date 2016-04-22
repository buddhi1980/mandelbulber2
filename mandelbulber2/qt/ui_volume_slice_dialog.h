/********************************************************************************
** Form generated from reading UI file 'volume_slice_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VOLUME_SLICE_DIALOG_H
#define UI_VOLUME_SLICE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "mygroupbox.h"
#include "mylineedit.h"
#include "myspinbox.h"

QT_BEGIN_NAMESPACE

class Ui_cVolumeSliceDialog
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    MyLineEdit *text_volume_slice_image_path;
    QPushButton *pushButton_select_image_path;
    QLabel *label;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    MySpinBox *spinboxInt_volume_slice_max_iter;
    QSlider *sliderInt_volume_slice_max_iter;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *gridLayout_3;
    QLabel *label_6;
    MySpinBox *spinboxInt_volume_slice_samples_x;
    QLabel *label_7;
    MySpinBox *spinboxInt_volume_slice_samples_y;
    QLabel *label_8;
    MySpinBox *spinboxInt_volume_slice_samples_z;
    MyGroupBox *groupCheck_volume_slice_custom_limit_enabled;
    QVBoxLayout *verticalLayout_45;
    QGridLayout *gridLayout_21;
    MyLineEdit *vect3_volume_slice_limit_max_z;
    QLabel *label_114;
    MyLineEdit *vect3_volume_slice_limit_min_z;
    QLabel *label_110;
    MyLineEdit *vect3_volume_slice_limit_max_x;
    MyLineEdit *vect3_volume_slice_limit_max_y;
    MyLineEdit *vect3_volume_slice_limit_min_x;
    QLabel *label_111;
    MyLineEdit *vect3_volume_slice_limit_min_y;
    QLabel *label_112;
    QLabel *label_109;
    QLabel *label_117;
    QLabel *label_118;
    QLabel *label_119;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_start_render_slices;
    QPushButton *pushButton_stop_render_slices;
    QProgressBar *progressBar;
    QLabel *label_info;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *cVolumeSliceDialog)
    {
        if (cVolumeSliceDialog->objectName().isEmpty())
            cVolumeSliceDialog->setObjectName(QStringLiteral("cVolumeSliceDialog"));
        cVolumeSliceDialog->resize(443, 454);
        verticalLayout = new QVBoxLayout(cVolumeSliceDialog);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        scrollArea = new QScrollArea(cVolumeSliceDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 437, 448));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(2, 2, 2, 2);
        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        text_volume_slice_image_path = new MyLineEdit(groupBox);
        text_volume_slice_image_path->setObjectName(QStringLiteral("text_volume_slice_image_path"));

        gridLayout->addWidget(text_volume_slice_image_path, 0, 1, 1, 1);

        pushButton_select_image_path = new QPushButton(groupBox);
        pushButton_select_image_path->setObjectName(QStringLiteral("pushButton_select_image_path"));
        QIcon icon;
        QString iconThemeName = QStringLiteral("folder");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QStringLiteral(":/system/icons/folder.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_select_image_path->setIcon(icon);

        gridLayout->addWidget(pushButton_select_image_path, 0, 2, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);


        verticalLayout_3->addLayout(gridLayout);


        verticalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(2, 2, 2, 2);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        spinboxInt_volume_slice_max_iter = new MySpinBox(groupBox_2);
        spinboxInt_volume_slice_max_iter->setObjectName(QStringLiteral("spinboxInt_volume_slice_max_iter"));
        spinboxInt_volume_slice_max_iter->setMinimum(1);
        spinboxInt_volume_slice_max_iter->setMaximum(10000);

        gridLayout_2->addWidget(spinboxInt_volume_slice_max_iter, 0, 2, 1, 1);

        sliderInt_volume_slice_max_iter = new QSlider(groupBox_2);
        sliderInt_volume_slice_max_iter->setObjectName(QStringLiteral("sliderInt_volume_slice_max_iter"));
        sliderInt_volume_slice_max_iter->setMinimum(1);
        sliderInt_volume_slice_max_iter->setMaximum(10000);
        sliderInt_volume_slice_max_iter->setSingleStep(8);
        sliderInt_volume_slice_max_iter->setPageStep(64);
        sliderInt_volume_slice_max_iter->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(sliderInt_volume_slice_max_iter, 0, 1, 1, 1);


        verticalLayout_4->addLayout(gridLayout_2);


        verticalLayout_2->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_5 = new QVBoxLayout(groupBox_3);
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(2, 2, 2, 2);
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 0, 0, 1, 1);

        spinboxInt_volume_slice_samples_x = new MySpinBox(groupBox_3);
        spinboxInt_volume_slice_samples_x->setObjectName(QStringLiteral("spinboxInt_volume_slice_samples_x"));
        spinboxInt_volume_slice_samples_x->setMinimum(1);
        spinboxInt_volume_slice_samples_x->setMaximum(10000);

        gridLayout_3->addWidget(spinboxInt_volume_slice_samples_x, 0, 1, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_3->addWidget(label_7, 0, 2, 1, 1);

        spinboxInt_volume_slice_samples_y = new MySpinBox(groupBox_3);
        spinboxInt_volume_slice_samples_y->setObjectName(QStringLiteral("spinboxInt_volume_slice_samples_y"));
        spinboxInt_volume_slice_samples_y->setMinimum(1);
        spinboxInt_volume_slice_samples_y->setMaximum(10000);

        gridLayout_3->addWidget(spinboxInt_volume_slice_samples_y, 0, 3, 1, 1);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_3->addWidget(label_8, 0, 4, 1, 1);

        spinboxInt_volume_slice_samples_z = new MySpinBox(groupBox_3);
        spinboxInt_volume_slice_samples_z->setObjectName(QStringLiteral("spinboxInt_volume_slice_samples_z"));
        spinboxInt_volume_slice_samples_z->setMinimum(1);
        spinboxInt_volume_slice_samples_z->setMaximum(10000);

        gridLayout_3->addWidget(spinboxInt_volume_slice_samples_z, 0, 5, 1, 1);


        verticalLayout_5->addLayout(gridLayout_3);


        verticalLayout_2->addWidget(groupBox_3);

        groupCheck_volume_slice_custom_limit_enabled = new MyGroupBox(scrollAreaWidgetContents);
        groupCheck_volume_slice_custom_limit_enabled->setObjectName(QStringLiteral("groupCheck_volume_slice_custom_limit_enabled"));
        groupCheck_volume_slice_custom_limit_enabled->setProperty("checkable", QVariant(true));
        verticalLayout_45 = new QVBoxLayout(groupCheck_volume_slice_custom_limit_enabled);
        verticalLayout_45->setSpacing(2);
        verticalLayout_45->setObjectName(QStringLiteral("verticalLayout_45"));
        verticalLayout_45->setContentsMargins(2, 2, 2, 2);
        gridLayout_21 = new QGridLayout();
        gridLayout_21->setSpacing(2);
        gridLayout_21->setObjectName(QStringLiteral("gridLayout_21"));
        vect3_volume_slice_limit_max_z = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_max_z->setObjectName(QStringLiteral("vect3_volume_slice_limit_max_z"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_max_z->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_max_z->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_max_z, 5, 2, 1, 1);

        label_114 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_114->setObjectName(QStringLiteral("label_114"));

        gridLayout_21->addWidget(label_114, 3, 0, 1, 1);

        vect3_volume_slice_limit_min_z = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_min_z->setObjectName(QStringLiteral("vect3_volume_slice_limit_min_z"));
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_min_z->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_min_z->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_min_z, 2, 2, 1, 1);

        label_110 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_110->setObjectName(QStringLiteral("label_110"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_110->sizePolicy().hasHeightForWidth());
        label_110->setSizePolicy(sizePolicy1);
        label_110->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_110, 0, 1, 1, 1);

        vect3_volume_slice_limit_max_x = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_max_x->setObjectName(QStringLiteral("vect3_volume_slice_limit_max_x"));
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_max_x->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_max_x->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_max_x, 3, 2, 1, 1);

        vect3_volume_slice_limit_max_y = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_max_y->setObjectName(QStringLiteral("vect3_volume_slice_limit_max_y"));
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_max_y->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_max_y->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_max_y, 4, 2, 1, 1);

        vect3_volume_slice_limit_min_x = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_min_x->setObjectName(QStringLiteral("vect3_volume_slice_limit_min_x"));
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_min_x->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_min_x->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_min_x, 0, 2, 1, 1);

        label_111 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_111->setObjectName(QStringLiteral("label_111"));
        sizePolicy1.setHeightForWidth(label_111->sizePolicy().hasHeightForWidth());
        label_111->setSizePolicy(sizePolicy1);
        label_111->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_111, 1, 1, 1, 1);

        vect3_volume_slice_limit_min_y = new MyLineEdit(groupCheck_volume_slice_custom_limit_enabled);
        vect3_volume_slice_limit_min_y->setObjectName(QStringLiteral("vect3_volume_slice_limit_min_y"));
        sizePolicy.setHeightForWidth(vect3_volume_slice_limit_min_y->sizePolicy().hasHeightForWidth());
        vect3_volume_slice_limit_min_y->setSizePolicy(sizePolicy);

        gridLayout_21->addWidget(vect3_volume_slice_limit_min_y, 1, 2, 1, 1);

        label_112 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_112->setObjectName(QStringLiteral("label_112"));
        sizePolicy1.setHeightForWidth(label_112->sizePolicy().hasHeightForWidth());
        label_112->setSizePolicy(sizePolicy1);
        label_112->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_112, 2, 1, 1, 1);

        label_109 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_109->setObjectName(QStringLiteral("label_109"));

        gridLayout_21->addWidget(label_109, 0, 0, 1, 1);

        label_117 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_117->setObjectName(QStringLiteral("label_117"));
        sizePolicy1.setHeightForWidth(label_117->sizePolicy().hasHeightForWidth());
        label_117->setSizePolicy(sizePolicy1);
        label_117->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_117, 3, 1, 1, 1);

        label_118 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_118->setObjectName(QStringLiteral("label_118"));
        sizePolicy1.setHeightForWidth(label_118->sizePolicy().hasHeightForWidth());
        label_118->setSizePolicy(sizePolicy1);
        label_118->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_118, 4, 1, 1, 1);

        label_119 = new QLabel(groupCheck_volume_slice_custom_limit_enabled);
        label_119->setObjectName(QStringLiteral("label_119"));
        sizePolicy1.setHeightForWidth(label_119->sizePolicy().hasHeightForWidth());
        label_119->setSizePolicy(sizePolicy1);
        label_119->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_21->addWidget(label_119, 5, 1, 1, 1);


        verticalLayout_45->addLayout(gridLayout_21);


        verticalLayout_2->addWidget(groupCheck_volume_slice_custom_limit_enabled);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_start_render_slices = new QPushButton(scrollAreaWidgetContents);
        pushButton_start_render_slices->setObjectName(QStringLiteral("pushButton_start_render_slices"));

        horizontalLayout->addWidget(pushButton_start_render_slices);

        pushButton_stop_render_slices = new QPushButton(scrollAreaWidgetContents);
        pushButton_stop_render_slices->setObjectName(QStringLiteral("pushButton_stop_render_slices"));

        horizontalLayout->addWidget(pushButton_stop_render_slices);


        verticalLayout_2->addLayout(horizontalLayout);

        progressBar = new QProgressBar(scrollAreaWidgetContents);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        verticalLayout_2->addWidget(progressBar);

        label_info = new QLabel(scrollAreaWidgetContents);
        label_info->setObjectName(QStringLiteral("label_info"));

        verticalLayout_2->addWidget(label_info);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(cVolumeSliceDialog);

        QMetaObject::connectSlotsByName(cVolumeSliceDialog);
    } // setupUi

    void retranslateUi(QDialog *cVolumeSliceDialog)
    {
        cVolumeSliceDialog->setWindowTitle(QApplication::translate("cVolumeSliceDialog", "Export Volume Slices", 0));
        groupBox->setTitle(QApplication::translate("cVolumeSliceDialog", "Slice settings", 0));
        pushButton_select_image_path->setText(QString());
        label->setText(QApplication::translate("cVolumeSliceDialog", "slices folder", 0));
        groupBox_2->setTitle(QApplication::translate("cVolumeSliceDialog", "Render settings", 0));
        label_5->setText(QApplication::translate("cVolumeSliceDialog", "MaxIter", 0));
        groupBox_3->setTitle(QApplication::translate("cVolumeSliceDialog", "Sample count", 0));
        label_6->setText(QApplication::translate("cVolumeSliceDialog", "Samples X", 0));
        label_7->setText(QApplication::translate("cVolumeSliceDialog", "Samples Y", 0));
        label_8->setText(QApplication::translate("cVolumeSliceDialog", "Samples Z", 0));
        groupCheck_volume_slice_custom_limit_enabled->setProperty("title", QVariant(QApplication::translate("cVolumeSliceDialog", "Custom Limits (leave untoggled to use global limits)", 0)));
        label_114->setText(QApplication::translate("cVolumeSliceDialog", "top right back corner:", 0));
        label_110->setText(QApplication::translate("cVolumeSliceDialog", "x:", 0));
        label_111->setText(QApplication::translate("cVolumeSliceDialog", "y:", 0));
        label_112->setText(QApplication::translate("cVolumeSliceDialog", "z:", 0));
        label_109->setText(QApplication::translate("cVolumeSliceDialog", "bottom left front corner:", 0));
        label_117->setText(QApplication::translate("cVolumeSliceDialog", "x:", 0));
        label_118->setText(QApplication::translate("cVolumeSliceDialog", "y:", 0));
        label_119->setText(QApplication::translate("cVolumeSliceDialog", "z:", 0));
        pushButton_start_render_slices->setText(QApplication::translate("cVolumeSliceDialog", "Render slices", 0));
        pushButton_stop_render_slices->setText(QApplication::translate("cVolumeSliceDialog", "Stop Render", 0));
        label_info->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class cVolumeSliceDialog: public Ui_cVolumeSliceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VOLUME_SLICE_DIALOG_H
