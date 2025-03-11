#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include <QtCharts/QChartGlobal>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>


#include "mousepresseater.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

public slots:
    void process(QList<QAbstractSeries *>);
    void setTitle(const QString &);

private slots:
    void connectMarkers();
    void disconnectMarkers();
    void removeSeries();
    void handleMarkerClicked();
    void setSeries(QList<QAbstractSeries *>);

private:

    QChart *m_chart;

    QChartView *m_chartView;
    QGridLayout *m_mainLayout;
    QGridLayout *m_fontLayout;



    MousePressEater *m_mousePressEater;

};

#endif // MAINWIDGET_H
