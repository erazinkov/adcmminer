#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QThread>

#include "filewatcherworker.h"
#include "processingworker.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(const QString &);
    ~Controller() override;

public slots:
    void operateTimer(bool checked);

signals:
    void handleResultsReadyCheck(const QString &);

    void operateCheck();
    void operatePath(const QString &);

    void handleResultsTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &);
    void handleResultsAmpByGamma(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text);
    void handleResultsProcessing(const QMap<QString, double> &data);

    void operateS(const QString &);
    void operateR();

private:
    QTimer *m_timer;
    QThread *m_fileWatcherThread;
    FileWatcherWorker *m_fileWatcherWorker;
    QThread *m_processingThread;
    ProcessingWorker *m_processingWorker;
};

#endif // CONTROLLER_H
