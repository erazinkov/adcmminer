#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

#include "worker.h"
#include "channelmap.h"

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(const QString &, const ChannelMap &);
    ~Controller() override;
//public slots:
signals:
    void handleResults(const QMap<QString, QList<QPointF>> &);
signals:
    void operate(const QString &, Enums::Type type = Enums::Type::TIME);
    void operate1(const QString &, Enums::Type type = Enums::Type::TIME);
};

#endif // CONTROLLER_H
