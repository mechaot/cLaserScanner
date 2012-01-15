#ifndef CENTERDIALOG_H
#define CENTERDIALOG_H

#include <QDialog>
#include <opencv.hpp>
#include "ui_centerdialog.h"

#define MAX_OPENCV_CAMERA 4 ///< maximum number of cameras to try detection

namespace Ui {
   // class CenterDialog;
    class centerDialog;
};

class CenterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CenterDialog(QWidget *parent = 0);
    ~CenterDialog();

private slots:
    void findCameras(void);
    void connectCamera(void);

private:
    Ui::centerDialog *ui;
    int        m_iCamera;      ///< id of opencv camera
    CvCapture *m_cvCapture;
};

#endif // CENTERDIALOG_H
