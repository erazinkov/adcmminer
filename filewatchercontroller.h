#ifndef FILEWATCHERCONTROLLER_H
#define FILEWATCHERCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "filewatcherworker.h"

class FileWatcherController : public QObject
{
    Q_OBJECT
public:
    FileWatcherController(const QString &);
    ~FileWatcherController() override;

signals:
    void handleResultsReadyFileCheck(const QString &, const bool &);

    void operateFC();
    void operateP(const QString &);

private:
    QThread *m_thread;
    QTimer *m_timer;
    FileWatcherWorker *m_worker;
};

#endif // FILEWATCHERCONTROLLER_H
