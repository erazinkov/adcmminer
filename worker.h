#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "decoder.h"
#include "datadelegate.h"
#include "calibration.h"

class Enums {
public:
    enum class Type
    {
        TIME,
        AMP,
    };
    Q_ENUM(Type)

    Q_GADGET
    Enums() = delete;
};

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(const QString &, const ChannelMap &);
public slots:
    void doWork(const QString &parameter, Enums::Type type);
    void doWork1(const QString &parameter, Enums::Type type);

signals:
    void resultReady(const QMap<QString, QList<QPointF>> &);
private:
    ChannelMap m_pre;
    Decoder *m_decoder;
    DataDelegate *m_dataDelegate;
    Calibration *m_calibration;
    std::vector<dec_ev_t> m_data;

    void doDataDelegateWork(const QString &parameter, Enums::Type type = Enums::Type::TIME);

};

#endif // WORKER_H
