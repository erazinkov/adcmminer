#include "worker.h"

Worker::Worker(const QString &path)
{
    m_decoder = new Decoder(path.toStdString());
    m_calibration = new Calibration;
    m_dataDelegate = new DataDelegate;
}

void Worker::doWorkS()
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
    m_calibration->setNewEventsM(m_decoder->events_m(), m_decoder->channels());
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

QVector<QPointF> Worker::histToPoints(TH1D *hist) {
    QVector<QPointF> points;
    if (!hist) return points;
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double x = hist->GetBinCenter(bin);
        double y = hist->GetBinContent(bin);
        points.append(QPointF(x, y));
    }
    return points;
}

void Worker::histToPointsTimeCorrectedByAlpha()
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

void Worker::histToPointsAmpByGamma()
{
    QMap<QString, QList<QPointF>> data;
    for (ulong i{0}; i < m_decoder->channels().g.size(); ++i) {
        TH1 *h;
        h = m_calibration->histogramManager->histsAmpByGamma()[i];
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
        {
            data.insert(j.key(), j.value());
        }
    }

    emit resultReadyAmpByGamma(data);
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
