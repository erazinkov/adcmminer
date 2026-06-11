#ifndef PROCESSINGCONTROLLER_H
#define PROCESSINGCONTROLLER_H

#include <QObject>
#include <QThread>

#include "processingworker.h"

class ProcessingController : public QObject
{
    Q_OBJECT
public:
    ProcessingController();
    ~ProcessingController() override;
signals:
    void handleResultsTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &);
    void handleResultsAmpByGamma(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text);

    void operateS(const QString &);
    void operateR();
private:
    QThread *m_thread;
    ProcessingWorker *m_worker;
};

#endif // PROCESSINGCONTROLLER_H
