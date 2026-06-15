#include "histchartwidget.h"

#include <QHBoxLayout>
#include <QValueAxis>

HistChartWidget::HistChartWidget(const double xMin, const double xMax, QWidget *parent) : QWidget{parent}
{
    m_sL = {
        "<h2><font color='red'>?</font></h2>", "<h3>?</h3>", "<h3>?</h3>"
    };
    m_chart = new QChart;
    m_chartView = new ChartView(m_chart, this);
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    header->setLayout(headerLayout);
    for (auto &s : m_sL) {
        m_headerLabels.push_back(new QLabel(s));
        headerLayout->addWidget(m_headerLabels.last());
    }
    headerLayout->insertStretch(1, 1);
    QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(m_chartView);
    m_chart->legend()->setVisible(false);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    m_lineSeries = new QLineSeries();
    m_areaSeries = new QAreaSeries(m_lineSeries);
    auto color{QColorConstants::Green};
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColorConstants::Green);
    m_areaSeries->setPen(pen);
    m_areaSeries->setColor(color);
    QLinearGradient dataGradient(QPointF(0, 0), QPointF(0, 1));
    dataGradient.setColorAt(0.0, color);
    dataGradient.setColorAt(1.0, color.lighter());
    dataGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    m_areaSeries->setOpacity(0.75);
    m_areaSeries->setBrush(dataGradient);
    m_areaSeries->setColor(color);

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(xMin, xMax);
    axisX->setTickCount(10);

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_areaSeries->attachAxis(axisX);
}

void HistChartWidget::setHeader(const QStringList &newData)
{
    for (auto i{0}; i < std::min(m_headerLabels.size(), newData.size()); i++) {
        m_headerLabels[i]->setText(newData.at(i));
    }
}

void HistChartWidget::setData(const QString &title, QList<QPointF> &points)
{
    m_chart->setTitle(title);
    m_lineSeries->replace(points);
}
