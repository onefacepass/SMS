#ifndef QUICREATOR_H
#define QUICREATOR_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QScopedPointer>
#include <QCameraInfo>
#include <QSettings>

#include "stuwidget.h"
#include "quiwidget.h"

#include "mainwindow.h"
#include "capturethread.h"
#include "facethread.h"

class QPushButton;
class QLineEdit;

namespace Ui
{
class QUICreator;
}

class QUICreator : public QMainWindow
{
    Q_OBJECT

public:
    explicit QUICreator(QWidget *parent = nullptr, const QString& config_file = ":/config.ini");
    ~QUICreator();
    void setQUIWidget(QUIWidget *q);


private:
    QUIWidget* qui;
    int MAX_RESULT = 5;
    Ui::QUICreator *ui;
    MainWindow *mainWindow;
    QImage img_tmp;
    QSettings *config;

    QScopedPointer<QCamera> camera;
    QScopedPointer<QCameraImageCapture> imageCapture;

    CaptureThread *captureThread;
    FaceThread *faceThread;

    QVector<std::shared_ptr<StuWidget>> stuWidgets;
    QVector<std::shared_ptr<QTableWidgetItem>> tableWidgetItems;

private:
    void checkConfig();

    // 打印摄像头支持的分辨率
    void debug_show_supported_viewfinder_resolutions();

    void debug_show_student_info(Student s);




private slots:
    void initForm();
    void initCamera();
    void initFace();

    void takeImage();
    void processCapturedImage(int requestId, const QImage& img);
    void doDetect();
    void doTrack();

    void setCamera(const QCameraInfo &camera_info);
    void updateCamera(QAction *action);
    void displayCameraError();

    void initStyle();
    void setStyle(const QString &str);
    void about();


    // 处理face线程发出的信号
    void faceDetectFinished(QVector<Student> res);
    void faceTrackFinished(QVector<QRect> res);

    void login();
};

#endif // QUICREATOR_H
