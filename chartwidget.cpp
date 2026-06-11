#include "chartwidget.h"

#include <QFile>

#include <algorithm>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget{parent}
{
    m_sL = {
        "<h1><font color='#e74c3c'>?</font></h1>", "<h2>?</h2>", "<h2>?</h2>"
    };
    setAttribute(Qt::WA_Hover);
    installEventFilter(this);
    // Create chart view with the chart
//    setStyleSheet("background-color: #e74c3c;");
//   setStyleSheet(
//        "QWidget {"
//        "    background-color: transparent;"
//        "    border: 1px solid #ddd;"
//        "    border-radius: 5px;"
//        "}"
//        "QWidget:hover {"
//        "    border: 1px solid #3498db;"
//        "    background-color: #f8f9fa;"
//        "}"
//    );

    m_chart = new QChart;
    m_chartView = new ChartView(m_chart, this);
//    m_label = new QLabel(this);
//    m_label->setContentsMargins(0, 0, 0, 0);
//    m_label->setText(
//                "<table width='100%' style='border-collapse: collapse;'>"
//                     "<tr>"
//                     "<td align='center'>"
//                     "<b><font color='#e74c3c'>1</font></b>"
//                     "</td>"
//                     "<td align='center'>"
//                     "<b>123123</b>"
//                     "</td>"
//                     "<td align='center'>"
//                     "<b>1%</b>"
//                     "</td>"
//                     "</tr>"
//                     "</table>"
//                );

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
//    m_mainLayout->setContentsMargins(0, 0, 0, 0);
//    m_mainLayout->setSpacing(0);
    // Set the title and show legend
//    m_chart->setAnimationOptions(QChart::AnimationOption::NoAnimation);
//    m_chart->setTitle("Legendmarker example (click on legend)");
    m_chart->legend()->setVisible(false);
//    m_chart->legend()->setAlignment(Qt::AlignRight);
    m_chartView->setRenderHint(QPainter::Antialiasing);

}



void ChartWidget::removeSeries()
{
    while (m_chart->series().count() > 0) {
        QAbstractSeries *series = m_chart->series().last();
        m_chart->removeSeries(series);
        delete series;
    }
}


void ChartWidget::setSeries(QList<QAbstractSeries *> series)
{
    for (const auto &item : series)
    {
        m_chart->addSeries(item);
    }
}

void ChartWidget::setHeader(const QStringList &newData)
{
    for (auto i{0}; i < std::min(m_headerLabels.size(), newData.size()); i++) {
        m_headerLabels[i]->setText(newData.at(i));
    }
//    m_label->setText(newText);
}

void ChartWidget::connectMarkers()
{
    // Connect all markers to handler
    foreach (QLegendMarker* marker, m_chart->legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void ChartWidget::disconnectMarkers()
{
    foreach (QLegendMarker *marker, m_chart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void ChartWidget::handleMarkerClicked()
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

void ChartWidget::process(QList<QAbstractSeries *> series)
{
    disconnectMarkers();
    removeSeries();
    setSeries(series);
    connectMarkers();

    if (m_chart->series().size()) {
        m_chart->createDefaultAxes();
    }
}

void ChartWidget::setTitle(const QString &title)
{
    m_chart->setTitle(title);
}
