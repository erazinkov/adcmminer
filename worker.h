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
    Worker(const QString &);
public slots:
    void doWorkS();

signals:
    void resultReadyTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &);
    void resultReadyAmpByGamma(const QMap<QString, QList<QPointF>> &);
private:
    Decoder *m_decoder;
    DataDelegate *m_dataDelegate;
    Calibration *m_calibration;

    void histToPointsTimeCorrectedByAlpha();
    void histToPointsAmpByGamma();

    QVector<QPointF> histToPoints(TH1D *hist);
};

#endif // WORKER_H
