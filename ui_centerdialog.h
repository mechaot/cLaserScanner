/********************************************************************************
** Form generated from reading UI file 'centerdialog.ui'
**
** Created: Sun 15. Jan 16:59:26 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CENTERDIALOG_H
#define UI_CENTERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "cameraWidget.h"

QT_BEGIN_NAMESPACE

class Ui_centerDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *pageLayout;
    QVBoxLayout *pageLayout2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QComboBox *comboCameras;
    QPushButton *buttonCamera;
    QPushButton *buttonCameraFind;
    QPushButton *buttonCalibrateRt;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QLabel *labelCamProps;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QPushButton *pushButton_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    CameraWidget *cameraWidget;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QLabel *labelCursorPos;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLabel *labelLaserPoint;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBox_2;
    QPushButton *pushButton_4;
    QVBoxLayout *verticalLayout;
    QWidget *heightmapWidget;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *buttonDigitize;
    QPushButton *buttonMayavi;
    QPushButton *pushButton_8;
    QPushButton *pushButton_7;
    QPushButton *pushButton_6;
    QPushButton *buttonHeightmapClear;

    void setupUi(QDialog *centerDialog)
    {
        if (centerDialog->objectName().isEmpty())
            centerDialog->setObjectName(QString::fromUtf8("centerDialog"));
        centerDialog->resize(1114, 789);
        layoutWidget = new QWidget(centerDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 30, 1061, 731));
        pageLayout = new QHBoxLayout(layoutWidget);
        pageLayout->setObjectName(QString::fromUtf8("pageLayout"));
        pageLayout->setContentsMargins(0, 0, 0, 0);
        pageLayout2 = new QVBoxLayout();
        pageLayout2->setObjectName(QString::fromUtf8("pageLayout2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        comboCameras = new QComboBox(layoutWidget);
        comboCameras->setObjectName(QString::fromUtf8("comboCameras"));

        horizontalLayout->addWidget(comboCameras);

        buttonCamera = new QPushButton(layoutWidget);
        buttonCamera->setObjectName(QString::fromUtf8("buttonCamera"));
        buttonCamera->setCheckable(true);

        horizontalLayout->addWidget(buttonCamera);

        buttonCameraFind = new QPushButton(layoutWidget);
        buttonCameraFind->setObjectName(QString::fromUtf8("buttonCameraFind"));

        horizontalLayout->addWidget(buttonCameraFind);

        buttonCalibrateRt = new QPushButton(layoutWidget);
        buttonCalibrateRt->setObjectName(QString::fromUtf8("buttonCalibrateRt"));
        buttonCalibrateRt->setEnabled(true);

        horizontalLayout->addWidget(buttonCalibrateRt);

        horizontalLayout->setStretch(1, 1);

        pageLayout2->addLayout(horizontalLayout);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_7->addWidget(label_8);

        labelCamProps = new QLabel(layoutWidget);
        labelCamProps->setObjectName(QString::fromUtf8("labelCamProps"));
        labelCamProps->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_7->addWidget(labelCamProps);

        horizontalLayout_7->setStretch(1, 1);

        pageLayout2->addLayout(horizontalLayout_7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_4->addWidget(label_5);

        lineEdit = new QLineEdit(layoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_4->addWidget(lineEdit);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setEnabled(false);

        horizontalLayout_4->addWidget(pushButton);

        pushButton_3 = new QPushButton(layoutWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setEnabled(false);

        horizontalLayout_4->addWidget(pushButton_3);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setEnabled(false);

        horizontalLayout_4->addWidget(pushButton_2);

        horizontalLayout_4->setStretch(1, 1);

        pageLayout2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);


        pageLayout2->addLayout(horizontalLayout_3);

        cameraWidget = new CameraWidget(layoutWidget);
        cameraWidget->setObjectName(QString::fromUtf8("cameraWidget"));
        cameraWidget->setStyleSheet(QString::fromUtf8("border: 1px;\n"
"border-style: solid;\n"
""));

        pageLayout2->addWidget(cameraWidget);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));

        horizontalLayout_8->addWidget(label_9);

        labelCursorPos = new QLabel(layoutWidget);
        labelCursorPos->setObjectName(QString::fromUtf8("labelCursorPos"));
        labelCursorPos->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));
        labelCursorPos->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_8->addWidget(labelCursorPos);


        pageLayout2->addLayout(horizontalLayout_8);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));

        horizontalLayout_2->addWidget(label_4);

        labelLaserPoint = new QLabel(layoutWidget);
        labelLaserPoint->setObjectName(QString::fromUtf8("labelLaserPoint"));
        labelLaserPoint->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));
        labelLaserPoint->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelLaserPoint);


        pageLayout2->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));

        horizontalLayout_5->addWidget(label_6);

        doubleSpinBox = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));
        doubleSpinBox->setDecimals(1);
        doubleSpinBox->setMaximum(200);
        doubleSpinBox->setValue(5);

        horizontalLayout_5->addWidget(doubleSpinBox);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));

        horizontalLayout_5->addWidget(label_7);

        doubleSpinBox_2 = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));
        doubleSpinBox_2->setStyleSheet(QString::fromUtf8("font: 75 12pt;"));
        doubleSpinBox_2->setDecimals(1);
        doubleSpinBox_2->setMaximum(200);
        doubleSpinBox_2->setValue(120);

        horizontalLayout_5->addWidget(doubleSpinBox_2);

        pushButton_4 = new QPushButton(layoutWidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setEnabled(false);

        horizontalLayout_5->addWidget(pushButton_4);

        horizontalLayout_5->setStretch(0, 1);

        pageLayout2->addLayout(horizontalLayout_5);

        pageLayout2->setStretch(4, 1);

        pageLayout->addLayout(pageLayout2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        heightmapWidget = new QWidget(layoutWidget);
        heightmapWidget->setObjectName(QString::fromUtf8("heightmapWidget"));

        verticalLayout->addWidget(heightmapWidget);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        buttonDigitize = new QPushButton(layoutWidget);
        buttonDigitize->setObjectName(QString::fromUtf8("buttonDigitize"));
        buttonDigitize->setEnabled(true);
        buttonDigitize->setCheckable(true);

        horizontalLayout_6->addWidget(buttonDigitize);

        buttonMayavi = new QPushButton(layoutWidget);
        buttonMayavi->setObjectName(QString::fromUtf8("buttonMayavi"));
        buttonMayavi->setEnabled(true);
        buttonMayavi->setCheckable(true);

        horizontalLayout_6->addWidget(buttonMayavi);

        pushButton_8 = new QPushButton(layoutWidget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setEnabled(false);

        horizontalLayout_6->addWidget(pushButton_8);

        pushButton_7 = new QPushButton(layoutWidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setEnabled(false);

        horizontalLayout_6->addWidget(pushButton_7);

        pushButton_6 = new QPushButton(layoutWidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setEnabled(false);

        horizontalLayout_6->addWidget(pushButton_6);

        buttonHeightmapClear = new QPushButton(layoutWidget);
        buttonHeightmapClear->setObjectName(QString::fromUtf8("buttonHeightmapClear"));
        buttonHeightmapClear->setEnabled(true);

        horizontalLayout_6->addWidget(buttonHeightmapClear);


        verticalLayout->addLayout(horizontalLayout_6);

        verticalLayout->setStretch(0, 1);

        pageLayout->addLayout(verticalLayout);

        pageLayout->setStretch(0, 1);
        pageLayout->setStretch(1, 1);

        retranslateUi(centerDialog);

        QMetaObject::connectSlotsByName(centerDialog);
    } // setupUi

    void retranslateUi(QDialog *centerDialog)
    {
        centerDialog->setWindowTitle(QApplication::translate("centerDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("centerDialog", "Camera:", 0, QApplication::UnicodeUTF8));
        buttonCamera->setText(QApplication::translate("centerDialog", "Connect", 0, QApplication::UnicodeUTF8));
        buttonCameraFind->setText(QApplication::translate("centerDialog", "Find", 0, QApplication::UnicodeUTF8));
        buttonCalibrateRt->setText(QApplication::translate("centerDialog", "Calibrate Rt", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("centerDialog", "Resolution:", 0, QApplication::UnicodeUTF8));
        labelCamProps->setText(QApplication::translate("centerDialog", "-", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("centerDialog", "Configuration:", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QApplication::translate("centerDialog", "D:/test/datei.xml", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("centerDialog", "Browse", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("centerDialog", "View", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("centerDialog", "Apply", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("centerDialog", "Mode:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("centerDialog", "Cursor Position [px]", 0, QApplication::UnicodeUTF8));
        labelCursorPos->setText(QApplication::translate("centerDialog", "-, -", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("centerDialog", "Point Position:", 0, QApplication::UnicodeUTF8));
        labelLaserPoint->setText(QApplication::translate("centerDialog", "-", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("centerDialog", "Digitize from", 0, QApplication::UnicodeUTF8));
        doubleSpinBox->setSuffix(QString());
        label_7->setText(QApplication::translate("centerDialog", "to", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("centerDialog", "Start", 0, QApplication::UnicodeUTF8));
        buttonDigitize->setText(QApplication::translate("centerDialog", "Digitize", 0, QApplication::UnicodeUTF8));
        buttonMayavi->setText(QApplication::translate("centerDialog", "3D", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("centerDialog", "Load Points", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("centerDialog", "Save Points", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("centerDialog", "Save Image", 0, QApplication::UnicodeUTF8));
        buttonHeightmapClear->setText(QApplication::translate("centerDialog", "Clear", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class centerDialog: public Ui_centerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CENTERDIALOG_H
