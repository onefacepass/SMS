#include "quicreator.h"
#include "ui_quicreator.h"
#include "stuwidget.h"
#include "quiwidget.h"

Q_DECLARE_METATYPE(QCameraInfo)

QUICreator::QUICreator(QWidget *parent, const QString& config_file) :
    QMainWindow(parent),
    ui(new Ui::QUICreator)
{

    ui->setupUi(this);

    config = new QSettings(config_file, QSettings::IniFormat);


    checkConfig();

    initForm();

    initFace();
}

QUICreator::~QUICreator()
{
    delete ui;

    if (faceThread) {
        faceThread->requestInterruption();
    }

    faceThread->wait();
}

void QUICreator::initForm()
{
    initCamera();

    initStyle();

}

/*
 * @func: 初始化负责人脸识别的进程
 */
void QUICreator::initFace()
{
#ifdef DEBUG
    qDebug() << "照片目录：" << QDir::toNativeSeparators(config->value("FaceDetect/preload").toString());
#endif
    faceThread = new FaceThread(QDir::toNativeSeparators(config->value("FaceDetect/preload").toString()));
    connect(faceThread, &FaceThread::DetectFinished, this, &QUICreator::faceDetectFinished);
    connect(faceThread, &FaceThread::TrackFinished, this, &QUICreator::faceTrackFinished);

    faceThread->start();
}


void QUICreator::about()
{

    QUIWidget::showMessageBoxInfo(tr("关于一脸通"));
//    QMessageBox::about(this, tr("关于一脸通"), tr("关于一脸通"));
}

/*
 * @func: 初始化摄像头 & 初始化负载对视频流截图的进程
 */
void QUICreator::initCamera()
{
    captureThread = new CaptureThread();
//    connect(captureThread, &QThread::finished, captureThread, &QObject::deleteLater);

// 加载“设备”菜单：后面现场调试时，可能电脑上会有多个摄像头设备吧！
// TODO: 当前多摄像设备的支持是有Bug的，后续再考虑修复Bug或者删除该支持
//    QActionGroup *videoDevicesGroup = new QActionGroup(this);
//    videoDevicesGroup->setExclusive(true);
//    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
//    for (const QCameraInfo &cameraInfo : availableCameras) {
//        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
//        videoDeviceAction->setCheckable(true);
//        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
//        if (cameraInfo == QCameraInfo::defaultCamera())
//            videoDeviceAction->setChecked(true);

//        ui->menuDevice->addAction(videoDeviceAction);
//    }

//    connect(videoDevicesGroup, &QActionGroup::triggered, this, &QUICreator::updateCamera);

    if (QCameraInfo::defaultCamera().isNull()) {
        QUIWidget::showMessageBoxError("没有检测到摄像头");
        close();
    }
    // 使用默认摄像头
    setCamera(QCameraInfo::defaultCamera());


    connect(captureThread, &CaptureThread::CaptureNotice, this, &QUICreator::takeImage);

    connect(captureThread, &CaptureThread::DetectFaceNotice, this, &QUICreator::doDetect);
    connect(captureThread, &CaptureThread::TrackFaceNotice, this, &QUICreator::doTrack);

    captureThread->start();
}


/*
 * @func: 对视频流进行截图
 *        这里会触发QCameraImageCapture::imageCaptured信号
 */
void QUICreator::takeImage()
{
    imageCapture->capture();
}

/*
 * @func: 切换摄像头
 */
