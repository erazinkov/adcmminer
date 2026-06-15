#include "processingcontroller.h"

ProcessingController::ProcessingController()
{
    m_thread = new QThread(this);
    m_worker = new ProcessingWorker();
    m_worker->moveToThread(m_thread);

    connect(this, &ProcessingController::operateS, m_worker, &ProcessingWorker::doWorkS);
    connect(this, &ProcessingController::operateR, m_worker, &ProcessingWorker::doWorkR);
    connect(m_worker, &ProcessingWorker::resultReadyTimeCorrectedByAlpha, this, &ProcessingController::handleResultsTimeCorrectedByAlpha);
    connect(m_worker, &ProcessingWorker::resultReadyAmpByGamma, this, &ProcessingController::handleResultsAmpByGamma);
    connect(m_worker, &ProcessingWorker::resultReadyProcessing, this, &ProcessingController::handleResultsProcessing);

    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_thread->start();
}

ProcessingController::~ProcessingController() {
    m_thread->quit();
    m_thread->wait();
}

