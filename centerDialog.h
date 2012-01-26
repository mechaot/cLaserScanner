#ifndef CENTERDIALOG_H
#define CENTERDIALOG_H

#include <QDialog>
#include "ui_centerdialog.h"

#include <opencv.hpp>
#include "cameraThread.h"

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
    void connectCamera(bool connect);
    void setLiveMode(const QString& mode);
    void setZoomMode(void);
    void displayCursorPosition(int x, int y);
    void displayPointPosition(int x, int y);
    void displayRoiPointCoords(const QRect& rect);
    void displayRoiLineCoords(const QRect& rect);

private:
    Ui::centerDialog *ui;
    int        m_iCamera;      ///< id of opencv camera
    CvCapture *m_cvCapture;

    CameraThread *m_threadCam;
};

#endif // CENTERDIALOG_H