void QUICreator::updateCamera(QAction *action)
{
    QUICreator::setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

/*
 * @func: 设置当前摄像头
 */
void QUICreator::setCamera(const QCameraInfo &cameraInfo)
{
    camera.reset(new QCamera(cameraInfo));

    imageCapture.reset(new QCameraImageCapture(camera.data()));

    connect(camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &QUICreator::displayCameraError);
    connect(imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &QUICreator::processCapturedImage);


    // 仅将截图保存到缓冲区
    if (imageCapture->isCaptureDestinationSupported(QCameraImageCapture::CaptureToBuffer) ) {
        imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    } else {
        QUIWidget::showMessageBoxInfo("暂不支持将截图保存至缓冲区！");
    }

    camera->setCaptureMode(QCamera::CaptureStillImage);

    camera->setViewfinder(ui->viewfinder);

    camera->start();

    // 设置摄像头分辨率
    QCameraViewfinderSettings set;
    set.setResolution(640, 480);
    camera->setViewfinderSettings(set);
}

/*
 * @func: 通知线程进行人脸识别和姿态识别
 */
void QUICreator::doDetect()
{
    if (this->img_tmp.isNull()) {
        return;
    }

    faceThread->ReceiveImg(true, this->img_tmp);
    // poseThread->ReceiveImg(true, this->img_tmp); // TODO
}

/*
 * @func: 通知线程进行人脸跟踪和姿态跟踪
 */
void QUICreator::doTrack()
{
    if (this->img_tmp.isNull()) {
        return;
    }

    faceThread->ReceiveImg(false, this->img_tmp);
    // poseThread->ReceiveImg(false, this->img_tmp);    // TODO
}


/*
 * @func: 处理人脸追踪结果：通知viewfinder画人脸框
 * @res: 人脸的范围坐标，可能是多个人脸
 */
void QUICreator::faceTrackFinished(QVector<QRect> res)
{
    ui->viewfinder->ReceiveRects(res);
    ui->viewfinder->update();
}

/*
 * @func: 处理人脸识别结果
 * @res: 由 FaceDetectThread::faceDetectFinished 发送过来的一组 Student
 */
void QUICreator::faceDetectFinished(QVector<Student> res)
{
#ifdef DEBUG
    for(auto r : res) {
        qDebug() << r.identifiable << r.id << "\t" << r.name << "\t" << r.major;
    }
#endif

    // 只有一个识别结果
    if (res.size() == 1) {
        if (!res[0].identifiable)
            return;

        // 停止人脸识别线程
        captureThread->requestInterruption();
        faceThread->requestInterruption();
        // 停止摄像头
        camera->stop();
        ui->labStatus->setText("识别成功！");

        tableWidgetItems.clear();

        tableWidgetItems.push_back(shared_ptr<QTableWidgetItem>(new QTableWidgetItem(QString(res[0].id))));
        tableWidgetItems.push_back(shared_ptr<QTableWidgetItem>(new QTableWidgetItem(QString(res[0].name))));
        tableWidgetItems.push_back(shared_ptr<QTableWidgetItem>(new QTableWidgetItem(QString(res[0].major))));

        login();
    }
    // 多个识别结果
    else {
        ui->labStatus->setText("检测到多人！");
        QUIWidget::showMessageBoxInfo("检测到多人！", 5);
    }
}

/*
 * @func: 执行登陆操作
 */
void QUICreator::login()
{
    if (!this->qui) {
        qDebug() << "Error: this->qui == nullptr";
    } else {
        QString info = "欢迎您，来自" + tableWidgetItems[2]->text() + "专业的" + tableWidgetItems[1]->text() + "！";
        QUIWidget::showMessageBoxInfo(info, 3);


        MainWindow* mw = new MainWindow();
        qui->setMainWidget(mw);

        mw->updateTableWidgetInfo(tableWidgetItems);

    }
}

/*
 * @func:       处理 takeImage 发送的信号
 * @requestId:  这是一个递增的序列，是QCameraCapture中实现的，当前没用
 * @_img:       截取的一帧视频流
 */
void QUICreator::processCapturedImage(int requestId, const QImage& _img)
{
    Q_UNUSED(requestId)

    ui->labStatus->setText("正在识别...");

    this->img_tmp = _img;
}




void QUICreator::displayCameraError()
{
    QUIWidget::showMessageBoxError("Camera Error: " + camera->errorString());
}

void QUICreator::setQUIWidget(QUIWidget *q)
{
    this->qui = q;
}


void QUICreator::initStyle()
{
    QString fileName = ":/qss/blue.css";

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QFile::ReadOnly)) {
            QString str = file.readAll();
            setStyle(str);
        }
    }
}



void QUICreator::setStyle(const QString &str)
{
    static QString qss;

    if (qss == str) {
        return;
    }

    qss = str;
    QString paletteColor = str.mid(20, 7);
    qApp->setPalette(QPalette(QColor(paletteColor)));
    qApp->setStyleSheet(str);
}

void QUICreator::checkConfig()
{
    if (config->allKeys().size() == 0) {
        QUIWidget::ShowMessageBoxErrorAndExit("配置文件初始化失败！");
    } else if (!config->contains("FaceDetect/preload")) {
        QUIWidget::ShowMessageBoxErrorAndExit("配置参数preload错误！");
    }
#ifdef DEBUG_CONFIG
    qDebug() << "[CONFIG]";
    foreach (auto key, config->allKeys()) {
        qDebug() << key << "=" << config->value(key).toString();
    }
    qDebug() << "\n";
#endif
}


/*********************************************************
 *                      调试专用                          *
 *********************************************************/

void QUICreator::debug_show_supported_viewfinder_resolutions()
{
    QList<QSize> size = camera->supportedViewfinderResolutions();
    qDebug() << "supportedViewfinderResolutions" << size.length();
    for (auto s : size) {
        qDebug() << s.width() << " " << s.height();
    }
}

void QUICreator::debug_show_student_info(Student s)
{
    qDebug()<< "\033[32m" << s.identifiable << s.id << "\t" << s.name << "\t" << s.major << s.path;
}
