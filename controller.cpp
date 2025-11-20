#include "controller.h"

Controller::Controller(const QString &path)
{
    Worker *worker = new Worker(path);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operateS, worker, &Worker::doWorkS);
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
