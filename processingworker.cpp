#include "processingworker.h"

ProcessingWorker::ProcessingWorker(const QString &path, QObject *parent)
    : QObject{parent}
{
    m_decoder = new Decoder(path.toStdString());
    m_calibration = new Calibration;
    m_dataDelegate = new DataDelegate;
}

void ProcessingWorker::doWorkS()
{
    auto start = std::chrono::steady_clock::now();
    std::cout << "Started!" << std::endl;
    m_decoder->process();
//    std::cout << "Events with 2 pulses: " << m_decoder->events().size() << std::endl;
//    std::cout << "Events with 1 pulse: " << m_decoder->events_1().size() << std::endl;
//    std::cout << "Counters: " << m_decoder->counters().rawhits.size() << " " << m_decoder->counters().time << std::endl;
//    for (size_t i{0}; i < m_decoder->counters().rawhits.size(); ++i)
//    {
//        std::cout << m_decoder->counters().rawhits.at(i) << " ";
//    }
//    std::cout << std::endl;
//    for (const auto &item : m_decoder->channels().a)
//    {
//        std::cout << static_cast<int>(item) << " ";
//    }
//    std::cout << std::endl;
    std::cout << "Finished!" << std::endl;
    auto stop = std::chrono::steady_clock::now();
    std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
//    m_data.clear();
//    m_data.insert(m_data.cend(), m_decoder->events().cbegin(), m_decoder->events().cend());
//    m_calibration->setNewEvents(m_data, m_decoder->channels());
    start = std::chrono::steady_clock::now();
    m_calibration->setNewEvents(m_decoder->events_m(), m_decoder->channels());
    stop = std::chrono::steady_clock::now();
    std::cout << "setNewEventsM Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    start = std::chrono::steady_clock::now();
    m_calibration->process();
    stop = std::chrono::steady_clock::now();
    std::cout << "process Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    start = std::chrono::steady_clock::now();
    histToPointsTimeCorrectedByAlpha();
    histToPointsAmpByGamma();
    stop = std::chrono::steady_clock::now();
    std::cout << "doDataDelegateWork Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
}

void ProcessingWorker::doWorkR()
{
    m_calibration->resetEvents();
//    m_calibration->process();
    histToPointsTimeCorrectedByAlpha();
    histToPointsAmpByGamma();
}

QVector<QPointF> ProcessingWorker::histToPoints(TH1D *hist) {
    QVector<QPointF> points;
    if (!hist) return points;
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double x = hist->GetBinCenter(bin);
        double y = hist->GetBinContent(bin);
        points.append(QPointF(x, y));
    }
    return points;
}

void ProcessingWorker::histToPointsTimeCorrectedByAlpha()
{

    QMap<QString, QList<QPointF>> data;
    for (ulong i{0}; i < m_decoder->channels().a.size(); ++i) {
        TH1 *h;
        h = m_calibration->histogramManager->histsTimeCorrectedByAlpha()[i];
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
        {
            data.insert(j.key(), j.value());
        }
        h = nullptr;
    }
    emit resultReadyTimeCorrectedByAlpha(data);
}

void ProcessingWorker::histToPointsAmpByGamma()
{
//    QMap<QString, QList<QPointF>> data;
//    QMap<QString, QString> text;
//    for (ulong i{0}; i < m_decoder->channels().g.size(); ++i) {
//        TH1 *h;
//        h = m_calibration->histogramManager->histsEnergyByGamma()[i];
//        m_dataDelegate->histToData(h);
//        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
//        {
//            data.insert(j.key(), j.value());
//            text.insert(j.key(), QString("<b>%1</b><br/>").arg(qRound(h->Integral())));

//        }
//    }

//    emit resultReadyAmpByGamma(data, text);

    QMap<QString, QList<QPointF>> data;
    QMap<QString, QStringList> text;
    double s{0.0};
    for (ulong i{0}; i < m_decoder->channels().a.size(); ++i) {
        TH1 *h;
        h = m_calibration->histogramManager->histsEnergyByAlpha()[i];
        s += h->Integral();
        h = nullptr;
    }

    for (ulong i{0}; i < m_decoder->channels().a.size(); ++i) {
        TH1 *h;
        h = m_calibration->histogramManager->histsEnergyByAlpha()[i];
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j) {
            data.insert(j.key(), j.value());
            text.insert(j.key(), {
                            QString("<h2><font color='red'>%1</font></h2>").arg(i),
                            QString("<h3>%1</h3>").arg(qRound(h->Integral())),
                            QString("<h3>%1</h3>").arg(qRound(100.0 * h->Integral() / (s < 1.0 ? 1.0 : s )))
                        });
            h = nullptr;
        }
    }

    emit resultReadyAmpByGamma(data, text);
}

//    void Worker::doDataDelegateWork(const QString &parameter)
//    {
//        /* ... here is the expensive or blocking operation ... */
//        bool ok;
//        auto i = parameter.toUInt(&ok);
//        auto idx{ ok ? i : 0};

//        QMap<QString, QList<QPointF>> data;

//        for (ulong i{0}; i < m_pre.numberOfChannelsAlpha(); ++i)
//        {
//            TH1 *h;
//            switch (type) {
//                case Enums::Type::AMP:
//                    h = m_calibration->histsAmp()[idx][i];
//                    break;
//                case Enums::Type::TIME:
//                    h = m_calibration->hists()[idx][i];
//                    break;
//            }
//            m_dataDelegate->histToData(h);
//            for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
//            {
//                data.insert(j.key(), j.value());
//            }
//        }

//        emit resultReady(data);
//    }

//void Worker::doWork(const QString &parameter, Enums::Type type) {

//    m_decoder->process();
//    const auto start = std::chrono::steady_clock::now();
//    m_data.insert(m_data.cend(), m_decoder->events().cbegin(), m_decoder->events().cend());

////    m_calibration->setNewEvents(m_decoder->events());
//    m_calibration->setNewEvents(m_data);
//    m_calibration->process();

//    doDataDelegateWork(parameter, type);
//    const auto stop = std::chrono::steady_clock::now();
//    std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
//}
