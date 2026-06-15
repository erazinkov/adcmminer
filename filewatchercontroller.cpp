#include "filewatchercontroller.h"


FileWatcherController::FileWatcherController(const QString &path)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1'000);
    m_thread = new QThread(this);
    m_worker = new FileWatcherWorker(path);
    m_worker->moveToThread(m_thread);

    connect(m_timer, &QTimer::timeout, m_worker, &FileWatcherWorker::doWorkCheck);
//    connect(this, &FileWatcherController::operateFC, m_worker, &FileWatcherWorker::doWorkFC);
    connect(this, &FileWatcherController::operateP, m_worker, &FileWatcherWorker::doWorkPath);
    connect(m_worker, &FileWatcherWorker::resultReadyCheck, this, &FileWatcherController::handleResultsReadyFileCheck);
    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_thread->start();
    m_timer->start();
}

FileWatcherController::~FileWatcherController()
{
    if (m_timer != nullptr) {
        m_timer->stop();
    }
    m_thread->quit();
    m_thread->wait();
}
