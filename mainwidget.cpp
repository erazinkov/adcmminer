#include "mainwidget.h"

#include <QFile>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    // Create chart view with the chart
    m_chart = new QChart();
    m_chartView = new QChartView(m_chart, this);

    // Create layout for grid and detached legend
    m_mainLayout = new QGridLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_chartView, 0, 1, 3, 1);
    setLayout(m_mainLayout);

    // Set the title and show legend
    m_chart->setAnimationOptions(QChart::AnimationOption::NoAnimation);
    m_chart->setTitle("Legendmarker example (click on legend)");
    m_chart->legend()->setVisible(false);
    m_chart->legend()->setAlignment(Qt::AlignRight);

    m_chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWidget::removeSeries()
{
    while (m_chart->series().count() > 0) {
        QAbstractSeries *series = m_chart->series().last();
        m_chart->removeSeries(series);
        delete series;
    }
}


void MainWidget::setSeries(QList<QAbstractSeries *> series)
{
    for (const auto &item : series)
    {
        m_chart->addSeries(item);
    }
}

void MainWidget::connectMarkers()
{
    // Connect all markers to handler
    foreach (QLegendMarker* marker, m_chart->legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void MainWidget::disconnectMarkers()
{
    foreach (QLegendMarker *marker, m_chart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void MainWidget::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    if (marker->type() == QLegendMarker::LegendMarkerTypeXY || marker->type() == QLegendMarker::LegendMarkerTypeArea)
    {
        // Toggle visibility of series
        marker->series()->setVisible(!marker->series()->isVisible());

        // Turn legend marker back to visible, since hiding series also hides the marker
        // and we don't want it to happen now.
        marker->setVisible(true);

        // Dim the marker, if series is not visible
        qreal alpha = 1.0;

        if (!marker->series()->isVisible()) {
            alpha = 0.5;
        }

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);
    }
}

void MainWidget::process(QList<QAbstractSeries *> series)
{
    disconnectMarkers();
    removeSeries();
    setSeries(series);
    connectMarkers();

    if (m_chart->series().size()) {
        m_chart->createDefaultAxes();
    }
}

void MainWidget::setTitle(const QString &title)
{
    m_chart->setTitle(title);
}
