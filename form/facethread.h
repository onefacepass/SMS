﻿#ifndef FACEDETETHREAD_H
#define FACEDETETHREAD_H

#include <QMutex>
#include <QImage>
#include <QVector>
#include <QRect>
#include <QQueue>

#include "FaceDete.h"


typedef struct t_student {
    bool identifiable;
    QString id;
    QString name;
    QString major;
    QRect faceRect;
    QString path;
    QString grade;
    QString contack_number;
    QString contack_add;
    QString gender;
} Student;



//class FaceThread :  public QObject, public QRunnable
class FaceThread :  public QThread
{
    Q_OBJECT
public:
    FaceThread(const QString& photoPath);
    ~FaceThread();

private:
    FaceDete* facedete;
    QPair<QImage, bool> t;

    Json::Value detectedResult;
    QVector<QRect> resultOnlyTrack;
    QVector<Student> resultComplete;

    QQueue<QPair<QImage, bool>> tasks;

    QMutex lock;

protected:
    void run();

signals:
    void DetectFinished(QVector<Student> res);
    void DetectFinishedWithoutResult();

    void TrackFinished(QVector<QRect> res);
    void TrackFinishedWithoutResult();

public slots:
    void ReceiveImg(bool _detect, const QImage& image);
};

#endif // FACEDETETHREAD_H
