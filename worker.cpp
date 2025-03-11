#include "worker.h"

Worker::Worker(const QString &path, const ChannelMap &pre) : m_pre(pre)
{
    m_decoder = new Decoder(path.toStdString(), m_pre);
    m_calibration = new Calibration(m_pre);
    m_dataDelegate = new DataDelegate;
}

void Worker::doWork(const QString &parameter, Enums::Type type) {

    m_decoder->process();
    const auto start = std::chrono::steady_clock::now();
//    m_data.insert(m_data.cend(), m_decoder->events().cbegin(), m_decoder->events().cend());

    m_calibration->setNewEvents(m_decoder->events());
    m_calibration->process();

    doDataDelegateWork(parameter, type);
    const auto stop = std::chrono::steady_clock::now();
    std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    /* ... here is the expensive or blocking operation ... */
}

void Worker::doWork1(const QString &parameter, Enums::Type type) {
    doDataDelegateWork(parameter, type);
    /* ... here is the expensive or blocking operation ... */
}

void Worker::doDataDelegateWork(const QString &parameter, Enums::Type type)
{
    /* ... here is the expensive or blocking operation ... */
    bool ok;
    auto i = parameter.toUInt(&ok);
    auto idx{ ok ? i : 0};

    QMap<QString, QList<QPointF>> data;

    for (ulong i{0}; i < m_pre.numberOfChannelsAlpha(); ++i)
    {
        TH1 *h;
        switch (type) {
            case Enums::Type::AMP:
                h = m_calibration->histsAmp()[idx][i];
                break;
            case Enums::Type::TIME:
                h = m_calibration->hists()[idx][i];
                break;
        }
        m_dataDelegate->histToData(h);
        for (auto j = m_dataDelegate->data().cbegin(), end = m_dataDelegate->data().cend(); j != end; ++j)
        {
            data.insert(j.key(), j.value());
        }
    }

    emit resultReady(data);
}
