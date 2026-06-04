#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

#include "worker.h"

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(const QString &);
    ~Controller() override;
//public slots:
signals:
    void handleResultsTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &);
    void handleResultsAmpByGamma(const QMap<QString, QList<QPointF>> &);
signals:
    void operateS();
};

#endif // CONTROLLER_H
