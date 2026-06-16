#include "processingworker.h"

#include "constants.h"

ProcessingWorker::ProcessingWorker(QObject *parent)
    : QObject{parent}
{
    m_decoder = new Decoder;
    m_histogramManager = new HistogramManager(AppConstants::MAX_GAMMA_NUMBER, AppConstants::MAX_ALPHA_NUMBER);
    m_calibration = new Calibration(m_histogramManager);
    m_dataDelegate = new DataDelegate;

}

void ProcessingWorker::doWorkS(const QString &path)
{
    QMap<QString, double> m;
    auto start = std::chrono::steady_clock::now();
    std::cout << "Started!" << std::endl;
    m_decoder->process(path.toStdString());

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
    m.insert("Decode", std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());

    start = std::chrono::steady_clock::now();
    m_calibration->setNewEvents(m_decoder->events_m(), m_decoder->channels());
    stop = std::chrono::steady_clock::now();
    std::cout << "setNewEventsM Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    start = std::chrono::steady_clock::now();
    m_calibration->process();
    stop = std::chrono::steady_clock::now();
    std::cout << "process Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    m.insert("Calibration", std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
    start = std::chrono::steady_clock::now();
    histToPointsTimeCorrectedByAlpha();
    histToPointsAmpByGamma();
    stop = std::chrono::steady_clock::now();
    std::cout << "doDataDelegateWork Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    m.insert("Hists", std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
    emit resultReadyProcessing(m);
}

void ProcessingWorker::doWorkReset()
{
    m_calibration->resetEvents();
    m_histogramManager->resetAll();
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
    QMap<QString, QStringList> text;
    for (ulong i{0}; i < m_decoder->channels().a.size(); ++i) {
        TH1 *h;
        h = m_histogramManager->histsTimeCorrectedByAlpha()[i];
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
        {
            data.insert(j.key(), j.value());
            text.insert(j.key(), {
                            QString("<span><font color='red'>%1</font></span>").arg(i)
                        });
        }
        h = nullptr;
    }
    emit resultReadyTimeCorrectedByAlpha(data, text);
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
        h = m_histogramManager->histsEnergyByAlpha()[i];
        s += h->Integral();
        h = nullptr;
    }

    for (ulong i{0}; i < m_decoder->channels().a.size(); ++i) {
        TH1 *h;
        h = m_histogramManager->histsEnergyByAlpha()[i];
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j) {
            data.insert(j.key(), j.value());
            text.insert(j.key(), {
                            QString("<span><font color='red'>%1</font></span>").arg(i),
                            QString("<span>%1</span>").arg(qRound(h->Integral())),
                            QString("<span>%1%</span>").arg(qRound(100.0 * h->Integral() / (s < 1.0 ? 1.0 : s )))
                        });
            h = nullptr;
        }
    }

    emit resultReadyEnergyByAlpha(data, text);
}
