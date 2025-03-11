#include "controller.h"

Controller::Controller(const QString &path, const ChannelMap &pre)
{
    Worker *worker = new Worker(path, pre);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operate, worker, &Worker::doWork);
    connect(this, &Controller::operate1, worker, &Worker::doWork1);
    connect(worker, &Worker::resultReady, this, &Controller::handleResults);
    workerThread.start();
}

Controller::~Controller() {
    workerThread.quit();
    workerThread.wait();
}

//void Controller::handleResults(const QMap<QString, QList<QPointF> > &result)
//{

//}
