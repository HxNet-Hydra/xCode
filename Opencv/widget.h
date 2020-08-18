#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/dnn/dnn.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <QMessageBox>
#include <QFile>
#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

using namespace cv;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void closeEvent(QCloseEvent *event);

private:
    Ui::Widget *ui;

    dnn::Net tfNetwork;
    QGraphicsScene scene;
    QGraphicsPixmapItem pixmap;
    bool videoStopped;
    QTimer *timer;

    VideoCapture cap;

    Mat image;
    Mat edges;
    QImage image1;
    QString dirName;

private slots:
    void updatePicture();
    void on_start_clicked();
    void on_capture_clicked();
};

#endif // WIDGET_H
