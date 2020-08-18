#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //disable window resize
    setMaximumSize(geometry().width(), geometry().height());
    setMinimumSize(geometry().width(), geometry().height());
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePicture()));
    videoStopped = true;

    QString path = QCoreApplication::applicationDirPath();
    dirName = path+"/tmp";

    QDir dir(dirName);

    if(!dir.exists(dirName)){
        dir.mkpath(dirName);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(!videoStopped)
    {
        QMessageBox::warning(this,
                             "Warning",
                             "First make sure you stop the camera!");
        event->ignore();
    }
    else
    {
        cap.release();
        delete timer;
        event->accept();
    }
}

void Widget::updatePicture()
{
    cap >> image;

    // detect the faces
    CascadeClassifier face_cascade;
    face_cascade.load("C:\\Silkron\\Projects\\Vendron4\\Core\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
    // CV_HAAR_SCALE_IMAGE 2
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(image, faces, 1.1, 2,1 | 2, Size(100,100));

    // circle/rectangle the faces
    for(int i = 0; i < faces.size(); i++)
    {
        // rectangle with blue colour
        Rect r = faces[i];
        Scalar color = Scalar(255, 0, 0);
        rectangle(image, cv::Point(cvRound(r.x*1), cvRound(r.y*1)), cv::Point(cvRound((r.x +
                                                                                       r.width-1)*1), cvRound((r.y + r.height-1)*1)), color, 3, 8, 0);

        //        // circle with pink color
        //        Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
        //        ellipse(image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);
    }


    if (image.channels() == 3) {
        cvtColor(image, edges,cv::COLOR_BGR2RGB);

        image1 = QImage((uchar*) (edges.data),
                        edges.cols, edges.rows, edges.step, QImage::Format_RGB888);
    }
    else
    {
        image1 = QImage((const unsigned char*)(image.data),
                        image.cols,image.rows,QImage::Format_Indexed8);
    }

    if(!image1.isNull()) {
        //qDebug()<<"myLabel"<<image1.isNull()<<cap.read(image);
        ui->myLabel->setAlignment(Qt::AlignCenter);
        ui->myLabel->setPixmap(QPixmap::fromImage(image1).scaled(ui->myLabel->size(),
                                                                 Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
}

void Widget::on_start_clicked()
{
    if(videoStopped) {
        videoStopped = false;
        cap.open(0);

        if(!timer->isActive())
            timer->start(20);

        ui->start->setText("Stop");
    } else {
        videoStopped = true;
        cap.release();

        if(timer->isActive())
            timer->stop();

        ui->myLabel->clear();
        ui->start->setText("Start");
    }
}

void Widget::on_capture_clicked()
{
    if(videoStopped) {
        QMessageBox::warning(this,
                             "Warning",
                             "Please make sure you open the camera!");

        return;
    }

    // detect the faces
    CascadeClassifier face_cascade;
    face_cascade.load("C:\\Silkron\\Projects\\Vendron4\\Core\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
    // CV_HAAR_SCALE_IMAGE 2
    Mat gray = image;
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(image, faces, 1.1, 2,1 | 2, Size(100,100));

    // circle/rectangle the faces
    Rect roi, rec, rec_tmp;
    for(int i = 0; i < faces.size(); i++)
    {
        // rectangle with blue colour
        Rect r = faces[i];
        rec_tmp.x = cvRound(r.x*1);
        rec_tmp.y = cvRound(r.y*1);
        rec_tmp.width = cvRound((r.x +r.width-1)*1);
        rec_tmp.height = cvRound((r.y + r.height-1)*1);
        //        rectangle(gray, cv::Point(cvRound(r.x*1), cvRound(r.y*1)), cv::Point(cvRound((r.x +
        //                                                                                      r.width-1)*1), cvRound((r.y + r.height-1)*1)), color, 3, 8, 0);


        // capture the largest width and height
        if(rec_tmp.width >= rec.width && rec_tmp.height >= rec.height) {
            // rect values of face only
            rec.x = rec_tmp.x;
            rec.y = rec_tmp.y;
            rec.width = rec_tmp.width;
            rec.height = rec_tmp.height;

            // rect values of crop
            roi.x = r.x;
            roi.y = r.y;
            roi.width = r.width;
            roi.height = r.height;
        }

        //        // circle with pink colour
        //        Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
        //        ellipse(gray, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);
    }

    Scalar color = Scalar(255, 0, 0);
    rectangle(gray, rec, color, 3, 8, 0);

    // return if no face detected
    if(roi.width == 0 && roi.height == 0)
        return;

    Mat crop = gray(roi);

    if (crop.channels() == 3) {
        cvtColor(crop, edges,cv::COLOR_BGR2RGB);

        image1 = QImage((uchar*) (edges.data),
                        edges.cols, edges.rows, edges.step, QImage::Format_RGB888);
    }
    else
    {
        image1 = QImage((const unsigned char*)(crop.data),
                        crop.cols,crop.rows,QImage::Format_Indexed8);
    }

    if(!image1.isNull()) {
        ui->small_img->setAlignment(Qt::AlignCenter);
        ui->small_img->setPixmap(QPixmap::fromImage(image1).scaled(100,100,
                                                                 Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }

    // save image and display in label
    // save image into tmp folder
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString(QLatin1String("yyyy_MM_dd_hhmmss"));
    QString _qsname = dirName+"/"+QString::fromLatin1("image_%1.png").arg(timestamp);

    //    // other way of save image
    //    bool result = false;
    //    if (!_qsname.isEmpty())
    //        result = image1.save(_qsname);

    Mat src;
    cvtColor(crop, src, cv::COLOR_BGR2GRAY);
    Size size(100,100);
    Mat gray_image;
    cv::resize(src,gray_image,size);
    bool result = imwrite(_qsname.toStdString(), gray_image);
    if(!result)
        return;

    QDir dir(dirName);
    QFileInfoList files_sub_name = dir.entryInfoList(QDir::Files);
    Q_FOREACH (QFileInfo file, files_sub_name) {
        Mat img = imread(file.absoluteFilePath().toStdString(), 0);
        int result_cols = img.cols - gray_image.cols + 1;
        int result_rows = img.rows - gray_image.rows + 1;
        Mat result;
        result.create(result_cols, result_rows, CV_32FC1);

        /// Do the Matching and Normalize
        matchTemplate(img, gray_image, result, TM_CCOEFF);
        normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
    }

    /* // read an image
    Mat myImage = imread(_qsname.toStdString(), 1);
    // create image window named "My Image"
    namedWindow("My Image");
    // show the image on window
    imshow("My Image", myImage);
    */
}
