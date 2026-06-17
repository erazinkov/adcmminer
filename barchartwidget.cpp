#include "barchartwidget.h"

BarChartWidget::BarChartWidget(const QString &title, QWidget *parent)
    : QWidget{parent}
{
    m_chart = new QChart();
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    m_set = new QBarSet(title);
    m_series = new QBarSeries();
    m_chart->addSeries(m_series);
    m_series->append(m_set);

    m_chartView = new QChartView(m_chart, this);
    m_mainLayout = new QVBoxLayout(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_mainLayout->addWidget(m_chartView);

    m_axisX = new QBarCategoryAxis();
    m_axisX->append(QStringList());
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    m_axisY = new QValueAxis();
    m_axisY->setRange(0, 1);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);
}
