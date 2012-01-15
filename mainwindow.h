#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv.hpp>


#define MAX_OPENCV_CAMERA 4 ///< maximum number of cameras to try detection

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void about();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
